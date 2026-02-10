////////////////////////////
//	
//	Memory pool manager
//	

#ifndef TWARE_MEMORY_H
#define TWARE_MEMORY_H

#include <tWare/List.h>
#include <cstdint>
#include <memory>

#define ALLOC_PARAMS(poolNum, alignment) (poolNum & 0xF) | ((alignment & 0x1FFC) << 6)
#define DEFAULT_ALIGNMENT 16

extern void* tWareMalloc(size_t size, const char* debugText, size_t debugLine, uint32_t allocParams);
extern void tFree(void* const ptr);
extern void tInitializeMemory();
extern void tMemoryPrintAllocationsByAddress(int poolNum);

enum tMemoryPools
{
	MAIN_POOL,
	TMEMORY_POOL_COUNT,
};

struct tMemoryPool : tTNode<tMemoryPool>
{
	struct Block : tTNode<Block>
	{
		size_t Size = 0;
		uint32_t MagicNumber = 0xDEADDEAD;
		uint8_t Occupied = false;
		const char* DebugName = NULL;
		uint16_t DebugLine = 0;
		uint8_t PoolNum = 0;

		Block(size_t size, uint8_t poolNum) : Size(size), PoolNum(poolNum) {};
	};

	const char* DebugName;
	uint32_t PoolNum = 0;
	tTList<Block> BlockList;
	void* MemoryBlock;
	intptr_t InitialAddress;
	size_t InitialSize;
	size_t FreeSize;

	void Init(void* memoryBlock, size_t size, const char* name, uint32_t poolNum);

	// allocations
	void* AllocateMemory(size_t size, uint32_t alignment, const char* debugText, uint32_t debugLine);
	void FreeMemory(void* ptr);

	// block management
	void ConsolidateFreeBlocks();
	Block* GetLargestFreeBlock();
	Block* GetFirstFreeBlockThatFitsSize(size_t size);

	// debugging
	void PrintAllocationsByAddress();
};

struct tMemoryPoolInfo
{
	bool NumberReserved = false;
	bool TopMeansLargerAddress = false;
	int OverflowPoolNumber = -1;
};

template <typename T>
class tStdAllocator {
public:
	typedef T value_type;

	tStdAllocator() = default;

	template <typename U>
	constexpr tStdAllocator(const tStdAllocator<U>&) noexcept {}

	value_type* allocate(size_t n) {
		return (value_type*)tWareMalloc(n, "tStdAllocator::allocate", 0, ALLOC_PARAMS(0, 0));
	}

	void deallocate(value_type* p, size_t n) noexcept {
		tFree(p);
	}
};

#endif
