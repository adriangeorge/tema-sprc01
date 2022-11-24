# Compiler flags 
CCPP = g++
CXXFLAGS = -g
LDLIBS += -lnsl
# Parameters
EXE = server 
SRC.cpp = ./src/xdr.cpp ./src/server_stub.cpp ./src/server.cpp 
OBJ = $(SRC.cpp:%.cpp=%.o)

# Targets 
all : $(EXE)

$(OBJ) : $(SRC.cpp) 

$(EXE) : $(OBJ) 
	$(CCPP) -o $(EXE) $(OBJ) $(LDLIBS)

 clean:
	 $(RM) $(OBJ) $(EXE)

