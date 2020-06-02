#include<iostream>
#include<bitset>

#include "Block.h"
#include "LinkedList.h"
#include "BuddyAllocator.h"

using namespace std;

const size_t MIN_ORD = 4;
const size_t MAX_ORD = 6;
const size_t MAX_SIZE = 1 << 3;

int main()
{
	BuddyAllocator<MAX_SIZE>* buddies = new BuddyAllocator<MAX_SIZE>(MIN_ORD, MAX_ORD);
	buddies->allocate(1);
	buddies->allocate(1);
	buddies->allocate(32);
	buddies->allocate(1);

	Block* buddy = new Block(16);
	buddy->addr = static_cast<char*>(buddies->memory) + 32;
	buddies->deallocate(buddy);

	buddy->addr = static_cast<char*>(buddies->memory) + 48;

	buddies->deallocate(buddy);

	buddies->allocate(32);
	return 0;
}