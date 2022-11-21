# Compiler flags 
CCPP = g++
CXXFLAGS = -g -Wall
LDLIBS += -lnsl
# Parameters
EXE = client 
SRC.cpp = ./src/xdr.cpp ./src/client_stub.cpp ./src/client.cpp 
OBJ = $(SRC.cpp:%.cpp=%.o)

# Targets 
all : $(EXE)

$(OBJ) : $(SRC.cpp) 

$(EXE) : $(OBJ) 
	$(CCPP) -o $(EXE) $(OBJ) $(LDLIBS)

 clean:
	 $(RM) $(OBJ) $(EXE)

