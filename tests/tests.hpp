#ifndef TESTS_HPP
# define TESTS_HPP


// explicit allocator, outputing a message on cout
// each time alloc() is called
#include "explicit_allocator.v.hpp"

#include <algorithm>

// two version of this file can be compiled
// "clang++ -DOG" to compile with orginial STL
#ifndef OG
    #include "../containers/vector/vector.hpp"
    #include "../containers/stack/stack.hpp"
    #include "../containers/map/map.hpp"
#else
    #include <vector>
    #include <stack>
    #include <map>
    namespace ft = std;
#endif

// Format utils
void print_title(const char *s);
void print_green(const char *s, int line = 0);

void vector_all_tests();
void stack_all_tests();
void map_all_tests();

#endif
