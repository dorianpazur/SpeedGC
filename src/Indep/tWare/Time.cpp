////////////////////////////////////
//	
//	tWare time ticker handlers
//	

#include <tWare/Time.h>
#include <EABase/eabase.h>

#ifdef EA_PLATFORM_GAMECUBE
#include <ogc/system.h>
#include <ogc/lwp_watchdog.h>
#endif

// get current time tick from CPU
unsigned int tGetTicker() {
#if defined(EA_PLATFORM_GAMECUBE) || defined(EA_PLATFORM_REVOLUTION)
    return gettick();
#endif
}

// get difference between two timestamps from tGetTicker in microseconds
float tGetTickerDifference(unsigned int startTick, unsigned int endTick) {
	unsigned int tickDifference;
	
	if (startTick < endTick)
		tickDifference = endTick - startTick;
	else
		tickDifference = startTick - endTick;
	
#if defined(EA_PLATFORM_GAMECUBE) || defined(EA_PLATFORM_REVOLUTION)
    return ticks_to_microsecs(tickDifference) * 0.001f;
#endif
}