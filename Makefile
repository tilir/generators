CXXFLAGS += --std=c++17

naivepavings: naivepavings.cc

.PHONY: clean
clean:
	rm naivepavings
