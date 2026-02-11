////////////////////////////
//	
//	Memory pool manager
//	

#include <tWare/Memory.h>
#include <tWare/Debug.h>

tMemoryPoolInfo MemoryPoolInfoTable[TMEMORY_POOL_COUNT];
tMemoryPool* MemoryPools[TMEMORY_POOL_COUNT];
uint8_t MemoryPoolMem[TMEMORY_POOL_COUNT][sizeof(tMemoryPool)];
#ifdef EA_PLATFORM_WINDOWS
uint8_t memory[0x4000]; // test memory space (will be dynamically allocated based on arena size later)
#endif

size_t tGetNextAlignedSizeOrAddress(size_t n, size_t alignment)
{
	return (n - 1u + alignment) & (0 - alignment);
}

size_t tGetPreviousAlignedSizeOrAddress(size_t n, size_t alignment)
{
	return n - (tGetNextAlignedSizeOrAddress(n, alignment) % alignment);
}

void tMemoryPool::Init(void* memoryBlock, size_t size, const char* name, uint32_t poolNum)
{
	DebugName = name;
	MemoryBlock = memoryBlock;
	InitialAddress = (intptr_t)memoryBlock;
	InitialSize = size;
	FreeSize = size;
	PoolNum = poolNum;

	memset(memoryBlock, 0, size);

	Block* startingBlock = new(memoryBlock) Block(size, poolNum);

	BlockList.AddTail(startingBlock);
}

void* tMemoryPool::AllocateMemory(size_t size, uint32_t alignment, const char* debugText, uint32_t debugLine)
{
	//printf("Allocating %u bytes for %s\n", size, debugText);
	size_t originalSize = size; // purely for debugging
	Block* block = GetFirstFreeBlockThatFitsSize(tGetNextAlignedSizeOrAddress(size + sizeof(Block), alignment));

	if (block)
	{
		uintptr_t newBlockBase = ((intptr_t)block) + block->Size;

		size_t newBlockSize = size + (tGetPreviousAlignedSizeOrAddress(newBlockBase - size, alignment) % alignment);
		Block* newBlock;

		newBlockSize += sizeof(Block);

		if (block->Size < newBlockSize)
			tBreak();

		block->Size -= newBlockSize;

		intptr_t endOfCurrentBlock = ((intptr_t)block) + block->Size;

		if (block->Size <= sizeof(Block)) // no point in keeping the block around if it's too small
		{
			Block* previousBlock = block->GetPrev();
			previousBlock->Size += block->Size; // swallow the remnants of the free block
			BlockList.Remove(block);
			block = previousBlock;
		}

		if (block->GetNext() != BlockList.EndOfList())
		{
			size = ((intptr_t)block->GetNext()) - endOfCurrentBlock;
		}
		else
		{
			size = (InitialAddress + InitialSize) - endOfCurrentBlock;
		}

		newBlock = new((void*)endOfCurrentBlock) Block(size, PoolNum); // add new occupied block in the free space
		newBlock->Occupied = true;
		newBlock->MagicNumber = 0xDEADBEEF;
		newBlock->DebugName = debugText;
		newBlock->DebugLine = debugLine;

		BlockList.AddAfter(block, newBlock); // slot it into the list

		FreeSize -= newBlock->Size;

		return (void*)(newBlock + 1);
	}
	else
	{
		return NULL; // we failed!
	}
}

void tMemoryPool::FreeMemory(void* ptr)
{
	Block* block = ((tMemoryPool::Block*)ptr) - 1;
	Block* prevBlock = block->GetPrev();

	block->Occupied = false;

	// if we're not the first in list (at bottom of pool) and previous isn't occupied, merge
	if (block != BlockList.GetHead() && !prevBlock->Occupied)
	{
		BlockList.Remove(block);
		prevBlock->Size += block->Size; // add this block's size
	}

	ConsolidateFreeBlocks(); // consolidate the blocks first

	FreeSize += block->Size;
}

void tMemoryPool::ConsolidateFreeBlocks()
{
	for (Block* block = BlockList.GetTail(); block != BlockList.EndOfList(); block = block->GetPrev())
	{
		if (block->Occupied)
			continue; // we only want free blocks

		Block* next = block->GetNext();
		if (next && next != BlockList.EndOfList() && !next->Occupied) // if the next block is free
		{
			BlockList.Remove(next); // remove next from list
			block->Size += next->Size; // swallow it
		}
	}
}

tMemoryPool::Block* tMemoryPool::GetLargestFreeBlock()
{
	Block* largest = NULL;

	// start counting from top
	for (Block* block = BlockList.EndOfList()->GetPrev(); block != BlockList.GetHead()->GetPrev(); block = block->GetPrev())
	{
		// find earliest spot in memory with largest block
		if (!block->Occupied && (largest == NULL || (block->Size >= largest->Size)))
		{
			largest = block;
		}
	}

	//if (largest)
	//	printf("Largest free block size: 0x%lx\n", largest->Size);
	//else
	if (!largest)
	{
		printf("No free blocks!");
		PrintAllocationsByAddress();
	}

	return largest;
}

tMemoryPool::Block* tMemoryPool::GetFirstFreeBlockThatFitsSize(size_t size)
{
	Block* fitting = NULL;

	// start counting from top
	for (Block* block = BlockList.GetTail(); block != BlockList.EndOfList(); block = block->GetPrev())
	{
		// find earliest spot in memory with block that's big enough
		if (!block->Occupied && block->Size >= size)
		{
			fitting = block;
			break;
		}
	}

	//if (fitting)
	//	printf("Found block of size: 0x%lx\n", fitting->Size);
	//else
	if (!fitting)
	{
		printf("No free blocks!");
		PrintAllocationsByAddress();
	}

	return fitting;
}

void tMemoryPool::PrintAllocationsByAddress()
{
	printf("\nMemoryPool: \"%s\"\n", DebugName);
	puts("Address       Size   Debug Text (& Line)");
	puts("=========================================================");
	// start from the top
	for (Block* block = BlockList.GetTail(); block != BlockList.EndOfList(); block = block->GetPrev())
	{
		if (block->Occupied)
		{
			printf("0x%08x %7u   %s", (intptr_t)block + sizeof(Block), block->Size - sizeof(Block), block->DebugName ? block->DebugName : "Unknown");
			if (block->DebugLine != 0) // only print it if it's set
				printf(", %d", block->DebugLine);
			printf("\n");
		}
	}
}

int tMemoryPool::GetAmountFree()
{
	int amountFree = 0;
	
	for (Block* block = BlockList.EndOfList()->GetPrev(); block != BlockList.GetHead()->GetPrev(); block = block->GetPrev())
	{
		if (!block->Occupied)
			amountFree += block->Size;
	}
	
	return amountFree;
}

int tCountFreeMemory(int poolNum)
{
	return MemoryPools[poolNum]->GetAmountFree();
}

void tInitMemoryPool(int poolNum, void* memory, size_t memorySize, const char* debugName)
{
	tMemoryPoolInfo& poolInfo = MemoryPoolInfoTable[poolNum];
	poolInfo.TopMeansLargerAddress = false;
	poolInfo.OverflowPoolNumber = -1;
	MemoryPools[poolNum] = new((void*)MemoryPoolMem[poolNum]) tMemoryPool();
	MemoryPools[poolNum]->DebugName = debugName;
	MemoryPools[poolNum]->Init(memory, memorySize, debugName, poolNum);
}

tMemoryPool* tGetMemoryPool(int poolNum)
{
	return MemoryPools[poolNum];
}

bool bInitializedMemory = false;

#ifdef EA_PLATFORM_WINDOWS
void tInitializeMemory()
{
	if (!bInitializedMemory)
	{
		tInitMemoryPool(MAIN_POOL, memory, sizeof(memory), "Main Pool");
		bInitializedMemory = true;
	}
}
#elif defined(EA_PLATFORM_GAMECUBE) || defined(EA_PLATFORM_REVOLUTION)
void tInitializeMemory()
{
	if (!bInitializedMemory)
	{
		SYS_STDIO_Report(true);
		size_t mainPoolSize = (((intptr_t)SYS_GetArenaHi() - (intptr_t)SYS_GetArenaLo()) & 0xFFF00000) - 0x100000; // keep 1mb in the heap
		printf("Main pool size: 0x%08x\n", mainPoolSize);
		tInitMemoryPool(0, malloc(mainPoolSize), mainPoolSize, "Main Pool");
		bInitializedMemory = true;
	}
}
#endif

void* tWareMalloc(size_t size, const char* debugText, size_t debugLine, uint32_t allocParams)
{
	uint32_t poolNumber = allocParams & 0xF;
	uint32_t alignment = (allocParams >> 6) & 0x1FFC;

	if (alignment == 0)
		alignment = DEFAULT_ALIGNMENT; // use default alignment if not specified

	if (!bInitializedMemory)
		tInitializeMemory(); // initialize memory before we begin

	tMemoryPool* pool = tGetMemoryPool(poolNumber);
	return pool->AllocateMemory(size, alignment, debugText, debugLine);
}

void tFree(void* const ptr)
{
	if (!ptr)
		return;

	tMemoryPool::Block* memoryBlock = ((tMemoryPool::Block*)ptr) - 1;

	if (memoryBlock->MagicNumber != 0xDEADBEEF)
	{
		if (memoryBlock->MagicNumber == 0xDEADDEAD)
		{
			printf("tFree: This pointer has already been freed! 0x%08x (magic no. 0x%08x)\n", (intptr_t)memoryBlock, memoryBlock->MagicNumber);
		}
		else
		{
			printf("tFree: Not our pointer! 0x%08x (magic no. 0x%08x)\n", (intptr_t)memoryBlock, memoryBlock->MagicNumber);
		}
		
		tBreak(); // bail
	}
	else
	{
		memoryBlock->MagicNumber = 0xDEADDEAD;
		tGetMemoryPool(memoryBlock->PoolNum)->FreeMemory(ptr);
	}
}

void tMemoryPrintAllocationsByAddress(int poolNum)
{
	tGetMemoryPool(poolNum)->PrintAllocationsByAddress();
}

#ifdef EA_COMPILER_MSVC
void* operator new(size_t size) throw(std::bad_alloc)
#else
void* operator new(size_t size)
#endif
{
	void* ptr = tWareMalloc(size, NULL, 0, ALLOC_PARAMS(0, 0));
	
	#ifdef EA_COMPILER_MSVC
	if (!ptr)
		throw new std::bad_alloc;
	#endif

	return ptr;
}

#ifdef EA_COMPILER_MSVC
void* operator new(size_t size, const char* debugName, uint32_t lineNum) throw(std::bad_alloc)
#else
void* operator new(size_t size, const char* debugName, uint32_t lineNum)
#endif
{
	void* ptr = tWareMalloc(size, debugName, lineNum, ALLOC_PARAMS(0, 0));

	#ifdef EA_COMPILER_MSVC
	if (!ptr)
		throw new std::bad_alloc;
	#endif

	return ptr;
}

#ifdef EA_COMPILER_MSVC
void operator delete(void* p) throw()
#else
void operator delete(void* p) _GLIBCXX_TXN_SAFE _GLIBCXX_USE_NOEXCEPT
#endif
{
	tFree(p);
}
