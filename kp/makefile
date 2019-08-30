.PHONY :  all, clean
TARGET := prog
SRC    := $(wildcard *.cpp)
HDR    := $(wildcard *.hpp)
CXX    := g++
CFLAGS := -pedantic -Wall -Wextra -Wpedantic -W -Wno-sign-compare -Wno-long-long -g3 -lm

all: $(TARGET)

clean:
	rm -f $(TARGET) $(SRC:%.cpp=%.o)

$(TARGET): $(SRC:%.cpp=%.o)
	$(CXX) $(CFLAGS) -o $@ $^

main.o: main.cpp $(HDR) 
	$(CXX) $(CFLAGS) -c main.cpp

%.o: %.cpp %.hpp
	$(CXX) $(CFLAGS) -c -o $@ $<