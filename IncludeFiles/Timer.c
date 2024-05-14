#include "timer.h"

void timerInit(uint16_t topValue) {
    TCB0.CTRLA &= ~TCB_ENABLE_bm;
    TCB0.CCMP = topValue;
    TCB0.INTCTRL = TCB_CAPT_bm;
}

void timerReset(void) {
    TCB0.CTRLA &= ~TCB_ENABLE_bm;
    TCB0.CNT = 0;
    TCB0.CTRLA |= TCB_ENABLE_bm;
}

void timerStart(void) {
    TCB0.CTRLA |= TCB_ENABLE_bm;
}

void timerStop(void) {
    TCB0.CTRLA &= ~TCB_ENABLE_bm;
}