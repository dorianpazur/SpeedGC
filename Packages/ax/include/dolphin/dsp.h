#ifndef _DOLPHIN_DSP_H_
#define _DOLPHIN_DSP_H_

#include <dolphin/os.h>
#include <gccore.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DSP_TASK_FLAG_CLEARALL  0x00000000  // zero all flags
#define DSP_TASK_FLAG_ATTACHED  0x00000001  // Indicates task is in the list. Asserted by DSPAddTask(). Cleared by __DSP_remove_task().
#define DSP_TASK_FLAG_CANCEL    0x00000002  // Asserted by DSPCancelTask(). Marks a task for cancellation/removal.

#define DSP_TASK_STATE_INIT     0           // task has never been run before    
#define DSP_TASK_STATE_RUN      1           // task is running
#define DSP_TASK_STATE_YIELD    2           // task has yielded control
#define DSP_TASK_STATE_DONE     3           // task is finished and has been culled from list

typedef struct STRUCT_DSP_TASK DSPTaskInfo;

typedef struct STRUCT_DSP_TASK {                                   
    /* 0x00 */ volatile u32 state;
    /* 0x04 */ volatile u32 priority;
    /* 0x08 */ volatile u32 flags;
    /* 0x0C */ u16* iram_mmem_addr;
    /* 0x10 */ u32 iram_length;
    /* 0x14 */ u32 iram_addr;
    /* 0x18 */ u16* dram_mmem_addr;
    /* 0x1C */ u32 dram_length;
    /* 0x20 */ u32 dram_addr;
    /* 0x24 */ u16 dsp_init_vector;
    /* 0x26 */ u16 dsp_resume_vector;
    /* 0x28 */ DSPCallback init_cb;
    /* 0x2C */ DSPCallback res_cb;
    /* 0x30 */ DSPCallback done_cb;
    /* 0x34 */ DSPCallback req_cb;
    /* 0x38 */ DSPTaskInfo* next;
    /* 0x3C */ DSPTaskInfo* prev;
    ///* 0x40 */ OSTime t_context;
    ///* 0x48 */ OSTime t_task;
} DSPTaskInfo;

#define DSPCheckMailToDSP DSP_CheckMailTo
#define DSPCheckMailFromDSP DSP_CheckMailFrom
#define DSPReadCPUToDSPMbox DSP_ReadCPUtoDSP
#define DSPReadMailFromDSP DSP_ReadMailFrom
#define DSPSendMailToDSP DSP_SendMailTo
#define DSPAssertInt DSP_AssertInt
#define DSPInit DSP_Init
inline BOOL DSPCheckInit() { return false; }; // libogc checks internally
#define DSPReset DSP_Reset
#define DSPHalt DSP_Halt
#define DSPUnhalt DSP_Unhalt
#define DSPGetDMAStatus DSP_GetDMAStatus
DSPTaskInfo* DSPAddTask(DSPTaskInfo* task);
DSPTaskInfo* DSPCancelTask(DSPTaskInfo* task);
DSPTaskInfo* DSPAssertTask(DSPTaskInfo* task);

#ifdef __cplusplus
}
#endif

#endif