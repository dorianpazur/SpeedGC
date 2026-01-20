////////////////////////////////////
//	
//	tWare time ticker handlers
//	

#include <tWare/Time.h>

#ifdef GEKKO
#include <ogc/system.h>
#include <ogc/lwp_watchdog.h>
#endif

// get current time tick from CPU
unsigned int tGetTicker() {
#ifdef GEKKO
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
	
#ifdef GEKKO
    return ticks_to_microsecs(tickDifference) * 0.001f;
#endif
}