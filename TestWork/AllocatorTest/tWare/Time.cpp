////////////////////////////////////
//	
//	tWare time ticker handlers
//	

#include <tWare/Time.h>

#if defined(EA_PLATFORM_GAMECUBE) || defined(EA_PLATFORM_REVOLUTION)
#include <ogc/system.h>
#include <ogc/lwp_watchdog.h>
#elif defined(EA_PLATFORM_WINDOWS)
#include <Windows.h>
#endif

#if defined(EA_PLATFORM_WINDOWS)
unsigned int gTickerWraparound = 0;
double gTickerMicroseconds = 0;
#endif

// based on bWare's bInitTicker
void tInitTicker(double minWraparoundTime)
{
	// not needed on PowerPC, tick rate is fixed there
#if defined(EA_PLATFORM_WINDOWS)
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	gTickerWraparound = 0;
	double microsecondConversion = 1000.0 / freq.QuadPart;

	double freqDivMicros = microsecondConversion * 2147483600.0;
	while (freqDivMicros < minWraparoundTime)
	{
		gTickerWraparound++;
		microsecondConversion *= 2;
		freqDivMicros *= 2;
	};

	gTickerMicroseconds = microsecondConversion;
#endif
}

// get current time tick from CPU
unsigned int tGetTicker()
{
#if defined(EA_PLATFORM_GAMECUBE) || defined(EA_PLATFORM_REVOLUTION)
    return gettick();
#elif defined(EA_PLATFORM_WINDOWS)
	LARGE_INTEGER perfCounter;
	QueryPerformanceCounter(&perfCounter);
	return perfCounter.QuadPart >> gTickerWraparound;
#endif
}

// get difference between two timestamps from tGetTicker in microseconds
double tGetTickerDifference(unsigned int startTick, unsigned int endTick)
{
#ifdef EA_PROCESSOR_POWERPC
	unsigned int tickDifference;
	
	if (startTick < endTick)
		tickDifference = endTick - startTick;
	else
		tickDifference = startTick - endTick;
	
#if defined(EA_PLATFORM_GAMECUBE) || defined(EA_PLATFORM_REVOLUTION)
    return ticks_to_microsecs(tickDifference) * 0.001;
#endif
#elif defined(EA_PLATFORM_WINDOWS)
	if (gTickerMicroseconds != 0.0)
	{
		return (unsigned int)(endTick - startTick) * gTickerMicroseconds;
	}
	else
	{
		tInitTicker();
		return 0.0;
	}
#endif
}

// get difference between now and previous tGetTicker timestamp in microseconds
double tGetTickerDifference(unsigned int startTick)
{
	return tGetTickerDifference(startTick, tGetTicker());
}
