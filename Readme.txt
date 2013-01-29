Basic C++11 template variant type.

zlib license and all that goodness

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