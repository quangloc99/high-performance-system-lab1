GCC=g++
# using c++17 because of filesystem (create_directory)
# FLAGS=-fopenmp -std=c++17 -g -Wall -Wshadow -Wconversion -O2 
FLAGS=-fopenmp -std=c++17 -g -Wall -Wshadow -Wconversion  
# FLAGS=-fopenmp -std=c++17 -g -Wall -Wshadow -Wconversion -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -ffinite-math-only -D_GLIBCXX_DEBUG

all: main
	
clean:
	rm -f *.o
	rm -f ./main
	rm -rf result/

test: test.cpp 
	$(GCC) $(FLAGS) test.cpp -o test
	
parallelized-modified-timsort.o: parallelized-modified-timsort.hpp parallelized-modified-timsort.cpp
	$(GCC) $(FLAGS) parallelized-modified-timsort.cpp -c

parallelized-treesort.o: parallelized-treesort.hpp parallelized-treesort.cpp
	$(GCC) $(FLAGS) parallelized-treesort.cpp -c
	
main: main.cpp parallelized-modified-timsort.o parallelized-treesort.o
	$(GCC) $(FLAGS) main.cpp parallelized-modified-timsort.o parallelized-treesort.o -o ./main
