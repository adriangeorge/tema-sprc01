#!/bin/bash

# Path definitions
PATH_CLIENT="./client/src/"
PATH_SERVER="./service/src/"

# File definitions
RPC_HEADER="interface.h"
RPC_GENERATOR="interface.x"
STUB_CLIENT="client_stub.cpp"
STUB_SERVER="server_stub.cpp"

INTF_H=${RPC_HEADER}
INTF_X=${RPC_GENERATOR}
CLNT=${PATH_CLIENT}${STUB_CLIENT}
SERV=${PATH_SERVER}${STUB_SERVER}

# Remove old files
rm  ${CLNT} ${SERV} ${INTF_H} ${PATH_CLIENT}xdr.cpp     \
    ${PATH_SERVER}xdr.cpp ${PATH_CLIENT}${RPC_HEADER}   \
    ${PATH_SERVER}${RPC_HEADER} 2> /dev/null

# Clean compiled files
cd ${PATH_CLIENT}
cd ..
make clean

cd ..
cd ${PATH_SERVER}
cd ..
make clean