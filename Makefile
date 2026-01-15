SYSTEMC_HOME = /usr/local/systemc
CXX = g++
CXXFLAGS = -I$(SYSTEMC_HOME)/include -Iinclude -std=c++17 -Wall -DSC_ALLOW_DEPRECATED_IEEE_API
LDFLAGS = -L$(SYSTEMC_HOME)/lib -lsystemc

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
EXEC = mp3_decoder

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(EXEC)