/*
 * kernel.c
 *
 *  Created on: Nov 9, 2023
 *      Author: zsyed
 */
#include <stdio.h>
#include "main.h"
#include "kernel.h"
#define RUN_FIRST_THREAD 3
#define YIELD 4
#define STACK_SIZE 0x400

/* --------------- GLOBAL VARIABLES -----------------*/
uint32_t* MSP_INIT_VAL;
uint32_t* current_stack; //location of last stack
uint32_t* stackptr;
thread mythread;
thread threads[15];
int nThreads = 0;
int curThreadIndx = 1;

/* -------------- System Calls -------------- */
void system_call3()
{
	__asm("SVC #3");
}
void system_call4()
{
	__asm("SVC #4");
}

void osKernelInitialize() {
	MSP_INIT_VAL = *(uint32_t**)0x0;
	//set the priority of PendSV to almost the weakest
	SHPR3 |= 0xFE << 16; //shift the constant 0xFE 16 bits to set PendSV priority
	SHPR2 |= 0xFDU << 24; //Set the priority of SVC higher than PendSV
	current_stack = MSP_INIT_VAL - 0x400;
}

void osKernelStart() {
	system_call3();
}

bool osCreateThread(void* fnc_ptr) {
	uint32_t* new_sp = allocateStack();
	if (new_sp == NULL) {
		return false;
	}
	else {
		*(--new_sp) = 1<<24; //A magic number, this is xPSR
		*(--new_sp) = (uint32_t)fnc_ptr; //the thread function
		for (int i = 0; i < 14; i++)
		{
		  *(--new_sp) = 0xA;
		}
	}
	mythread.sp = new_sp;
	mythread.thread_function = fnc_ptr;
	threads[nThreads] = mythread;
	return true;
}

uint32_t* allocateStack() {
	nThreads++;
	if (current_stack > MSP_INIT_VAL - (0x4000 - nThreads*STACK_SIZE)) {
		current_stack = current_stack - nThreads*STACK_SIZE;
	}
	else {
		current_stack = NULL;
	}
	return current_stack;
}

void SVC_Handler_Main( unsigned int *svc_args )
{
	unsigned int svc_number;
	svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
	switch( svc_number )
	{
	case RUN_FIRST_THREAD:
		__set_PSP((uint32_t)threads[curThreadIndx].sp);
		runFirstThread();
		break;
	case YIELD:
		//Pend an interrupt to do the context switch
		_ICSR |= 1<<28;
		__asm("isb");
		break;
	default: /* unknown SVC */
		break;
	}
}

void osSched()
{
	threads[curThreadIndx].sp = (uint32_t*)(__get_PSP() - 8*4);
	curThreadIndx = (curThreadIndx+1)%nThreads;
	__set_PSP((uint32_t)threads[curThreadIndx].sp);
	return;
}

void osYield(void)
{
	system_call4();
}
