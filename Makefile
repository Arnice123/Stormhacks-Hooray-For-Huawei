CXX = g++
#CXXFLAGS = -Wall -Wextra -Iinclude
CXXFLAGS = -Iinclude
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGETS = recomputation test_parser

all: $(TARGETS)

$(TARGETS): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGETS)
