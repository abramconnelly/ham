CC=g++
DEBUG_FLAGS=-g -std=c++11
PROF_FLAGS=-g -std=c++11
FLAGS=-O3 -std=c++11
LIBS=-lm

../bin/vcham: vcham.hpp vcham.cpp vcham-solve.cpp vcham-tests.cpp main.cpp
	$(CC) $(FLAGS) -o ../bin/vcham vcham.cpp vcham-solve.cpp vcham-tests.cpp main.cpp $(LIBS)

../bin/vcham-prof: vcham.hpp vcham.cpp vcham-solve.cpp vcham-tests.cpp main.cpp
	$(CC) $(PROF_FLAGS) -o ../bin/vcham-prof vcham.cpp vcham-solve.cpp vcham-tests.cpp main.cpp $(LIBS)

../bin/vcham-debug: vcham.hpp vcham.cpp vcham-solve.cpp vcham-tests.cpp main.cpp
	$(CC) $(DEBUG_FLAGS) -o ../bin/vcham-debug vcham.cpp vcham-solve.cpp vcham-tests.cpp main.cpp $(LIBS)
	#$(CC) -DVERBOSE_SOLVE_PRINT $(DEBUG_FLAGS) -o ../bin/vcham-debug vcham.cpp vcham-solve.cpp vcham-tests.cpp main.cpp $(LIBS)

random-graph/random-graph.o: random-graph/random-graph.cpp
	$(CC) $(FLAGS) -c -o random-graph/random-graph.o random-graph/random-graph.cpp $(LIBS)

../bin/random-graph: random-graph/random-graph.o random-graph/random-graph_main.cpp
	$(CC) $(FLAGS) -o ../bin/random-graph random-graph/random-graph_main.cpp random-graph/random-graph.o $(LIBS)

../bin/er: random-graph/er.cpp
	$(CC) $(FLAGS) -o ../bin/er random-graph/er.cpp $(LIBS)
