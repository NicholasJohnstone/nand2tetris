CC=g++ 

SOURCE_DIR = ./src
BUILD_DIR = ./build
INCLUDE_DIR = ./include

CFLAGS=-c -Wall -std=c++11 -I $(INCLUDE_DIR)/ -o $@

all: main tests

main: $(BUILD_DIR)/main.o $(BUILD_DIR)/Parser.o $(BUILD_DIR)/Code.o $(SOURCE_DIR)/Parser.h $(SOURCE_DIR)/Code.h
	$(CC) $(BUILD_DIR)/main.o $(BUILD_DIR)/Parser.o $(BUILD_DIR)/Code.o -o main $(LIBS)

$(BUILD_DIR)/main.o: $(SOURCE_DIR)/main.cpp
	$(CC) $(CFLAGS) $(SOURCE_DIR)/main.cpp

$(BUILD_DIR)/Parser.o: $(SOURCE_DIR)/Parser.cpp $(SOURCE_DIR)/Parser.h
	$(CC) $(CFLAGS) $(SOURCE_DIR)/Parser.cpp

$(BUILD_DIR)/Code.o: $(SOURCE_DIR)/Code.cpp $(SOURCE_DIR)/Code.h
	$(CC) $(CFLAGS) $(SOURCE_DIR)/Code.cpp

tests: test_Parser

test_Parser: $(BUILD_DIR)/test_Parser.o $(BUILD_DIR)/Parser.o
	$(CC) $(BUILD_DIR)/test_Parser.o $(BUILD_DIR)/Parser.o -o test_Parser $(LIBS)

$(BUILD_DIR)/test_Parser.o: $(SOURCE_DIR)/test_Parser.cpp
	$(CC) $(CFLAGS) $(SOURCE_DIR)/test_Parser.cpp

clean:
	rm -rf $(BUILD_DIR)/*.o
