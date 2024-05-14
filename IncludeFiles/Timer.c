#include "timer.h"

void Timer_Init(uint16_t topValue) {
    TCB0.CTRLA &= ~TCB_ENABLE_bm;
    TCB0.CCMP = topValue;
    TCB0.INTCTRL = TCB_CAPT_bm;
}

void Timer_Reset(void) {
    Timer_Stop();
    TCB0.CNT = 0;
    Timer_Start();
}

void Timer_Start(void) {
    TCB0.CTRLA |= TCB_ENABLE_bm;
}

void Timer_Stop(void) {
    TCB0.CTRLA &= ~TCB_ENABLE_bm;
}