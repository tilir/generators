CXXFLAGS += --std=c++17

all : naivepavings grtests allspan

naivepavings : naivepavings.cc
	${CXX} ${CXXFLAGS} $^ -o $@ 

grtests : grtests.cc graphdef.cc graphgens.cc
	${CXX} ${CXXFLAGS} $^ -o $@ 

allspan : allspan.cc graphdef.cc graphgens.cc
	${CXX} ${CXXFLAGS} $^ -o $@ 

.PHONY: clean
clean :
	rm -rf naivepavings knuth.dot lat23.dot lat33.dot lat43.dot kspan.dot lat23span.dot lat33span.dot lat43span.dot kloop.dot lat23loop.dot lat33loop.dot
	rm -rf grtests allspan grtests.o allspan.o graphrep.o
