CXX = g++
CXXFLAGS = -Wall -Wextra -Iinclude

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = recomputation

TEST_SRC = tester/tester.cpp
TEST_TARGET = tester

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(OBJ) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_SRC) $(OBJ)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET) $(TEST_TARGET)
