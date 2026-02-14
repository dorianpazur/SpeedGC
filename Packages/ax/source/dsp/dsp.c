#include <stddef.h>
#include <dolphin/os.h>
#include <dolphin/hw_regs.h>
#include <dolphin/dsp.h>

DSPTaskInfo* DSPAddTask(DSPTaskInfo* task) {
    return (DSPTaskInfo*)DSP_AddTask((dsptask_t*)task);
}

DSPTaskInfo* DSPCancelTask(DSPTaskInfo* task) {
	DSP_CancelTask((dsptask_t*)task);
    return task;
}

DSPTaskInfo* DSPAssertTask(DSPTaskInfo* task) {
    return (DSPTaskInfo*)DSP_AssertTask((dsptask_t*)task);
}
