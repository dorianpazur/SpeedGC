#ifndef _DOLPHIN_AX_H_
#define _DOLPHIN_AX_H_

#include <gccore.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _AXPBMIX {
    /* 0x00 */ u16 vL;
    /* 0x02 */ u16 vDeltaL;
    /* 0x04 */ u16 vR;
    /* 0x06 */ u16 vDeltaR;
    /* 0x08 */ u16 vAuxAL;
    /* 0x0A */ u16 vDeltaAuxAL;
    /* 0x0C */ u16 vAuxAR;
    /* 0x0E */ u16 vDeltaAuxAR;
    /* 0x10 */ u16 vAuxBL;
    /* 0x12 */ u16 vDeltaAuxBL;
    /* 0x14 */ u16 vAuxBR;
    /* 0x16 */ u16 vDeltaAuxBR;
    /* 0x18 */ u16 vAuxBS;
    /* 0x1A */ u16 vDeltaAuxBS;
    /* 0x1C */ u16 vS;
    /* 0x1E */ u16 vDeltaS;
    /* 0x20 */ u16 vAuxAS;
    /* 0x22 */ u16 vDeltaAuxAS;
} AXPBMIX;

typedef struct _AXPBITD {
    /* 0x00 */ u16 flag;
    /* 0x02 */ u16 bufferHi;
    /* 0x04 */ u16 bufferLo;
    /* 0x06 */ u16 shiftL;
    /* 0x08 */ u16 shiftR;
    /* 0x0A */ u16 targetShiftL;
    /* 0x0C */ u16 targetShiftR;
} AXPBITD;

typedef struct _AXPBUPDATE {
    /* 0x00 */ u16 updNum[5];
    /* 0x0A */ u16 dataHi;
    /* 0x0C */ u16 dataLo;
} AXPBUPDATE;

typedef struct _AXPBDPOP {
    /* 0x00 */ s16 aL;
    /* 0x02 */ s16 aAuxAL;
    /* 0x04 */ s16 aAuxBL;
    /* 0x06 */ s16 aR;
    /* 0x08 */ s16 aAuxAR;
    /* 0x0A */ s16 aAuxBR;
    /* 0x0C */ s16 aS;
    /* 0x0E */ s16 aAuxAS;
    /* 0x10 */ s16 aAuxBS;
} AXPBDPOP;

typedef struct _AXPBVE {
    /* 0x00 */ u16 currentVolume;
    /* 0x02 */ s16 currentDelta;
} AXPBVE;

typedef struct _AXPBFIR {
    /* 0x00 */ u16 numCoefs;
    /* 0x02 */ u16 coefsHi;
    /* 0x04 */ u16 coefsLo;
} AXPBFIR;

typedef struct _AXPBADDR {
    /* 0x00 */ u16 loopFlag;
    /* 0x02 */ u16 format;
    /* 0x04 */ u16 loopAddressHi;
    /* 0x06 */ u16 loopAddressLo;
    /* 0x08 */ u16 endAddressHi;
    /* 0x0A */ u16 endAddressLo;
    /* 0x0C */ u16 currentAddressHi;
    /* 0x0E */ u16 currentAddressLo;
} AXPBADDR;

typedef struct _AXPBADPCM {
    /* 0x00 */ u16 a[8][2];
    /* 0x20 */ u16 gain;
    /* 0x22 */ u16 pred_scale;
    /* 0x24 */ u16 yn1;
    /* 0x26 */ u16 yn2;
} AXPBADPCM;

typedef struct _AXPBSRC {
    /* 0x00 */ u16 ratioHi;
    /* 0x02 */ u16 ratioLo;
    /* 0x04 */ u16 currentAddressFrac;
    /* 0x06 */ u16 last_samples[4];
} AXPBSRC;

typedef struct _AXPBADPCMLOOP {
    /* 0x00 */ u16 loop_pred_scale;
    /* 0x02 */ u16 loop_yn1;
    /* 0x04 */ u16 loop_yn2;
} AXPBADPCMLOOP;

typedef struct _AXPBLPF {
    u16 on;
    u16 yn1;
    u16 a0;
    u16 b0;
} AXPBLPF;

typedef struct _AXPB {
    /* 0x00 */ u16 nextHi;
    /* 0x02 */ u16 nextLo;
    /* 0x04 */ u16 currHi;
    /* 0x06 */ u16 currLo;
    /* 0x08 */ u16 srcSelect;
    /* 0x0A */ u16 coefSelect;
    /* 0x0C */ u16 mixerCtrl;
    /* 0x0E */ u16 state;
    /* 0x10 */ u16 type;
    /* 0x12 */ AXPBMIX mix;
    /* 0x36 */ AXPBITD itd;
    /* 0x44 */ AXPBUPDATE update;
    /* 0x52 */ AXPBDPOP dpop;
    /* 0x64 */ AXPBVE ve;
    /* 0x68 */ AXPBFIR fir;
    /* 0x6E */ AXPBADDR addr;
    /* 0x7E */ AXPBADPCM adpcm;
    /* 0xA6 */ AXPBSRC src;
    /* 0xB4 */ AXPBADPCMLOOP adpcmLoop;
    /* 0xBA */ AXPBLPF lpf;
    /* 0xC2 */ u16 pad[25];
} AXPB;

typedef struct _AXVPB {
    /* 0x000 */ void* next;
    /* 0x004 */ void* prev;
    /* 0x008 */ void* next1;
    /* 0x00C */ u32 priority;
    /* 0x010 */ void (*callback)(void*);
    /* 0x014 */ u32 userContext;
    /* 0x018 */ u32 index;
    /* 0x01C */ u32 sync;
    /* 0x020 */ u32 depop;
    /* 0x024 */ u32 updateMS;
    /* 0x028 */ u32 updateCounter;
    /* 0x02C */ u32 updateTotal;
    /* 0x030 */ u16* updateWrite;
    /* 0x034 */ u16 updateData[128];
    /* 0x134 */ void* itdBuffer;
    /* 0x138 */ AXPB pb;
} AXVPB;

typedef struct _AXPBITDBUFFER {
    /* 0x00 */ s16 data[32];
} AXPBITDBUFFER;

typedef struct _AXPBU {
    /* 0x00 */ u16 data[128];
} AXPBU;

typedef struct _AXSPB {
    /* 0x00 */ u16 dpopLHi;
    /* 0x02 */ u16 dpopLLo;
    /* 0x04 */ s16 dpopLDelta;
    /* 0x06 */ u16 dpopRHi;
    /* 0x08 */ u16 dpopRLo;
    /* 0x0A */ s16 dpopRDelta;
    /* 0x0C */ u16 dpopSHi;
    /* 0x0E */ u16 dpopSLo;
    /* 0x10 */ s16 dpopSDelta;
    /* 0x12 */ u16 dpopALHi;
    /* 0x14 */ u16 dpopALLo;
    /* 0x16 */ s16 dpopALDelta;
    /* 0x18 */ u16 dpopARHi;
    /* 0x1A */ u16 dpopARLo;
    /* 0x1C */ s16 dpopARDelta;
    /* 0x1E */ u16 dpopASHi;
    /* 0x20 */ u16 dpopASLo;
    /* 0x22 */ s16 dpopASDelta;
    /* 0x24 */ u16 dpopBLHi;
    /* 0x26 */ u16 dpopBLLo;
    /* 0x28 */ s16 dpopBLDelta;
    /* 0x2A */ u16 dpopBRHi;
    /* 0x2C */ u16 dpopBRLo;
    /* 0x2E */ s16 dpopBRDelta;
    /* 0x30 */ u16 dpopBSHi;
    /* 0x32 */ u16 dpopBSLo;
    /* 0x34 */ s16 dpopBSDelta;
} AXSPB;

typedef struct _AXPROFILE {
    /* 0x00 */ u64 axFrameStart;
    /* 0x08 */ u64 auxProcessingStart;
    /* 0x10 */ u64 auxProcessingEnd;
    /* 0x18 */ u64 userCallbackStart;
    /* 0x20 */ u64 userCallbackEnd;
    /* 0x28 */ u64 axFrameEnd;
    /* 0x30 */ u32 axNumVoices;
} AXPROFILE;

typedef struct AX_AUX_DATA {
    /* 0x00 */ s32* l;
    /* 0x00 */ s32* r;
    /* 0x00 */ s32* s;
} AX_AUX_DATA;

typedef struct AX_AUX_DATA_DPL2 {
    /* 0x00 */ s32* l;
    /* 0x00 */ s32* r;
    /* 0x00 */ s32* ls;
    /* 0x00 */ s32* rs;
} AX_AUX_DATA_DPL2;


typedef struct
{
    u32 num_samples;       // total number of RAW samples
    u32 num_adpcm_nibbles; // number of ADPCM nibbles (including frame headers)
    u32 sample_rate;       // Sample rate, in Hz
    u16 loop_flag;         // 1=LOOPED, 0=NOT LOOPED
    u16 format;            // Always 0x0000, for ADPCM
    u32 sa;                // Start offset address for looped samples (zero for non-looped)
    u32 ea;                // End offset address for looped samples
    u32 ca;                // always zero
    u16 coef[16];          // decode coefficients (eight pairs of 16-bit words)
    u16 gain;              // always zero for ADPCM
    u16 ps;                // predictor/scale
    u16 yn1;               // sample history
    u16 yn2;               // sample history
    u16 lps;               // predictor/scale for loop context
    u16 lyn1;              // sample history (n-1) for loop context
    u16 lyn2;              // sample history (n-2) for loop context
    u16 pad[11];           // reserved
} DSPADPCM;

typedef void (*AXCallback)();

/*---------------------------------------------------------------------------*
 *---------------------------------------------------------------------------*/
#define AX_DSP_CYCLES_PBSYNC        2000
#define AX_DSP_CYCLES               (OS_BUS_CLOCK / 400)

/*---------------------------------------------------------------------------*
 *---------------------------------------------------------------------------*/
#define AX_MAX_VOICES               64

#define AX_MS_PER_FRAME             5

#define AX_IN_SAMPLES_PER_MS        32
#define AX_IN_SAMPLES_PER_SEC       (AX_IN_SAMPLES_PER_MS * 1000)
#define AX_IN_SAMPLES_PER_FRAME     (AX_IN_SAMPLES_PER_MS * AX_MS_PER_FRAME)

/*---------------------------------------------------------------------------*
 *---------------------------------------------------------------------------*/
#define AX_MODE_STEREO              0
#define AX_MODE_SURROUND            1
#define AX_MODE_DPL2                2

/*---------------------------------------------------------------------------*
 *---------------------------------------------------------------------------*/
#define AX_COMPRESSOR_OFF           0
#define AX_COMPRESSOR_ON            1

/*---------------------------------------------------------------------------*
 *---------------------------------------------------------------------------*/
#define AX_PRIORITY_STACKS          32
#define AX_PRIORITY_NODROP          (AX_PRIORITY_STACKS - 1)
#define AX_PRIORITY_LOWEST          1
#define AX_PRIORITY_FREE            0

/*---------------------------------------------------------------------------*
 *---------------------------------------------------------------------------*/
#define AX_SRC_TYPE_NONE            0
#define AX_SRC_TYPE_LINEAR          1
#define AX_SRC_TYPE_4TAP_8K         2
#define AX_SRC_TYPE_4TAP_12K        3
#define AX_SRC_TYPE_4TAP_16K        4

/*---------------------------------------------------------------------------*
 *---------------------------------------------------------------------------*/
#define AX_ADDR_ONESHOT             0
#define AX_ADDR_LOOP                1

/*---------------------------------------------------------------------------*
 *---------------------------------------------------------------------------*/
#define AX_SYNC_NONEWPARAMS         0x00000000
#define AX_SYNC_USER_SRCSELECT      0x00000001
#define AX_SYNC_USER_MIXCTRL        0x00000002
#define AX_SYNC_USER_STATE          0x00000004
#define AX_SYNC_USER_TYPE           0x00000008
#define AX_SYNC_USER_MIX            0x00000010
#define AX_SYNC_USER_ITD            0x00000020
#define AX_SYNC_USER_ITDTARGET      0x00000040
#define AX_SYNC_USER_UPDATE         0x00000080
#define AX_SYNC_USER_DPOP           0x00000100
#define AX_SYNC_USER_VE             0x00000200
#define AX_SYNC_USER_VEDELTA        0x00000400
#define AX_SYNC_USER_FIR            0x00000800
#define AX_SYNC_USER_ADDR           0x00001000
#define AX_SYNC_USER_LOOP           0x00002000
#define AX_SYNC_USER_LOOPADDR       0x00004000
#define AX_SYNC_USER_ENDADDR        0x00008000
#define AX_SYNC_USER_CURRADDR       0x00010000
#define AX_SYNC_USER_ADPCM          0x00020000
#define AX_SYNC_USER_SRC            0x00040000
#define AX_SYNC_USER_SRCRATIO       0x00080000
#define AX_SYNC_USER_ADPCMLOOP      0x00100000
#define AX_SYNC_USER_LPF            0x00200000
#define AX_SYNC_USER_LPF_COEF       0x00400000
#define AX_SYNC_USER_ALLPARAMS      0x80000000

/*---------------------------------------------------------------------------*
 *---------------------------------------------------------------------------*/
#define AX_OUTPUT_BUFFER_DOUBLE     0
#define AX_OUTPUT_BUFFER_TRIPLE     1

#define AX_DSP_SLAVE_LENGTH 0xF80
#define AX_MAX_VOICES 64

#define AX_SRC_TYPE_NONE     0
#define AX_SRC_TYPE_LINEAR   1
#define AX_SRC_TYPE_4TAP_8K  2
#define AX_SRC_TYPE_4TAP_12K 3
#define AX_SRC_TYPE_4TAP_16K 4

// sync flags
#define AX_SYNC_FLAG_COPYALL       (1 << 31)
#define AX_SYNC_FLAG_UNK1          (1 << 30) // reserved, unused?
#define AX_SYNC_FLAG_UNK2          (1 << 29) // reserved, unused?
#define AX_SYNC_FLAG_UNK3          (1 << 28) // reserved, unused?
#define AX_SYNC_FLAG_UNK4          (1 << 27) // reserved, unused?
#define AX_SYNC_FLAG_UNK5          (1 << 26) // reserved, unused?
#define AX_SYNC_FLAG_UNK6          (1 << 25) // reserved, unused?
#define AX_SYNC_FLAG_UNK7          (1 << 24) // reserved, unused?
#define AX_SYNC_FLAG_UNK8          (1 << 23) // reserved, unused?
#define AX_SYNC_FLAG_UNK9          (1 << 22) // reserved, unused?
#define AX_SYNC_FLAG_UNK10         (1 << 21) // reserved, unused?
#define AX_SYNC_FLAG_COPYADPCMLOOP (1 << 20)
#define AX_SYNC_FLAG_COPYRATIO     (1 << 19)
#define AX_SYNC_FLAG_COPYSRC       (1 << 18)
#define AX_SYNC_FLAG_COPYADPCM     (1 << 17)
#define AX_SYNC_FLAG_COPYCURADDR   (1 << 16)
#define AX_SYNC_FLAG_COPYENDADDR   (1 << 15)
#define AX_SYNC_FLAG_COPYLOOPADDR  (1 << 14)
#define AX_SYNC_FLAG_COPYLOOP      (1 << 13)
#define AX_SYNC_FLAG_COPYADDR      (1 << 12)
#define AX_SYNC_FLAG_COPYFIR       (1 << 11)
#define AX_SYNC_FLAG_SWAPVOL       (1 << 10)
#define AX_SYNC_FLAG_COPYVOL       (1 << 9)
#define AX_SYNC_FLAG_COPYDPOP      (1 << 8)
#define AX_SYNC_FLAG_COPYUPDATE    (1 << 7)
#define AX_SYNC_FLAG_COPYTSHIFT    (1 << 6)
#define AX_SYNC_FLAG_COPYITD       (1 << 5)
#define AX_SYNC_FLAG_COPYAXPBMIX   (1 << 4)
#define AX_SYNC_FLAG_COPYTYPE      (1 << 3)
#define AX_SYNC_FLAG_COPYSTATE     (1 << 2)
#define AX_SYNC_FLAG_COPYMXRCTRL   (1 << 1)
#define AX_SYNC_FLAG_COPYSELECT    (1 << 0)

#define AX_PRIORITY_STACKS 32

//  state
#define AX_PB_STATE_STOP        0x0000
#define AX_PB_STATE_RUN         0x0001

//  type
#define AX_PB_TYPE_NORMAL       0x0000
#define AX_PB_TYPE_STREAM       0x0001  // no loop context programming for ADPCM

//  format
#define AX_PB_FORMAT_PCM16      0x000A  // signed 16 bit PCM mono
#define AX_PB_FORMAT_PCM8       0x0019  // signed 8 bit PCM mono
#define AX_PB_FORMAT_ADPCM      0x0000  // ADPCM encoded (both standard & extended)

//  src select
#define AX_PB_SRCSEL_POLYPHASE  0x0000  // N64 type polyphase filter (4-tap)
#define AX_PB_SRCSEL_LINEAR     0x0001  // Linear interpolator
#define AX_PB_SRCSEL_NONE       0x0002  // No SRC (1:1)

//  coef select
#define AX_PB_COEFSEL_8KHZ      0x0000  // 8KHz low pass response
#define AX_PB_COEFSEL_12KHZ     0x0001  // 12.8KHz N64 type response
#define AX_PB_COEFSEL_16KHZ     0x0002  // 16KHz response

//  mixer ctrl
#define AX_PB_MIXCTRL_L         0x0001  // main left mix
#define AX_PB_MIXCTRL_R         0x0002  // main right mix
#define AX_PB_MIXCTRL_S         0x0004  // main surround mix
#define AX_PB_MIXCTRL_RAMP      0x0008  // main bus ramp (applies to LRS)

#define AX_PB_MIXCTRL_A_L       0x0010  // AuxA left mix
#define AX_PB_MIXCTRL_A_R       0x0020  // AuxA rigth mix
#define AX_PB_MIXCTRL_A_LR_RAMP 0x0040  // AuxA bus ramp (applies to LR only)
#define AX_PB_MIXCTRL_A_S       0x0080  // AuxA surround mix
#define AX_PB_MIXCTRL_A_S_RAMP  0x0100  // AuxA bus ramp (applies to S only)

#define AX_PB_MIXCTRL_B_L       0x0200  // AuxB left mix
#define AX_PB_MIXCTRL_B_R       0x0400  // AuxB right mix
#define AX_PB_MIXCTRL_B_LR_RAMP 0x0800  // AuxB bus ramp (applies to LR only)
#define AX_PB_MIXCTRL_B_S       0x1000  // AuxB surround mix
#define AX_PB_MIXCTRL_B_S_RAMP  0x2000  // AuxB bus ramp (applies to S only)

#define AX_PB_MIXCTRL_B_DPL2    0x4000  // AuxB DPL2, does not apply ITD for surrounds

//  IIR filter switch
#define AX_PB_LPF_OFF           0x0000  // IIR filter switch
#define AX_PB_LPF_ON            0x0001


#define AX_PB_NEXTHI_OFF        0
#define AX_PB_NEXTLO_OFF        1
#define AX_PB_CURRHI_OFF        2
#define AX_PB_CURRLO_OFF        3

#define AX_PB_SRCSELECT_OFF     4
#define AX_PB_COEFSELECT_OFF    5
#define AX_PB_MIXERCTRL_OFF     6

#define AX_PB_STATE_OFF         7
#define AX_PB_TYPE_OFF          8

#define AXPBMIX_OFF             9

#define AXPBITD_OFF           (AXPBMIX_OFF      + AXPBMIX_SIZE)
#define AXPBUPDATE_OFF        (AXPBITD_OFF      + AXPBITD_SIZE)
#define AXPBDPOP_OFF          (AXPBUPDATE_OFF   + AXPBUPDATE_SIZE)
#define AXPBVE_OFF            (AXPBDPOP_OFF     + AXPBDPOP_SIZE)
#define AXPBFIR_OFF           (AXPBVE_OFF       + AXPBVE_SIZE)
#define AXPBADDR_OFF          (AXPBFIR_OFF      + AXPBFIR_SIZE)
#define AXPBADPCM_OFF         (AXPBADDR_OFF     + AXPBADDR_SIZE)
#define AXPBSRC_OFF           (AXPBADPCM_OFF    + AXPBADPCM_SIZE)
#define AXPBADPCMLOOP_OFF     (AXPBSRC_OFF      + AXPBSRC_SIZE)

#define AX_PB_SIZE            (AXPBADPCMLOOP_OFF+AXPBADPCMLOOP_SIZE)

#define AX_DSP_PATCHDATA_SIZE  128

#define AXPBADDR_LOOP_OFF     0           // States for loopFlag field
#define AXPBADDR_LOOP_ON      1


#define AXPBADDR_LOOPFLAG_OFF         AXPBADDR_OFF
#define AXPBADDR_FORMAT_OFF           (AXPBADDR_OFF+1)
#define AXPBADDR_LOOPADDRESSHI_OFF    (AXPBADDR_OFF+2)
#define AXPBADDR_LOOPADDRESSLO_OFF    (AXPBADDR_OFF+3)
#define AXPBADDR_ENDADDRESSHI_OFF     (AXPBADDR_OFF+4)
#define AXPBADDR_ENDADDRESSLO_OFF     (AXPBADDR_OFF+5)
#define AXPBADDR_CURRENTADDRESSHI_OFF (AXPBADDR_OFF+6)
#define AXPBADDR_CURRENTADDRESSLO_OFF (AXPBADDR_OFF+7)
#define AXPBADDR_SIZE                 8

// AX
void AXInit(void);
void AXInitEx(u32 outputBufferMode);
void AXQuit(void);

// AXAlloc
void AXFreeVoice(AXVPB* p);
AXVPB* AXAcquireVoice(u32 priority, void (*callback)(void *), u32 userContext);
void AXSetVoicePriority(AXVPB* p, u32 priority);

// AXAux
void AXRegisterAuxACallback(void (*callback)(void*, void*), void* context);
void AXRegisterAuxBCallback(void (*callback)(void*, void*), void* context);

// AXCL
void AXSetMode(u32 mode);
u32 AXGetMode(void);

// AXOut
extern AXPROFILE __AXLocalProfile;

void AXSetStepMode(u32 i);
AXCallback AXRegisterCallback(AXCallback callback);

// AXProf
void AXInitProfile(AXPROFILE* profile, u32 maxProfiles);
u32 AXGetProfile(void);

// AXVPB
void AXSetVoiceSrcType(AXVPB* p, u32 type);
void AXSetVoiceState(AXVPB* p, u16 state);
void AXSetVoiceType(AXVPB* p, u16 type);
void AXSetVoiceMix(AXVPB* p, AXPBMIX* mix);
void AXSetVoiceItdOn(AXVPB* p);
void AXSetVoiceItdTarget(AXVPB* p, u16 lShift, u16 rShift);
void AXSetVoiceUpdateIncrement(AXVPB* p);
void AXSetVoiceUpdateWrite(AXVPB* p, u16 param, u16 data);
void AXSetVoiceDpop(AXVPB* p, AXPBDPOP* dpop);
void AXSetVoiceVe(AXVPB* p, AXPBVE* ve);
void AXSetVoiceVeDelta(AXVPB* p, s16 delta);
void AXSetVoiceFir(AXVPB* p, AXPBFIR* fir);
void AXSetVoiceAddr(AXVPB* p, AXPBADDR* addr);
void AXSetVoiceLoop(AXVPB* p, u16 loop);
void AXSetVoiceLoopAddr(AXVPB* p, u32 addr);
void AXSetVoiceEndAddr(AXVPB* p, u32 addr);
void AXSetVoiceCurrentAddr(AXVPB* p, u32 addr);
void AXSetVoiceAdpcm(AXVPB* p, AXPBADPCM* adpcm);
void AXSetVoiceSrc(AXVPB* p, AXPBSRC* src_);
void AXSetVoiceSrcRatio(AXVPB* p, f32 ratio);
void AXSetVoiceAdpcmLoop(AXVPB* p, AXPBADPCMLOOP* adpcmloop);
void AXSetMaxDspCycles(u32 cycles);
u32 AXGetMaxDspCycles(void);
u32 AXGetDspCycles(void);
void AXSetVoiceLpf(AXVPB* p, AXPBLPF* lpf);
void AXSetVoiceLpfCoefs(AXVPB* p, u16 a0, u16 b0);
void AXGetLpfCoefs(u16 freq, u16* a0, u16* b0);
void AXSetCompressor(u32);

// DSPCode
extern u16 axDspSlaveLength;
extern u16 axDspSlave[AX_DSP_SLAVE_LENGTH];

#ifdef __cplusplus
}
#endif

#endif // _DOLPHIN_AX_H_
