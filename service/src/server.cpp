/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "server.h"
#include "../../token.h"
#include "interface.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

map<string, user> user_db;
map<string, token> token_db;
map<string, string> access_token_db;
set<string> resource_db;

vector<map<string, unsigned char>> user_approvals;
int current_approval_index = 0;
int token_availability = 0;

char *sign_tk(char *tk)
{
	char *stk = (char *)malloc(sizeof(char) * 32);
	memset(stk, 0, 32);
	strcat(stk, tk);
	strcat(stk, "C31M41C14r3NUN7");
	return stk;
}
char *unsign_tk(char *tk)
{
	char *ustk = (char *)malloc(sizeof(char) * TOKEN_LEN);
	memset(ustk, 0, TOKEN_LEN);
	memcpy(ustk, tk, TOKEN_LEN);
	return ustk;
}
void server_init(string client_f, string resource_f, string approval_f,
		 int avail)
{
	// Set token availabilty
	token_availability = avail;
	// Read and store client info
	ifstream client_file(client_f);

	int client_count;
	client_file >> client_count;
	string user_id;
	while (client_file >> user_id) {
		user new_user;
		new_user.acc_token = "N/A";
		user_db.insert(pair<string, user>(user_id, new_user));

#ifdef __DEBUG__
		cout << "[UserDB] Inserted " << user_id << " with token "
		     << user_db.at(user_id).curr_token << endl;
#endif
	}

	client_file.close();

	// Read and store resource info
	ifstream resource_file(resource_f);
	int res_count;
	resource_file >> res_count;
	string res_name;
	while (resource_file >> res_name) {
		resource_db.insert(res_name);

#ifdef __DEBUG__
		cout << "[ResDB] Inserted " << *(resource_db.find(res_name))
		     << endl;
#endif
	}

	resource_file.close();

	// Read and store approval info
#ifdef __DEBUG__
	cout << "[PermDB] Begin " << endl;
#endif
	ifstream approval_file(approval_f);
	string app_name;
	while (approval_file >> app_name) {
		map<string, unsigned char> new_approval;

		stringstream s(app_name);
		string csv_res_name, csv_perm;

		// For each specified permission grab a pair of
		// (res_name, perm_str) And determine permission value
		while (getline(s, csv_res_name, ',')) {
			// If no permissions are allowed no point in assigning
			// to permission map

			unsigned char new_perm = NONE;
			getline(s, csv_perm, ',');

			// Search each permission type and perform binary OR
			for (int i = 0; i < csv_perm.length(); i++) {
				if (csv_perm[i] == 'R') {
					new_perm |= READ;
				} else if (csv_perm[i] == 'I') {
					new_perm |= INSERT;
				} else if (csv_perm[i] == 'M') {
					new_perm |= MODIFY;
				} else if (csv_perm[i] == 'D') {
					new_perm |= DELETE;
				} else if (csv_perm[i] == 'X') {
					new_perm |= EXECUTE;
				}
			}

			auto perm_pair =
			    pair<string, unsigned char>(csv_res_name, new_perm);

			new_approval.insert(perm_pair);
#ifdef __DEBUG__
			cout << "[" << csv_res_name << "] | ["
			     << to_string(new_approval.at(csv_res_name)) << "]"
			     << endl;
#endif
		}
		user_approvals.push_back(new_approval);
	}
#ifdef __DEBUG__
	cout << "[PermDB] Finished " << endl;
#endif

	approval_file.close();
}

server_res_token *req_auth_1_svc(client_req_auth *argp, struct svc_req *rqstp)
{
	static server_res_token result;

	memset(&result, 0, sizeof(server_res_token));
	// Print begin message
	cout << "BEGIN " << argp->c_id << " AUTHZ" << endl;

	// Populate unneeded fields
	result.ref_token = "N/A";
	result.token_ttl = 0;

	// Check if user in db
	if (!user_db.count(argp->c_id)) {
		result.status = status_code::USER_NOT_FOUND;
		result.token = "N/A";
		return &result;
	}
	// Generate auth token
	// f(id_user)
	result.token = generate_access_token(argp->c_id);
	result.status = status_code::OK;
	cout << "  RequestToken = " << result.token << endl;
	return &result;
}

server_res_token *approve_req_token_1_svc(client_req_signature *argp,
					  struct svc_req *rqstp)
{
	static server_res_token result;
	memset(&result, 0, sizeof(server_res_token));

	// Grab permission set
	auto approval = user_approvals.at(current_approval_index++);
	if (current_approval_index >= user_approvals.size())
		current_approval_index = 0;

	// Check if end user allows any permissions for this resource
	if (approval.count("*")) {
		result.token_ttl = 0;
		result.ref_token = "N/A";
		result.status = status_code::REQUEST_DENIED;
		result.token = argp->request_token;
		return &result;
	}

	// Attach permissions to token
	token new_tk;
	new_tk.perms = approval;

	token_db.insert({argp->request_token, new_tk});

	// Sign token
	result.token = sign_tk(argp->request_token);

	// Populate rest of response fields
	result.ref_token = "N/A";
	result.token_ttl = 0;
	result.status = status_code::OK;

	return &result;
}

server_res_token *req_bearer_token_1_svc(client_req_bearer_token *argp,
					 struct svc_req *rqstp)
{
	static server_res_token result;
	memset(&result, 0, sizeof(server_res_token));

	string signed_tk(argp->c_auth_token);

	// Populate rest of response fields
	result.token = generate_access_token(
	    (char *)signed_tk.substr(0, TOKEN_LEN).c_str());

	result.ref_token = "N/A";
	result.token_ttl = token_availability;
	result.status = status_code::OK;

	// Associate user_id with access token
	access_token_db.insert({result.token, argp->c_id});

	// Associate user_id with auth token
	string unsigned_tk(unsign_tk(argp->c_auth_token));
	token_db.at(unsigned_tk).user_id = argp->c_id;

	// Update user token info
	user_db.at(argp->c_id).acc_token = result.token;
	user_db.at(argp->c_id).ref_token = "N/A";
	user_db.at(argp->c_id).auth_token = unsigned_tk;
	user_db.at(argp->c_id).token_ttl = result.token_ttl;

	cout << "  AccessToken = " << result.token << endl;
	return &result;
}

server_res_token *req_bearer_token_refresh_1_svc(client_req_bearer_token *argp,
						 struct svc_req *rqstp)
{
	static server_res_token result;
	memset(&result, 0, sizeof(server_res_token));

	if (strcmp(argp->c_refresh_token, "N/A") != 0) {
		// Generate access token based on automatic refesh token
		result.token = generate_access_token(argp->c_refresh_token);
	} else {
		// Generate access token using signed auth token
		result.token =
		    generate_access_token(unsign_tk(argp->c_auth_token));
	}
	// Generate refresh token using access token
	result.ref_token = generate_access_token(result.token);

	// Populate rest of response fields
	result.token_ttl = token_availability;
	result.status = status_code::OK;

	// Associate user_id with access token
	access_token_db.insert({result.token, argp->c_id});

	// Associate user_id with auth token
	string unsigned_tk(unsign_tk(argp->c_auth_token));
	token_db.at(unsigned_tk).user_id = argp->c_id;

	// Update user token info
	user_db.at(argp->c_id).acc_token = result.token;
	user_db.at(argp->c_id).ref_token = result.ref_token;
	user_db.at(argp->c_id).auth_token = unsigned_tk;
	user_db.at(argp->c_id).token_ttl = result.token_ttl;

	cout << "  AccessToken = " << result.token << endl
	     << "  RefreshToken = " << result.ref_token << endl;
	return &result;
}

server_res_op *validate_delegated_action_1_svc(client_req_op *argp,
					       struct svc_req *rqstp)
{
	static server_res_op result;
	memset(&result, 0, sizeof(server_res_op));

	// Check if access token is valid
	if (access_token_db.count(argp->c_access_token)) {
		// Check user associated with this token
		string user_id = access_token_db.at(argp->c_access_token);
		if (!user_db.count(user_id)) {
			result.status = status_code::PERMISSION_DENIED;
			return &result;
		}
		// Check that token actually matches with user's current active
		// token
		user u = user_db.at(user_id);
		if (u.acc_token != string(argp->c_access_token)) {
			result.status = status_code::PERMISSION_DENIED;
			return &result;
		}
		// Check token ttl
		if (u.token_ttl == 0) {
			result.status = status_code::TOKEN_EXPIRED;
			return &result;
		} else {
			// Decrement token lifetime
			user_db.at(user_id).token_ttl--;
		}
		// Check if resource exists
		if (!resource_db.count(argp->resource)) {
			result.status = status_code::RESOURCE_NOT_FOUND;
			return &result;
		}
		// Check if user has permissions
		string user_auth_tk = user_db.at(user_id).auth_token;

		auto res_perms_map = token_db.at(user_auth_tk).perms;
		if (!res_perms_map.count(argp->resource)) {
			result.status = status_code::OPERATION_NOT_PERMITTED;
			return &result;
		}

		unsigned char perms = res_perms_map.at(argp->resource);

		// Get requested operation
		unsigned char req_op_perm;
		string req_op_str(argp->op);
		if (req_op_str == "READ")
			req_op_perm = perm_flag::READ;
		if (req_op_str == "MODIFY")
			req_op_perm = perm_flag::MODIFY;
		if (req_op_str == "INSERT")
			req_op_perm = perm_flag::INSERT;
		if (req_op_str == "DELETE")
			req_op_perm = perm_flag::DELETE;
		if (req_op_str == "EXECUTE")
			req_op_perm = perm_flag::EXECUTE;
		if (!(req_op_perm & perms)) {
			result.status = status_code::OPERATION_NOT_PERMITTED;
			return &result;
		}
	}

	result.status = status_code::PERMISSION_GRANTED;
	return &result;
}
