#!/bin/bash

# Path definitions
PATH_CLIENT="./client/src/"
PATH_SERVER="./service/src/"

cd ${PATH_CLIENT}
cd ..
make

cd ..
cd ${PATH_SERVER}
cd ..
make