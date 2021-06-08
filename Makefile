all:
	g++ -c MarkovChain.cpp
	g++ -o a.out main.cpp MarkovChain.o
clean:
	rm MarkovChain.o a.out

