CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g -Wno-unused-parameter

SRCS = main.cpp Player.cpp Game.cpp Actions.cpp RoleBase.cpp Governor.cpp Spy.cpp Judge.cpp Merchant.cpp Baron.cpp General.cpp
OBJS = $(SRCS:.cpp=.o)

TEST_OBJS = $(filter-out main.o, $(OBJS)) test.o


Main: $(OBJS)
	g++ $(CXXFLAGS) -o game $^
	./game

test: $(TEST_OBJS)
	g++ $(CXXFLAGS) -o test $^
	./test

valgrind: Main test
	valgrind --leak-check=summary ./game 
	valgrind --leak-check=summary ./test


clean:
	rm -f *.o game test
