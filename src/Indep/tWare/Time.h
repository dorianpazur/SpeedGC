////////////////////////////////////
//	
//	tWare time ticker handlers
//	

#ifndef TWARETIME_H
#define TWARETIME_H

#include <EABase/eabase.h>

extern void tInitTicker(double minWraparoundTime = 6000.0);
extern unsigned int tGetTicker();
extern double tGetTickerDifference(unsigned int startTick, unsigned int endTick);
extern double tGetTickerDifference(unsigned int startTick);

#endif
