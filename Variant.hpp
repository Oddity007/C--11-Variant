/*
Copyright (c) 2015 Oliver Daids.

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software.  If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source distribution.
*/

#include <cstdlib>
#include <typeinfo>
#include <typeindex>
#include <cassert>

template<typename... Types>
struct Variant
{
	

	Variant(const Variant& other) : type_index(other.type_index)
	{
		selectCopy<Types...>(other);
	}
	
	Variant& operator= (const Variant& other)
	{
		type_index = other.type_index;
		selectCopy<Types...>(other);
	}
	
	template<typename T>
	explicit
	Variant(const T& value) : type_index(typeid(T))
	{
		new(data) T(value);
	}
	
	Variant(Variant&& other) : type_index(std::move(other.type_index))
	{
		selectMove<Types...>(other);
	}
	
	Variant& operator= (Variant&& other)
	{
		type_index = std::move(other.type_index);
		selectMove<Types...>(other);
	}

	~Variant(void)
	{
		selectDelete<Types...>();
	}

	template<typename T>
	bool is(void) const
	{
		return type_index == typeid(T);
	}
	
	template<typename T>
	T& as(void) const
	{
		assert(is<T>());
		return *(T*)(this->data);
	}

	private:
	
	Variant(void)
	{
		
	}
	
	template <size_t N> struct NumberAsType{};
	
	template<typename T>
	void selectDelete(NumberAsType<0> = NumberAsType<0>())
	{
		as<T>().~T();
	}
	
	template<typename T, typename... Ts>
	void selectDelete(NumberAsType<sizeof...(Ts)> = NumberAsType<sizeof...(Ts)>())
	{
		if (not is<T>())
			return selectDelete<Ts...>(NumberAsType<sizeof...(Ts) - 1>());
		as<T>().~T();
	}
	
	template<typename T>
	void selectCopy(const Variant& other, NumberAsType<0> = NumberAsType<0>())
	{
		new (data) T(other.as<T>());
	}
	
	template<typename T, typename... Ts>
	void selectCopy(const Variant& other, NumberAsType<sizeof...(Ts)> = NumberAsType<sizeof...(Ts)>())
	{
		if (not is<T>()) return selectCopy<Ts...>(other, NumberAsType<sizeof...(Ts) - 1>());
		new (data) T(other.as<T>());
	}
	
	template<typename T>
	void selectMove(Variant&& other, NumberAsType<0> = NumberAsType<0>())
	{
		new (data) T(std::move(other.as<T>()));
	}
	
	template<typename T, typename... Ts>
	void selectMove(Variant&& other, NumberAsType<sizeof...(Ts)> = NumberAsType<sizeof...(Ts)>())
	{
		if (not is<T>()) return selectMove<Ts...>(other, NumberAsType<sizeof...(Ts) - 1>());
		new (data) T(std::move(other.as<T>()));
	}
	
	static constexpr size_t max(size_t A, size_t B)
	{
		return A > B ? A : B;
	}

	template<typename T>
	static constexpr size_t CommonSize(NumberAsType<0> = NumberAsType<0>())
	{
		return sizeof(T);
	}

	template <typename T, typename... Ts>
	static constexpr size_t CommonSize(NumberAsType<sizeof...(Ts)> = NumberAsType<sizeof...(Ts)>())
	{
		return max(sizeof(T), CommonSize<Ts...>(NumberAsType<sizeof...(Ts) - 1>()));
	}

	static constexpr size_t gcd(size_t m, size_t n)
	{
		return n == 0 ? m : gcd(n, m % n);
	}
	
	static constexpr size_t lcm(size_t m, size_t n)
	{
		return m * n == 0 ? 0 : m / gcd(m, n) * n;
	}

	template<typename T>
	static constexpr size_t CommonAlignment(NumberAsType<0> = NumberAsType<0>())
	{
		return sizeof(T);
	}

	template <typename T, typename... Ts>
	static constexpr size_t CommonAlignment(NumberAsType<sizeof...(Ts)> = NumberAsType<sizeof...(Ts)>())
	{
		return lcm(alignof(T), CommonAlignment<Ts...>(NumberAsType<sizeof...(Ts) - 1>()));
	}
	
	alignas(CommonAlignment<Types...>()) char data[CommonSize<Types...>()];
	std::type_index type_index;
};

