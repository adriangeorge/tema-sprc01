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
./clean.sh

# Break down generation in steps
# Stubs
rpcgen -C -l ${INTF_X} -o ${CLNT}
rpcgen -C -s udp ${INTF_X} -o ${SERV}

# Interface headers
rpcgen -C -h ${INTF_X} -o ${PATH_CLIENT}${RPC_HEADER}
rpcgen -C -h ${INTF_X} -o ${PATH_SERVER}${RPC_HEADER}

# Samples
rpcgen -C -Sc ${INTF_X} -o ${PATH_CLIENT}client.cpp
rpcgen -C -Ss ${INTF_X} -o ${PATH_SERVER}server.cpp

# xdr
rpcgen -C -c ${INTF_X} -o ${PATH_CLIENT}xdr.cpp
rpcgen -C -c ${INTF_X} -o ${PATH_SERVER}xdr.cpp
