
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

void EndianSwap(int16_t& i)
{
	uint16_t* data = (uint16_t*)&i;
	uint16_t swap = (*data >> 8) | (*data << 8);
	i = *(int16_t*)&swap;
}

void EndianSwap(uint16_t& i)
{
	uint16_t* data = (uint16_t*)&i;
	uint16_t swap = (*data >> 8) | (*data << 8);
	i = *(uint16_t*)&swap;
}

void EndianSwap(int32_t& i)
{
	uint32_t* data = (uint32_t*)&i;
	uint32_t swap =	((*data & 0xFF000000) >> 24) |
					((*data & 0x00FF0000) >> 8) |
					((*data & 0x0000FF00) << 8) |
					((*data & 0x000000FF) << 24);
	
	i = *(int32_t*)&swap;
}

void EndianSwap(uint32_t& i)
{
	uint32_t* data = (uint32_t*)&i;
	uint32_t swap =	((*data & 0xFF000000) >> 24) |
					((*data & 0x00FF0000) >> 8) |
					((*data & 0x0000FF00) << 8) |
					((*data & 0x000000FF) << 24);
	
	i = *(uint32_t*)&swap;
}

int16_t EndianSwapSample(int16_t i)
{
	uint16_t* data = (uint16_t*)&i;
	uint16_t swap = (*data >> 8) | (*data << 8);
	return *(int16_t*)&swap;
}

//---------------------------------------------------------------------------------

struct WaveHeader
{
	struct RIFFChunk
	{
		// RIFF header
		
		uint32_t ChunkID;
		uint32_t ChunkSize;
		uint32_t ChunkFmt;
		
		void EndianSwapData()
		{
			EndianSwap(ChunkID);
			EndianSwap(ChunkSize);
			EndianSwap(ChunkFmt);
		}
	};
	
	struct FmtChunk
	{
		// RIFF header
		uint32_t ChunkID;
		uint32_t ChunkSize;
		uint16_t AudioFormat;
		uint16_t NumChannels;
		uint32_t SampleRate;
		uint32_t ByteRate;
		uint16_t BlockAlign;
		uint16_t BitsPerSample;
		
		void EndianSwapData()
		{
			EndianSwap(ChunkID);
			EndianSwap(ChunkSize);
			EndianSwap(AudioFormat);
			EndianSwap(NumChannels);
			EndianSwap(SampleRate);
			EndianSwap(ByteRate);
			EndianSwap(BlockAlign);
			EndianSwap(BitsPerSample);
		}
	};
	
	struct DataChunk
	{
		// RIFF header
		uint32_t ChunkID;
		uint32_t ChunkSize;
		
		void EndianSwapData()
		{
			EndianSwap(ChunkID);
			EndianSwap(ChunkSize);
		}
	};
	
	RIFFChunk RIFF;
	FmtChunk fmt;
	DataChunk data;
	
	void EndianSwapData()
	{
		RIFF.EndianSwapData();
		fmt.EndianSwapData();
		data.EndianSwapData();
	}
};

struct AudioStream
{
	FILE* file;
	
	WaveHeader header;
	double dspTime = 0;
	size_t size = 0;
	bool playing = false;
	
	#define AudioStreamAssert(cond) \
	if (!(cond)) \
	{ \
		printf("AudioStreamAssert failed on line %u: %s was false\n", __LINE__, #cond); \
		fclose(file); \
		gFileIOMutex.Unlock(); \
		return false; \
	}
	
	bool OpenFile(const char* path)
	{
		char realPath[TFILE_MAX_PATH + 1];
	
		snprintf(realPath, TFILE_MAX_PATH, "%s%s", gBaseDir, path);
		
		gFileIOMutex.Lock();
		file = fopen(realPath, "rb");
		
		if (!file)
		{
			gFileIOMutex.Unlock();
			return false;
		}
		
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fseek(file, 0, SEEK_SET);
		
		AudioStreamAssert(size > sizeof(header)); // definitely not a wave file if smaller than header
		
		fread(&header, sizeof(header), 1, file); // read header
		
		header.EndianSwapData(); // endian swap the data within
		
		// check header if it's valid
		
		AudioStreamAssert(header.RIFF.ChunkID == 0x46464952); // check if says RIFF
		AudioStreamAssert(header.RIFF.ChunkFmt == 0x45564157); // check if says WAVE
		
		AudioStreamAssert(header.fmt.ChunkID == 0x20746D66); // check if says WAVE
		AudioStreamAssert(header.fmt.AudioFormat == 1); // check if wave is PCM
		
		AudioStreamAssert(header.data.ChunkID == 0x61746164); // check if says data	
		
		gFileIOMutex.Unlock();
		playing = true;
		printf("Now Playing: %s\n", path);
		return true;
	};
	
	void GetData(void* dest, size_t bufferSize)
	{
		if (file)
		{	
			gFileIOMutex.Lock();
			
			if (((size_t)ftell(file)) > size - bufferSize)
				fseek(file, sizeof(header), SEEK_SET); // go back to start of audio buffer to loop
			
			fread(dest, bufferSize, 1, file);
			
			gFileIOMutex.Unlock();
		}
	}
	
	void CloseFile()
	{
		gFileIOMutex.Lock();
		if (file)
			fclose(file);
		gFileIOMutex.Unlock();
		playing = false;
		file = NULL;
	};
	
	~AudioStream()
	{
		CloseFile();
	};
	
	#undef AudioStreamAssert
};

AudioStream stream;

static lwpq_t thQueue;
static lwp_t thread_handle;
const size_t kAudioStackSize = 0x8000;
uint8_t gAudioStack[kAudioStackSize];

const size_t kNumSecondsOfBuffer = 1;
const size_t kNumSamplesInBuffer = 32000 * kNumSecondsOfBuffer; // second
const size_t kNumChannels = 2;
const size_t kBufferSize = kNumSamplesInBuffer * sizeof(uint16_t) * kNumChannels;
int16_t *streamingBuffer[2] ATTRIBUTE_ALIGN(32); // double buffered 32000 stereo samples

tMutex gAudioMutexBuf1;
tMutex gAudioMutexBuf2;
tMutex gDSPTimeMutex;

void UpdateAudio()
{
	static uint64_t prevFrame = -1;
	static uint64_t fileOffset = 0;
	
	gDSPTimeMutex.Lock();
	uint64_t curFrame = 1 - ((uint64_t)stream.dspTime / kNumSamplesInBuffer); // get the other frame
	gDSPTimeMutex.Unlock();
	
	if (prevFrame != curFrame)
	{
		tMutex& mutex = curFrame ? gAudioMutexBuf2 : gAudioMutexBuf1;
		
		if (stream.playing)
		{
			mutex.Lock();
			stream.GetData(streamingBuffer[curFrame], kBufferSize);
			mutex.Unlock();
		}
		
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
	
	if (streamingBuffer[0] && streamingBuffer[1] && stream.playing)
	{
		uint32_t currentBuffer;
		uint32_t currentSample;
		
		for (int i = 0; i < 160; i++)
		{
			currentBuffer = ((uint64_t)stream.dspTime) / kNumSamplesInBuffer;
			currentSample = ((uint64_t)stream.dspTime) % kNumSamplesInBuffer;
			
			bufL[other ? 0 : 1][i] = EndianSwapSample(streamingBuffer[currentBuffer][(currentSample * stream.header.fmt.NumChannels) + 0]);
			bufR[other ? 0 : 1][i] = EndianSwapSample(streamingBuffer[currentBuffer][(currentSample * stream.header.fmt.NumChannels) + 1]);
			
			stream.dspTime = fmod(stream.dspTime + (stream.header.fmt.SampleRate / 32000.0), (kNumSamplesInBuffer * 2)); // both buffers
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
		
		stream.OpenFile("Audio/Music/title.wav");
	}
	
	void Update()
	{
		//UpdateAudio();
	}
}

