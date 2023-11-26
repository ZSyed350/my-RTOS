/*
 * kernel.h
 *
 *  Created on: Nov 9, 2023
 *      Author: zsyed
 */

#ifndef INC_KERNEL_H_
#define INC_KERNEL_H_
#include <stdbool.h>

typedef struct k_thread{
	uint32_t* sp; //stack pointer
	void (*thread_function)(void*); //function pointer
}thread;

uint32_t* allocateStack();
extern void osKernelInitialize();
extern void osKernelStart();
extern bool osCreateThread(void *ptr);
extern void runFirstThread(void);
void SVC_Handler_Main( unsigned int *svc_args );

#endif /* INC_KERNEL_H_ */
