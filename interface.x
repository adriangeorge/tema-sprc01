/*
    Define client request structures
*/

/*
-----------------------------------------------------------------------------
client_req_auth
    c_id                :   id of client
-----------------------------------------------------------------------------
*/
struct client_req_auth {
    string c_id<>;
};

/*
-----------------------------------------------------------------------------
client_req_access
    c_id                :   id of client
    access_token        :   access token approved by end user
-----------------------------------------------------------------------------
*/
struct client_req_access {
    string c_id<>;
    string approve_token<>;
};

/*
-----------------------------------------------------------------------------
client_req_approve
    
    approve_token       :   token of client returned by authz
-----------------------------------------------------------------------------
*/
struct client_req_approve {
    string approve_token<>;
};

/*
-----------------------------------------------------------------------------
client_req_bearer_token
    
    c_access_token      :   token of client returned by authz
    c_refresh_token     :   token of client returned by authz
-----------------------------------------------------------------------------
*/
struct client_req_bearer_token {
    string c_access_token<>;
    string c_request_token<>;
};

/*
-----------------------------------------------------------------------------
client_req_op
    
    c_access_token      :   token of client returned by authz
    op                  :   desired operation
    resource            :   desired resource 
-----------------------------------------------------------------------------
*/
struct client_req_op {
    string c_access_token<>;
    string op<>;
    string resource<>;
};

/*
    Define server response structures
*/
/*
-----------------------------------------------------------------------------
server_res_token
    status              : status of response, stores error codes
                        0 = SUCCESS
                        1 = USER_NOT_FOUND
                        2 = REQUEST_DENIED
                        3 = TOKEN_EXPIRED
                        4 = OPERATION_NOT_PERMITTED
                        5 = RESOURCE_NOT_FOUND
                        6 = PERMISSION_DENIED
                        7 = PERMISSION_GRANTED
    token   : token returned by server
    ref_token   : refresh token returned by server
-----------------------------------------------------------------------------
*/
struct server_res_token {
    int status;
    int token_ttl;
    string token<>;
    string ref_token<>;
};

/*
-----------------------------------------------------------------------------
server_res_op
    status              : status of response, stores error codes
                        0 = SUCCESS
                        1 = USER_NOT_FOUND
                        2 = REQUEST_DENIED
                        3 = TOKEN_EXPIRED
                        4 = OPERATION_NOT_PERMITTED
                        5 = RESOURCE_NOT_FOUND
                        6 = PERMISSION_DENIED
                        7 = PERMISSION_GRANTED
-----------------------------------------------------------------------------
*/
struct server_res_op {
    string status<>;
};

program SPRC_HW {
	version SPRC_HW_VER {
		server_res_token req_auth(client_req_auth)                    = 1;
		server_res_token approve_req_token(client_req_approve)           = 2;
		server_res_token req_bearer_token(client_req_access)            = 3;
		server_res_token req_bearer_token_refresh(client_req_access)    = 4;
		server_res_token validate_delegated_action(client_req_op)   = 5;

	} = 1;
} = 123456789;