////////////////////////////
//	
//	Memory pool manager
//	

#ifndef TWARE_MEMORY_H
#define TWARE_MEMORY_H

#include <tWare/List.h>
#include <cstdint>
#include <memory>
#include <cstring>

#define ALLOC_PARAMS(poolNum, alignment) (poolNum & 0xF) | ((alignment & 0x1FFC) << 6)
#define DEFAULT_ALIGNMENT 16

enum tMemoryPools
{
	MAIN_POOL,
	PHYSICS_POOL,
	TINYGLTF_POOL,
	TMEMORY_POOL_COUNT,
};

extern void* tWareMalloc(size_t size, const char* debugText, size_t debugLine, uint32_t allocParams);
#define tMalloc(size) tWareMalloc(size, __FILE__, __LINE__, ALLOC_PARAMS(MAIN_POOL, 0));
extern void tFree(void* const ptr);
extern void tInitializeMemory();
extern void tInitMemoryPool(int poolNum, void* memory, size_t memorySize, const char* debugName);
extern int tCountFreeMemory(int poolNum);
extern void tMemoryPrintAllocationsByAddress(int poolNum);

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
	int GetAmountFree();
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

#ifdef EA_COMPILER_MSVC

extern void* operator new(size_t size, const char* debugName, uint32_t lineNum) throw(std::bad_alloc);

#define DEF_TWARE_NEW_OVERRIDE(debugName) \
void* operator new(size_t size) throw(std::bad_alloc) \
{ \
	void* ptr = tWareMalloc(size, #debugName, 0, ALLOC_PARAMS(0, 0)); \
	if (!ptr) \
		throw new std::bad_alloc; \
	return ptr; \
} \
void operator delete(void* ptr) throw() \
{ \
	return tFree(ptr); \
}

#define DEF_TWARE_NEW_OVERRIDE_POOL(debugName, poolNum) \
void* operator new(size_t size) throw(std::bad_alloc) \
{ \
	void* ptr = tWareMalloc(size, #debugName, 0, ALLOC_PARAMS(poolNum, 0)); \
	if (!ptr) \
		throw new std::bad_alloc; \
	return ptr; \
} \
void operator delete(void* ptr) throw() \
{ \
	return tFree(ptr); \
}

#else

extern void* operator new(size_t size, const char* debugName, uint32_t lineNum);

#define DEF_TWARE_NEW_OVERRIDE(debugName, poolNum) \
void* operator new(size_t size) \
{ \
	return tWareMalloc(size, #debugName, 0, ALLOC_PARAMS(poolNum, 0)); \
} \
void operator delete(void* ptr) \
{ \
	return tFree(ptr); \
}
#endif

#endif
