#ifndef __AI_H__
#define __AI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------------*/
#include <gccore.h>

/*---------------------------------------------------------------------------*
 * Definitions
 *---------------------------------------------------------------------------*/

#define AI_STREAM_START      0x00000001   // Audio streaming sample clock
#define AI_STREAM_STOP       0x00000000

#define AI_SAMPLERATE_48KHZ  0x00000001   // SRC sample rates for DSP
#define AI_SAMPLERATE_32KHZ  0x00000000
 
/*---------------------------------------------------------------------------*
 * Globals
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Function Prototypes
 *---------------------------------------------------------------------------*/

// AI-FIFO DMA ---------------------------------------------------------------
#define AIRegisterDMACallback AUDIO_RegisterDMACallback

#define AIInitDMA          AUDIO_InitDMA         
#define AIGetDMAEnableFlag AUDIO_GetDMAEnableFlag
#define AIStartDMA         AUDIO_StartDMA        
#define AIStopDMA          AUDIO_StopDMA         
#define AIGetDMABytesLeft  AUDIO_GetDMABytesLeft

#define AIGetDMAStartAddr  AUDIO_GetDMAStartAddr 
#define AIGetDMALength     AUDIO_GetDMALength    

#define AIGetDSPSampleRate AUDIO_GetDSPSampleRate
#define AISetDSPSampleRate AUDIO_SetDSPSampleRate

   
// Streaming Audio API --------------------------------------------------------
#define AIRegisterStreamCallback    AUDIO_RegisterStreamCallback

#define AIGetStreamSampleCount      AUDIO_GetStreamSampleCount
#define AIResetStreamSampleCount    AUDIO_ResetStreamSampleCount;

#define AISetStreamTrigger          AUDIO_SetStreamTrigger
#define AIGetStreamTrigger          AUDIO_GetStreamTrigger

#define AISetStreamPlayState        AUDIO_GetStreamTrigger
#define AIGetStreamPlayState        AUDIO_GetStreamTrigger

#define AISetStreamSampleRate       AUDIO_SetStreamSampleRate
#define AIGetStreamSampleRate       AUDIO_GetStreamSampleRate

#define AISetStreamVolLeft          AUDIO_SetStreamVolLeft
#define AISetStreamVolRight         AUDIO_SetStreamVolLeft

#define AIGetStreamVolLeft          AUDIO_GetStreamVolLeft
#define AIGetStreamVolRight         AUDIO_GetStreamVolLeft

// General/initialization ------------------------------------------------------
#define AIInit AUDIO_Init
inline BOOL AICheckInit() { return false; }; // libogc checks internally
inline void AIReset (void) {} // unsupported by libogc

#ifdef __cplusplus
}
#endif

#endif
