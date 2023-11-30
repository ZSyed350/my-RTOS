/*
 * kernel.h
 *
 *  Created on: Nov 9, 2023
 *      Author: zsyed
 */

#ifndef INC_KERNEL_H_
#define INC_KERNEL_H
#define SHPR2 *(uint32_t*)0xE000ED1C //for setting SVC priority, bits 31-24
#define SHPR3 *(uint32_t*)0xE000ED20 // PendSV is bits 23-16
#define _ICSR *(uint32_t*)0xE000ED04 //This lets us trigger PendSV_
#include <stdbool.h>

typedef struct k_thread{
	uint32_t* sp; //stack pointer
	void (*thread_function)(void*); //function pointer
}thread;

uint32_t* allocateStack();
extern void osKernelInitialize();
extern void osKernelStart();
extern bool osCreateThread(void* fnc_ptr);
extern void runFirstThread(void);
extern void osSched();
extern void osYield(void);
void SVC_Handler_Main( unsigned int *svc_args );

#endif /* INC_KERNEL_H_ */
