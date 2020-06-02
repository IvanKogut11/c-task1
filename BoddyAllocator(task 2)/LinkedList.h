#pragma once

#include "Block.h"

class LinkedList
{

public:

	size_t count;
	size_t block_size;

	LinkedList()
	{
		head = nullptr;
		tail = nullptr;
		count = 0;
		block_size = 0;
	}

	void pushBackAddressed(void* memory)
	{
		Block* new_element = new Block(block_size);
		if (count == 0)
		{
			head = new_element;
			tail = new_element;
			head->addr = memory;
		}
		else
		{
			void* addr = tail->addr;
			tail->next = new_element;
			tail = new_element;
			tail->addr = memory;
		}
		count++;
	}

	void pushBack(Block* buddy)
	{
		if (count == 0)
		{
			head = buddy;
			tail = buddy;
		}
		else
		{
			tail->next = buddy;
			tail = buddy;
		}
		count++;
	}

	void Erase(Block* elem)
	{
		if (count == 0)
			return;

		if (elem->addr == head->addr)
		{
			head = head->next;
			if (count == 1)
				tail = nullptr;
			count--;
			return;
		}

		Block* current = head;
		while (current->next->addr != elem->addr)
		{
			current = head->next;
			if (current->next == nullptr)
				return;
		}

		Block* found = current->next;
		current->next = found->next;
		if (found == tail)
			tail = current;
		count--;
	}

	Block* PopFront()
	{
		Block* return_value = head;
		if (count > 0)
		{
			head = head->next;
			count--;
		}
		return return_value;
	}

	Block* GetBlockAtPosition(size_t position)
	{
		Block* current = head;
		for (int i = 1; i < position; ++i)
			current = head->next;
		return current;
	}

private:

	Block* head;
	Block* tail;
};
