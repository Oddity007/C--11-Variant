/*
Copyright (c) 2013 Oliver Daids.

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software.  If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source distribution.
*/

#include <cstdlib>
#include <typeinfo>
#include <typeindex>

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
	
	/*template<typename T, typename F>
	bool doIf(const F& f)
	{
		bool doesMatch = (this->type_index == std::type_index(typeid(T)));
		if(doesMatch) f(*(T*)(this->data));
		return doesMatch;
	}*/
	
	template<typename T>
	bool isType(void)
	{
		return this->type_index == std::type_index(typeid(T));
	}
	
	template<typename T>
	T& cast(void)
	{
		if(not isType<T>()) throw;
		return *(T*)(this->data);
	}
};

