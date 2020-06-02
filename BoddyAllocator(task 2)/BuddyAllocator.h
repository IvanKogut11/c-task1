#pragma once

#include <bitset>

#include "LinkedList.h"

template<std::size_t N>
class BuddyAllocator
{

public:
	
	void* memory;
	std::bitset<N> flags;
	LinkedList** free_spaces;

	BuddyAllocator(std::size_t min_order, std::size_t max_order)
	{
		memory_size = 1 << max_order;
		memory = malloc(memory_size);
		min_ord = min_order;
		max_ord = max_order;
		free_spaces = new LinkedList * [max_order - min_order + 1];
		std::size_t current_size = 1 << min_order;

		for (int i = min_order; i <= max_order; ++i)
		{
			free_spaces[i - min_order] = new LinkedList();
			free_spaces[i - min_order]->block_size = current_size;
			current_size = current_size << 1;
		}
		free_spaces[max_ord - min_ord]->pushBackAddressed(memory);
		flags.reset();
	}

	~BuddyAllocator()
	{
		delete memory;
		delete[] free_spaces;
	}

	void* allocate(std::size_t size)
	{
		for (int i = min_ord; i <= max_ord; ++i)
		{
			if (free_spaces[i - min_ord]->block_size >= size)
			{
				if (free_spaces[i - min_ord]->count > 0)
				{
					Block* elem = free_spaces[i - min_ord]->PopFront();
					printf("Allocated memory at %d level, %d bytes, %p address\n", i - min_ord, free_spaces[i - min_ord]->block_size, elem->addr);
					int index = (memory_size / free_spaces[i - min_ord]->block_size - 1) + ((static_cast<char*>(elem->addr) - static_cast<char*>(memory)) / free_spaces[i - min_ord]->block_size);
					flags[index] = true;
					return elem->addr;
				}
				else
				{
					for (int index = i - min_ord + 1; index <= max_ord - i; ++index)
					{
						if (free_spaces[index]->count > 0)
						{
							void* addr = free_spaces[index]->PopFront()->addr;

							int ind = (memory_size / free_spaces[index]->block_size - 1) + ((static_cast<char*>(addr) - static_cast<char*>(memory)) / free_spaces[index]->block_size);
							flags[ind] = true;

							int deep_index = index - 1;
							while (deep_index >= i - min_ord && deep_index >= 0)
							{
								free_spaces[deep_index]->pushBackAddressed(addr);
								addr = static_cast<char*>(addr) + free_spaces[deep_index]->block_size;
								int ind = (memory_size / free_spaces[deep_index]->block_size - 1) + ((static_cast<char*>(addr) - static_cast<char*>(memory)) / free_spaces[deep_index]->block_size);
								flags[ind] = true;
								deep_index--;
							}
							printf("Allocated memory at %d level, %d bytes, %p address\n", deep_index + 1, free_spaces[deep_index + 1]->block_size, addr);
							return addr;
						}
					}
				}
			}
		}
		printf("Memory is full\n");
		return nullptr;
	}

	void deallocate(Block* buddy)
	{
		Block* current = buddy;
		int depth = memory_size / (1 << min_ord) - memory_size / current->size;
		int index = (memory_size / free_spaces[depth]->block_size - 1) + ((static_cast<char*>(current->addr) - static_cast<char*>(memory)) / free_spaces[depth]->block_size);

		while (true)
		{
			free_spaces[depth]->pushBack(current);
			flags[index] = false;
			printf("Block at %d level with %p address was cleared\n", depth, current->addr);
			if (index > 0)
			{
				if (index % 2 == 0)
				{
					if (!flags[index - 1])
					{
						Block* neighbour = new Block(current->size);
						neighbour->addr = static_cast<char*>(current->addr) - current->size;

						Block* new_block = new Block(current->size * 2);
						new_block->addr = static_cast<char*>(current->addr) - current->size;

						printf("Blocks at %d level with %p and %p addresses was merged\n", depth, current->addr, neighbour->addr);

						current = new_block;
					}
					else
						break;
				}
				else
				{
					if (!flags[index + 1])
					{
						Block* neighbour = new Block(current->size);
						neighbour->addr = static_cast<char*>(current->addr) + current->size;

						Block* new_block = new Block(current->size * 2);
						new_block->addr = static_cast<char*>(current->addr);

						printf("Blocks at %d level with %p and %p addresses was merged\n", depth, current->addr, neighbour->addr);

						current = new_block;
					}
					else
						break;
				}
				index = (index - 1) / 2;
				depth++;
			}
			else
				break;
		}
	}

private:
	std::size_t memory_size;
	std::size_t min_ord;
	std::size_t max_ord;
};