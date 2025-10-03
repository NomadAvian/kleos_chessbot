CXX = g++
CXXFLAGS = -O3
SOURCES = ./src/*.cpp

ifeq ($(OS),Windows_NT)
	EXE = .exe
else
	EXE = 
endif

TARGET = ./bin/kleos_classic$(EXE)

all: build run clean

build:
	$(CXX) $(SOURCES) $(CXXFLAGS) -o $(TARGET)

run:
	$(TARGET)

clean:
	rm -f $(TARGET)