#include <iostream>

#include "SmartPointer.h"
#include "ObjectForDemo.h"

int main()
{
	SmartPointer<ObjectForDemo> Ptr1(new ObjectForDemo(1));
	SmartPointer<ObjectForDemo> Ptr2(new ObjectForDemo(2));
	Ptr2 = Ptr1; //Object with id 2 was deleted here because of Ptr2's redirection 

	SmartPointer<ObjectForDemo> Ptr3(new ObjectForDemo(3));
	std::cout << "Program finished" << "\n";
	//Objects with ids 1 and 3 were deleted here because program finished
}