CXX = g++
CXXFLAGS = -Wall -Wextra -Iinclude

# Source files
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

# Main program
MAIN_OBJ = src/main.o
# Library objects (exclude main)
LIB_OBJ = $(filter-out $(MAIN_OBJ), $(OBJ))

# Test files
TEST_SRC = $(wildcard test/*.cpp)
TEST_OBJ = $(TEST_SRC:.cpp=.o)

TARGET = recomputation
TEST_TARGET1 = tester_exec
TEST_TARGET2 = tester_parser

all: $(TARGET)

tests: $(TEST_TARGET1) $(TEST_TARGET2)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET1): test/exec_order_tester.o $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET2): test/test_parser.o $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o test/*.o $(TARGET) $(TEST_TARGET1) $(TEST_TARGET2)
