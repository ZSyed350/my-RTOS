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
#define STACK_SIZE 0x400
#define MAX_THREADS_ALLOWED 3 // FIXME: should be the maximum amount of threads there is stack space for

/* --------------- GLOBAL VARIABLES -----------------*/
uint32_t* MSP_INIT_VAL;
uint32_t* current_stack; //location of last stack
uint32_t* stackptr;
//thread thread1;

thread threads[MAX_THREADS_ALLOWED];
int nThreadsRunning = 0;
int curThreadIndx;

extern void osKernelInitialize() {
	MSP_INIT_VAL = *(uint32_t**)0x0;
	//set the priority of PendSV to almost the weakest
	SHPR3 |= 0xFE << 16; //shift the constant 0xFE 16 bits to set PendSV priority
	SHPR2 |= 0xFDU << 24; //Set the priority of SVC higher than PendSV
	current_stack = MSP_INIT_VAL;
	curThreadIndx = 0;
}

extern void osKernelStart() {
	__asm("SVC #3");
}

extern bool osCreateThread(void *ptr) {
	stackptr = allocateStack();
	if (stackptr == NULL) {
		return false;
	}
	*(--stackptr) = 1<<24; //A magic number, this is xPSR
	*(--stackptr) = (uint32_t)ptr; //the thread function
	for (int i = 0; i < 14; i++)
	{
	  *(--stackptr) = 0xA;
	}
	threads[curThreadIndx].sp = stackptr;
	threads[curThreadIndx].thread_function = ptr;
	return true;
}

uint32_t* allocateStack() {
	uint32_t* sptr;
	if (current_stack > MSP_INIT_VAL - (0x4000 - STACK_SIZE)) {
		sptr = current_stack - STACK_SIZE;
	}
	else {
		sptr = NULL;
	}
	return sptr;
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
	default: /* unknown SVC */
		break;
	}
}

void osSched()
{
	threads[curThreadIndx].sp = (uint32_t*)(__get_PSP() - 8*4);
	curThreadIndx = (curThreadIndx+1)%nThreadsRunning;
	__set_PSP((uint32_t)threads[curThreadIndx].sp);
}
