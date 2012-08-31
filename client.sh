#!/bin/bash

CLIENT_CMD="tia-client"

if [ ! -f bin/${CLIENT_CMD} ]; then
  echo "** ERROR - bin/${CLIENT_CMD} does not exist."
  exit 1
fi

PLATFORM=$(uname -m)

if [ "$PLATFORM" == "x86_64" ]
then
  LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/lib/amd64
else
  LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/lib/x32
fi

export PATH LD_LIBRARY_PATH
exec ./bin/${CLIENT_CMD}  $1 $2 $3
# exec valgrind  --leak-check=full --show-reachable=yes ./bin/${CLIENT_CMD}  $1 $2 $3

