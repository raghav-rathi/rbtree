# Compiler
CXX = g++

# Compiler flags

# Target executable name
TARGET = t1

# Source file
SRC = t1.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET) input

clean:
	rm -f $(TARGET)