all: a.out

a.out: src.cpp
	g++ -fsanitize=address -Wextra -Wall -pedantic -std=c++11 src.cpp

.PHONY: demo
demo: a.out ../sample/input.txt
	./a.out ../sample/courses.csv "Prog" < ../sample/input.txt > ../sample/output.txt

.PHONY: clean
clean:
	rm a.out
