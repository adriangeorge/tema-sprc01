#include <map>
#include <set>
#include <string>
#include <vector>

#ifndef __SERVER__
#define __SERVER__

enum perm_flag {
	NONE = 0,
	READ = 1,
	MODIFY = 2,
	DELETE = 4,
	INSERT = 8,
	EXECUTE = 16
};
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
// Database structures
struct user {
	std::string acc_token;
	std::string auth_token;
	std::string ref_token;
	int token_ttl;
};

struct token {
	std::string user_id;
	// Key      : Resource Name
	// Value    : Encoded permissions for resource
	std::map<std::string, unsigned char> perms;
};

// Function that reads input files and populates databases
extern void server_init(std::string client_f, std::string resource_f,
			std::string approval_f, std::string avail_f);

// Databases

// Key      : User_ID
// Value    : User structure containing information about token
extern std::map<std::string, user> user_db;
// Key      : Access Token
// Value    : User Id
extern std::map<std::string, std::string> access_token_db;
// Key      : Auth Token
// Value    : Token Permissions Information
extern std::map<std::string, token> token_db;
// Set of available resources
extern std::set<std::string> resource_db;
// Array of user approvals for each resource
extern std::vector<std::map<std::string, unsigned char>> user_approvals;
extern int current_approval_index;
// Availability set upon newly generated tokens
extern int token_availability;

#endif