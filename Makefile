PROJECT := $(shell readlink $(dir $(lastword $(MAKEFILE_LIST))) -f)

CXX = g++
CXXFLAGS = -std=c++11 \
           -Wall \
           -Wno-sign-compare \
           -fno-omit-frame-pointer \
	   -g \
	   -gdwarf-2
 
MULTIVERSO_DIR = $(PROJECT)/../multiverso
MULTIVERSO_INC = $(MULTIVERSO_DIR)/include/multiverso
MULTIVERSO_LIB = $(MULTIVERSO_DIR)/lib
THIRD_PARTY_LIB = $(MULTIVERSO_DIR)/third_party/lib

INC_FLAGS = -I$(MULTIVERSO_INC)
LD_FLAGS  = -L$(MULTIVERSO_LIB) -lmultiverso
LD_FLAGS += -L$(THIRD_PARTY_LIB) -lzmq -lmpich -lmpl -lpthread
  	  	
SVRG_HEADERS = $(shell find $(PROJECT)/src -type f -name "*.h")
SVRG_SRC     = $(shell find $(PROJECT)/src -type f -name "*.cpp")
SVRG_OBJ = $(SVRG_SRC:.cpp=.o)

BIN_DIR = $(PROJECT)/bin
SVRG = $(BIN_DIR)/svrg

all: path \
	 svrg 

path: $(BIN_DIR)

$(BIN_DIR):
	mkdir -p $@

$(SVRG): $(SVRG_OBJ)
	$(CXX) $(SVRG_OBJ) $(CXXFLAGS) $(INC_FLAGS) $(LD_FLAGS) -o $@

$(SVRG_OBJ): %.o: %.cpp $(SVRG_HEADERS) $(MULTIVERSO_INC)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

svrg: path $(SVRG)
	
clean:
	rm -rf $(BIN_DIR) $(SVRG_OBJ)

.PHONY: all path svrg clean
