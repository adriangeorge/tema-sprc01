/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "interface.h"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

using namespace std;
enum status_code {
	OK,
	USER_NOT_FOUND,
	REQUEST_DENIED,
	TOKEN_EXPIRED,
	OPERATION_NOT_PERMITTED,
	RESOURCE_NOT_FOUND,
	PERMISSION_DENIED,
	PERMISSION_GRANTED
};

string status_code_str[] = {
    "SUCCESS",		 "USER_NOT_FOUND",	    "REQUEST_DENIED",
    "TOKEN_EXPIRED",	 "OPERATION_NOT_PERMITTED", "RESOURCE_NOT_FOUND",
    "PERMISSION_DENIED", "PERMISSION_GRANTED"};

struct token_cl {
	int ttl;
	string token_request;
	string token_access;
	string token_refresh;
};

map<string, token_cl> user_db;
string curr_token;
int curr_token_ttl;
string input_file;

CLIENT *clnt;

void bearer_authorization(string user_id, int arg, int automatic)
{
	server_res_token *res;
	// Populate request body
	client_req_bearer_token req;

	req.c_auth_token = (char *)user_db.at(user_id).token_request.c_str();
	req.c_id = (char *)user_id.c_str();
	// Check if this bearer request has been triggered by automatic refresh
	if (automatic) {
		req.c_refresh_token =
		    (char *)user_db.at(user_id).token_refresh.c_str();

	} else {
		req.c_refresh_token = "N/A";
	}

	// Send request
	if (arg) {
		// cout << "Send signed req_tk get acc_tk, ref_tk\n";
		res = req_bearer_token_refresh_1(&req, clnt);
	} else {
		// cout << "Send signed req_tk get acc_tk\n";
		res = req_bearer_token_1(&req, clnt);
	}

	if (res == (server_res_token *)NULL) {
		printf("%d :", __LINE__);
		clnt_perror(clnt, "call failed");
	} else if (res->status != status_code::OK) {
		cout << status_code_str[res->status] << endl;
		return;
	} else {
		user_db.at(user_id).ttl = res->token_ttl;
		user_db.at(user_id).token_access = res->token;
		if (arg)
			user_db.at(user_id).token_refresh = res->ref_token;
	}
}

void authorization(string user_id, int arg)
{

	server_res_token *res;
	// Get initial request token
	client_req_auth req;
	req.c_id = (char *)user_id.c_str();
	// cout << "Send cid get req_tk\n";
	res = req_auth_1(&req, clnt);

	//  Error checking
	if (res == (server_res_token *)NULL) {
		printf("%d :", __LINE__);
		clnt_perror(clnt, "call failed");
	} else if (res->status != status_code::OK) {
		cout << status_code_str[res->status] << endl;
		return;
	}

	//  Get signed request token
	client_req_signature req_approve;
	req_approve.request_token = res->token;
	// cout << "Send reqtk get signed reqtk\n";
	res = approve_req_token_1(&req_approve, clnt);
	if (res == (server_res_token *)NULL) {
		printf("%d :", __LINE__);
		clnt_perror(clnt, "call failed");
	}
	// Check if permissions were granted
	if (res->status != status_code::OK) {
		cout << status_code_str[res->status] << endl;
		return;
	} else {
		user_db.at(user_id).token_request = res->token;
	}

	// Get access token
	bearer_authorization(user_id, arg, 0);

	cout << user_db.at(user_id).token_request.substr(0, 15) << " -> "
	     << user_db.at(user_id).token_access;

	if (arg)
		cout << "," << user_db.at(user_id).token_refresh;

	cout << endl;
}

void execute_resource_op(string user_id, string op_type, string arg)
{
	server_res_op *res;
	client_req_op req;

	req.c_access_token = (char *)user_db.at(user_id).token_access.c_str();
	req.op = (char *)op_type.c_str();
	req.resource = (char *)arg.c_str();
	req.c_id = (char *)user_id.c_str();
	// Check for presence of refresh token and if an automatic refresh
	// is possible
	if (user_db.at(user_id).ttl == 0 &&
	    user_db.at(user_id).token_refresh != "N/A") {
		// Get new access token

		bearer_authorization(user_id, 1, 1);
	}

	res = validate_delegated_action_1(&req, clnt);
	if (res == (server_res_op *)NULL) {
		clnt_perror(clnt, "call failed");
	} else {
		cout << status_code_str[res->status] << endl;
		user_db.at(user_id).ttl--;
	}
}

void sprc_hw_1(char *host)
{

	clnt = clnt_create(host, SPRC_HW, SPRC_HW_VER, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror(host);
		exit(1);
	}

	ifstream req_file(input_file);

	// Process all requests in file
	string in_line;
	while (req_file >> in_line) {
		stringstream s(in_line);
		string user_id, operation_type, operation_arg;
		getline(s, user_id, ',');
		getline(s, operation_type, ',');
		getline(s, operation_arg, ',');
		// Check if user exists in users db
		if (!user_db.count(user_id)) {
			token_cl new_token;
			new_token.ttl = 0;
			new_token.token_access = "N/A";
			new_token.token_refresh = "N/A";
			new_token.token_request = "N/A";

			user_db.insert(
			    pair<string, token_cl>(user_id, new_token));
		}

		// Check operation type
		if (operation_type == "REQUEST") {

			authorization(user_id, atoi(operation_arg.c_str()));
		} else {

			execute_resource_op(user_id, operation_type,
					    operation_arg);
		}
	}
	req_file.close();

	clnt_destroy(clnt);
}

int main(int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf("usage: %s server_host\n", argv[0]);
		exit(1);
	}
	host = argv[1];
	input_file = argv[2];
	sprc_hw_1(host);
	exit(0);
}
