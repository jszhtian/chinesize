#pragma once
#include "ilhook.h"
#include <stdint.h>

void HOOKFUNC MyOpenFile(Registers* regs);
uint32_t HOOKFUNC MyReadInst(Registers* regs, uint32_t old_proc_func);
void HOOKFUNC MyMbtowc(Registers* regs);
void InitWs2();
void HOOKFUNC MySelString(Registers* regs);
void HOOKFUNC MyOpenFile1(wchar_t* arcName, wchar_t* entName);
void HOOKFUNC MyChangeFont(Registers* regs);
