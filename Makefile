CXX=g++
CXXFLAGS=-Wall -Wextra -O2 -std=c++20
PHONY=example clean
VALGRIND=valgrind --error-exitcode=123 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --run-cxx-freeres=yes -q

example: college_example
	./college_example

match: matching_tests
	./matching_tests

clean:
	-rm college_example

.cc: college.h
	$(CXX) $(CXXFLAGS) $< -o $@

