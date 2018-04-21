CC=g++
FLAGS=-g
LIBS=-lm

../bin/vcham: vcham.hpp vcham.cpp vcham-solve.cpp vcham-tests.cpp main.cpp
	$(CC) $(FLAGS) -o ../bin/vcham vcham.cpp vcham-solve.cpp vcham-tests.cpp main.cpp $(LIBS)

random-graph/random-graph.o: random-graph/random-graph.cpp
	$(CC) $(FLAGS) -c -o random-graph/random-graph.o random-graph/random-graph.cpp $(LIBS)

../bin/random-graph: random-graph/random-graph.o random-graph/random-graph_main.cpp
	$(CC) $(FLAGS) -o ../bin/random-graph random-graph/random-graph_main.cpp random-graph/random-graph.o $(LIBS)

../bin/er: random-graph/er.cpp
	$(CC) $(FLAGS) -o ../bin/er random-graph/er.cpp $(LIBS)
