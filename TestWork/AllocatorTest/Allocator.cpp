// allocator test

#include <chrono>
#include <thread>
#include <cstddef>
#include <cstdio>
#include <tWare/List.h>
#include <tWare/Time.h>
#include <tWare/Align.h>
#include <EABase/eabase.h>
#include <tWare/File.h>
#include <tWare/Memory.h>
#include <tWare/Debug.h>

class ALIGN(32) TestClass
{
public:
	char aow[16] = { "Awoo" };
	char mrrp[8] = { ":3" };
	uint32_t meep = 0x24242424;

	void* operator new(size_t size) throw(std::bad_alloc)
	{
		void* ptr = tWareMalloc(size, "TestClass", __LINE__, ALLOC_PARAMS(0, 0));

		if (!ptr)
			throw new std::bad_alloc;

		return ptr;
	}
};

int main()
{
	void* address = tWareMalloc(0x20, "0xBD", __LINE__, ALLOC_PARAMS(MAIN_POOL, 0));
	memset(address, 0xBD, 0x20);
	void* address2 = tWareMalloc(0x20, "0xD4", __LINE__, ALLOC_PARAMS(MAIN_POOL, 0));
	memset(address2, 0xD4, 0x20);
	tFree(address);
	void* address3 = tWareMalloc(0x4, "0xDEADDEAD", __LINE__, ALLOC_PARAMS(MAIN_POOL, 0));
	*(uint32_t*)address3 = 0xADDEADDE; // DEADDEAD but little endian

	tFree(address3);

	size_t meowSize = sizeof("meo");
	char* testStringThing = (char*)tWareMalloc(meowSize, "meow text (5 bytes)", __LINE__, ALLOC_PARAMS(MAIN_POOL, 1));
	memcpy(testStringThing, "meo", meowSize);

	TestClass* testClass1 = new TestClass();
	TestClass* testClass1_ = testClass1;

	delete testClass1;

	TestClass* testClass2 = new TestClass(); // recreate it, should be put in the same spot
	TestClass* testClass2_ = testClass2;

	_ASSERTE(testClass2 == testClass1_);

	delete testClass2;

	const char* test32CharStr = "Test string that is 32 chs long";
	const char* testNot32CharStr = "Not 32 chars long";

	char* testStringThing2 = (char*)tWareMalloc(strlen(test32CharStr), "test string", __LINE__, ALLOC_PARAMS(MAIN_POOL, 0));
	memcpy(testStringThing2, test32CharStr, strlen(test32CharStr)); // this should be in the same place

	_ASSERTE((void*)testStringThing2 == (void*)testClass2_);

	char* testStringThing3 = (char*)tWareMalloc(strlen(testNot32CharStr), "test string2", __LINE__, ALLOC_PARAMS(MAIN_POOL, 0));
	memcpy(testStringThing3, test32CharStr, strlen(testNot32CharStr)); // this should be in the same region but not the same place

	_ASSERTE((void*)testStringThing3 != (void*)testClass2_);

	TestClass* testClass3 = new TestClass(); // recreate it, now should be put further forwards

	tFree(testStringThing);
	char* testStringThing4 = (char*)tWareMalloc(sizeof("meow"), "meow", __LINE__, ALLOC_PARAMS(MAIN_POOL, 0));
	memcpy(testStringThing4, "meow", sizeof("meow")); // this should be put in the same place

	char* testStringThing5 = (char*)tWareMalloc(sizeof("32 align"), "32 byte align", __LINE__, ALLOC_PARAMS(MAIN_POOL, 32));
	memcpy(testStringThing5, "32 align", sizeof("32 align")); // this should be put in the same place

	_ASSERTE(((intptr_t)testStringThing5) % 32 == 0); // this should be aligned to a 32-byte boundary

	char* testStringThing6 = (char*)tWareMalloc(sizeof("64 align"), "64 byte align", __LINE__, ALLOC_PARAMS(MAIN_POOL, 64));
	memcpy(testStringThing6, "64 align", sizeof("64 align")); // this should be put in the same place

	_ASSERTE(((intptr_t)testStringThing6) % 64 == 0); // this should be aligned to a 64-byte boundary

	tFile* file = tOpenFile("Allocator.cpp");

	tMemoryPrintAllocationsByAddress(MAIN_POOL);

	tCloseFile(file);

	tMemoryPrintAllocationsByAddress(MAIN_POOL);

	file = tOpenFile("Allocator.cpp");
	
	tMemoryPrintAllocationsByAddress(MAIN_POOL);

	system("pause");
	return 0;
}
