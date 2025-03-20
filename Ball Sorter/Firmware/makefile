CXX = g++

# Compiler and linker flags
CFLAGS = -Wall `pkg-config --cflags opencv` -std=c++11
LDFLAGS = `pkg-config --libs opencv` -lpigpio -lrt -lpthread -lwiringPi  -lwiringPiDev

# Target and source files
TARGET = Sorter

SRCS = main.cpp 
OBJS = $(SRCS:.cpp=.o)

# Build rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)