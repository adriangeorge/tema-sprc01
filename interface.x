/*
    Define client request structures
*/

/*
client_req_op
    c_id        :   id of client
*/
struct client_req_auth {
    string c_id<20>;
};

/*
client_req_approve_token
    
    c_token     :   token of client
    c_op        :   operation requested by client
    c_arg       :   argument for operation requested by client 
                Note: defined as string for both token requests and actions
*/
struct client_req_approve_token {
    string c_id<20>;
    string c_op<20>;
    string c_arg<20>;
};

/*
    Define server response structures
*/
/*
server_res_token
    token   : token returned by server
*/
struct server_res_token {
    string token<20>;
};

program SPRC_HW {
	version SPRC_HW_VER {
		server_res_token req_auth(client_req_auth)                    = 1;
		server_res_token approve_req_token(client_req_auth)           = 2;
		server_res_token req_bearer_token(client_req_auth)            = 3;
		server_res_token req_bearer_token_refresh(client_req_auth)    = 4;
		server_res_token validate_delegated_action(client_req_auth)   = 5;

	} = 1;
} = 123456789;