
#include <gccore.h>
#include "Audio.h"

//---------------------------------------------------------------------------------

static lwpq_t thQueue;
static lwp_t thread_handle;
const size_t kAudioStackSize = 0x8000;
uint8_t gAudioStack[kAudioStackSize];

void *AudioTestThread(void*)
{
	while (true)
	{
		tThreadYield();
	}
	
	return NULL;
}

namespace Audio
{
	void Init()
	{
		LWP_InitQueue(&thQueue);
		LWP_CreateThread(&thread_handle, AudioTestThread, NULL, &gAudioStack, kAudioStackSize, 0);
	}
}

