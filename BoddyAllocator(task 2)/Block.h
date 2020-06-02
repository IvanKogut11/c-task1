#pragma once

class Block
{

public:

	void* addr;
	Block* next;
	size_t size;

	Block(size_t s)
	{
		next = nullptr;
		size = s;
	}
};