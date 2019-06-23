#pragma once
template<class Derived>
class Singleton
{
	static Derived* instance;

public:
	static Derived* getPtr() { 
		if (instance == nullptr)
			instance = new Derived;

		return instance; 
	}
};

template<class Derived>
Derived* Singleton<Derived>::instance = nullptr;
