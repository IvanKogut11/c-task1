#include <iostream>

class ObjectForDemo
{

public:

	ObjectForDemo(int Id)
	{
		this->Id = Id;
		std::cout << "New object was created with id: " << Id << "\n";
	}

	~ObjectForDemo()
	{
		std::cout << "Object was deleted, it's id: " << Id << "\n";
	}

private:
	int Id;

};