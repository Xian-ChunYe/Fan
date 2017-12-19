#ifndef __FAN_H
#define __FAN_H	 
#include "board_config.h"

extern void stop_device_error(u8 err_type);

void Fan_Init(void);
void TIM2_Int_Init(u16 arr,u16 psc);
		 				    
#endif



