CXX = g++
CXXFLAGS = -g -Wall -std=c++17

CXX_SRCS = csim.cpp csim_tests.cpp
CXX_OBJS = $(CXX_SRCS:.cpp=.o)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $*.o

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

csim : $(CXX_OBJS)
	$(CXX) -o $@ $(CXX_OBJS)

.PHONY: solution.zip
solution.zip :
	rm -f $@
	zip -9r $@ *.c *.cpp *.h README.txt

clean :
	rm -f bigint_tests *.o