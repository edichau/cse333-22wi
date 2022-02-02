# Copyright 2022 <Edison Chau>
CXX = g++
CPPFLAGS = -Wall -g -std=c++17
OBJFILES = Vector.o ex6.o
TARGET  = ex6
HEADERS = Vector.h

$(TARGET): $(OBJFILES)
	$(CXX) $(CPPFLAGS) -o $(TARGET) $(OBJFILES)

%.o: %.cc Vector.h
	$(CXX) $(CPPFLAGS) $< -c 

clean:
	rm -rf $(TARGET) *.o *~