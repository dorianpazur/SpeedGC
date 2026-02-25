
#include <gccore.h>
#include "Audio.h"
#include <dolphin/ax.h>
#include <dolphin/mix.h>
#include <dolphin/os.h>
#include <tWare/File.h>
#include <tWare/Memory.h>
#include <tWare/Thread.h>
#include "ScreenPrintf.h"

//---------------------------------------------------------------------------------

uint64_t dspTime = 0;

static lwpq_t thQueue;
static lwp_t thread_handle;
const size_t kAudioStackSize = 0x8000;
uint8_t gAudioStack[kAudioStackSize];

const size_t kNumSecondsOfBuffer = 1;
const size_t kNumSamplesInBuffer = 32000 * kNumSecondsOfBuffer; // second
const size_t kNumChannels = 2;
const size_t kBufferSize = kNumSamplesInBuffer * sizeof(uint16_t) * kNumChannels;
int16_t *streamingBuffer[2] ATTRIBUTE_ALIGN(32); // double buffered 32000 stereo samples

//FILE* audioFile;

tMutex gAudioMutexBuf1;
tMutex gAudioMutexBuf2;
tMutex gDSPTimeMutex;

inline int16_t EndianSwapI16(int16_t i)
{
	uint16_t* data = (uint16_t*)&i;
	uint16_t swap = (*data >> 8) | (*data << 8);
	return *(int16_t*)&swap;
}

void UpdateAudio()
{
	static uint64_t prevFrame = -1;
	static uint64_t fileOffset = 0;
	
	char path[TFILE_MAX_PATH + 1];
	
	snprintf(path, TFILE_MAX_PATH, "%s%s", gBaseDir, "title.raw");
	
	gDSPTimeMutex.Lock();
	uint64_t curFrame = 1 - (dspTime / kNumSamplesInBuffer); // get the other frame
	gDSPTimeMutex.Unlock();
	
	if (prevFrame != curFrame)
	{
		tMutex& mutex = curFrame ? gAudioMutexBuf2 : gAudioMutexBuf1;
		
		gFileIOMutex.Lock();
		
		static FILE* audioFile = fopen(path, "rb");
		
		if (audioFile)
		{
			static long size = -1;
			if (size == -1)
			{
				fseek(audioFile, 0, SEEK_END);
				size = ftell(audioFile);
				fseek(audioFile, 0, SEEK_SET);
			}
			
			if (fileOffset > size - kBufferSize)
				fileOffset = 0;
			
			//fseek(audioFile, fileOffset, SEEK_SET);
			
			mutex.Lock();
			
			fread(streamingBuffer[curFrame], kBufferSize, 1, audioFile);
			
			mutex.Unlock();
			
			fileOffset = ftell(audioFile);
		}
	
		//fclose(audioFile);
		gFileIOMutex.Unlock();
		//audioFile = NULL;
		
		prevFrame = curFrame;
	}
}

void *AudioThread(void*)
{
	while (true)
	{
		UpdateAudio();
		
		tThreadYield();
	}
	
	return NULL;
}

typedef struct
{
    s32 *l; // pointer to left aux channel buffer in main memory
    s32 *r; // pointer to right aux channel buffer in main memory
    s32 *s; // pointer to surround aux channel buffer in main memory
} aux_data;

tFile* music;
bool play = false;

int32_t bufL[2][160] ATTRIBUTE_ALIGN(32);
int32_t bufR[2][160] ATTRIBUTE_ALIGN(32);

void AudioFrameCallback()
{
	static bool other = true;
	
	if (streamingBuffer[0] && streamingBuffer[1])
	{
		uint32_t currentBuffer;
		uint32_t currentSample;
		
		for (int i = 0; i < 160; i++)
		{
			currentBuffer = (dspTime) / kNumSamplesInBuffer;
			currentSample = (dspTime) % kNumSamplesInBuffer;
			
			bufL[other ? 0 : 1][i] = EndianSwapI16(streamingBuffer[currentBuffer][(currentSample * kNumChannels) + 0]);
			bufR[other ? 0 : 1][i] = EndianSwapI16(streamingBuffer[currentBuffer][(currentSample * kNumChannels) + 1]);
			
			dspTime = (dspTime + 1) % (kNumSamplesInBuffer * 2); // both buffers
		}
		
		DCFlushRange(bufL[other ? 0 : 1], 160 * sizeof(int32_t));
		DCFlushRange(bufR[other ? 0 : 1], 160 * sizeof(int32_t));
	}
	
	other = !other;
}

void AuxACallback(void *data, void *context)
{
	static bool other = false;
	const int len = 160;
	
	aux_data* auxData = (aux_data*)data;
	
	for (int i = 0; i < len; i++)
	{
		auxData->l[i] += bufL[other ? 0 : 1][i];
		auxData->r[i] += bufR[other ? 0 : 1][i];
	}
	
	other = !other;
}

namespace Audio
{
	void Init()
	{
		// audio
		AR_Init(NULL, 0);
		ARQ_Init();
		AUDIO_Init(NULL);
		AXInit();
		AXSetCompressor(AX_COMPRESSOR_OFF);
		AXRegisterAuxACallback(AuxACallback, NULL);
		AXRegisterCallback(&AudioFrameCallback);
		AXSetMode(AX_MODE_STEREO);
		
		DCFlushRangeNoSync(bufL, sizeof(bufL));
		DCFlushRangeNoSync(bufR, sizeof(bufR));
		
		streamingBuffer[0] = (int16_t*)tWareMalloc(kBufferSize, "Audio Buffer 1", __LINE__, ALLOC_PARAMS(MAIN_POOL, 32));
		streamingBuffer[1] = (int16_t*)tWareMalloc(kBufferSize, "Audio Buffer 2", __LINE__, ALLOC_PARAMS(MAIN_POOL, 32));
		
		LWP_InitQueue(&thQueue);
		LWP_CreateThread(&thread_handle, AudioThread, NULL, &gAudioStack, kAudioStackSize, 0);
		
		//music = tOpenFile("subwaysofyourmind.raw");
		//int16_t* data = (int16_t*)music->data;
		//for (size_t i = 0; i < music->filesize / 2; i++)
		//{
		//	data[i] = EndianSwapI16(data[i]);
		//}
		
		play = true;
	}
	
	void Update()
	{
		//UpdateAudio();
	}
}

