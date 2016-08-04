OBJS=test.o tinyhtml.o jsoncpp.o
CC=g++
CFLAGS=-Wall -O -g -std=c++11
INCLUDE = "../jsoncpp"
testexe:$(OBJS)
	$(CC) $^ -o $@
%.o:%.cpp
	$(CC) $(CFLAGS) -c -I$(INCLUDE) $< -o $@
clean:
	rm *.o *.exe testexe