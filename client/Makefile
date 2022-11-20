# Compiler flags 
CCPP = g++
CFLAGS += -g -Wall
LDLIBS += -lnsl
RPCGENFLAGS = 
# Parameters
EXE = client 
SRC.cpp = ./src/xdr.cpp ./src/client_stub.cpp ./src/sample_client.cpp 
OBJ = $(SRC.cpp:%.cpp=%.o)

# Targets 
all : $(EXE)

$(OBJ) : $(SRC.cpp) 

$(EXE) : $(OBJ) 
	$(CCPP) -o $(EXE) $(OBJ) $(LDLIBS)

 clean:
	 $(RM) $(OBJ) $(EXE)

