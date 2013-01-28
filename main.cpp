//
//  main.cpp
//  Variants
//
//  Created by Oliver Daids on 1/17/13.
//  Copyright (c) 2013 Oliver Daids. All rights reserved.
//

#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <algorithm>
#include <functional>

template<typename... Types>
struct Variant
{
	private:
	
	static constexpr size_t Max(size_t A, size_t B)
	{
		return A > B ? A : B;
	}

	template<typename T>
	static constexpr size_t TypeMaxSize()
	{
		return sizeof(T);
	}

	template <typename T, typename T2, typename... Ts>
	static constexpr size_t TypeMaxSize()
	{
		return Max(Max(sizeof(T), sizeof(T2)), TypeMaxSize<Ts...>());
	}

	
	char data[TypeMaxSize<Types...>()];
	std::type_index type_index;
	
	Variant(void) : type_index(typeid(void))
	{
		
	}
	
	template<typename F>
	bool forEachType(F f) const
	{
		return false;
	}
	
	template<typename F, typename T, typename... TS>
	bool forEachType(F f) const
	{
		return f(*((T*)this->data)) or forEachType<F, TS...>(f);
	}
	
	public:
	
	Variant(const Variant<Types...>& other) : type_index(other.type_index)
	{
		this->type_index = other.type_index;
		struct
		{
			const Variant<Types...>* self;
			
			template<typename T>
			bool operator () (T& t)
			{
				if(this->self->type_index not_eq std::type_index(typeid(T))) return false;
				(*(T*)(this->self->data)) = t;
				return true;
			}
		}f;
		f.self = this;
		other.forEachType<decltype(f), Types...>(f);
	}
	
	template<typename T>
	explicit
	Variant(T value) : type_index(typeid(T))
	{
		new(data) T(value);
	}
	
	~Variant(void)
	{
		struct
		{
			std::type_index type_index = this->type_index;
			
			template<typename T>
			bool operator () (T& t)
			{
				
				if(this->type_index not_eq std::type_index(typeid(T))) return false;
				t.~T();
				return true;
			}
		}f;
		forEachType<decltype(f), Types...>(f);
	}
	
	void operator = (const Variant<Types...>& other)
	{
		this->type_index = other.type_index;
		struct
		{
			const Variant<Types...>* self;
			
			template<typename T>
			bool operator () (T& t)
			{
				if(this->self->type_index not_eq std::type_index(typeid(T))) return false;
				(*(T*)(this->self->data)) = t;
				return true;
			}
		}f;
		f.self = this;
		other.forEachType<decltype(f), Types...>(f);
	}
	
	template<typename T, typename F>
	bool doIf(const F& f)
	{
		bool doesMatch = (this->type_index == std::type_index(typeid(T)));
		if(doesMatch) f(*(T*)(this->data));
		return doesMatch;
	}
};

int main(int argc, const char * argv[])
{
	typedef Variant<const char*, int, float> V;
	V value(float(100));
	V value2 = value;
	//value = V(int(0));
	value.doIf<int>([](int v){std::cout << "Sup: " << v << "\n";});
	value.doIf<float>([](float v){std::cout << "Sup: " << v << "\n";});
	value.doIf<const char*>([](const char* v){std::cout << "Sup: " << v << "\n";});
    return 0;
}

