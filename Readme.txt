Basic C++11 template variant type.

zlib license and all that goodness

I make use of tagged unions quite a lot, and I wanted a variant type to handle all the boilerplate I write, at least during prototyping, so I did this weekend project that essentially taught me that C++'s template metaprogramming sucks.  In my opinion, Clay (http://claylabs.com/clay/) is a lot nicer for this type of thing, but it already has a variant type built-in.

I might try and get rid of the use of std::type_index in favor of a length-dependent type-unique integer type using more template metaprogramming magic, but std::type_index works for now.

Usage example:

#include "Variant.hpp"
#include <iostream>

int main(int argc, const char * argv[])
{
	typedef Variant<const char*, int, float> V;
	V value(float(100));
	V value2 = value;
	value = V(int(0));
	if(value.isType<int>())
	{
		std::cout << value.cast<int>() << std::endl;
	}
	else if(value.isType<float>())
	{
		std::cout << value.cast<float>() << std::endl;
	}
	else if(value.isType<const char*>())
	{
		std::cout << value.cast<const char*>() << std::endl;
	}
	return 0;
}