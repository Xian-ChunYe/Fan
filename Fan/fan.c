#include "fan.h"
#include "usart_app.h"

//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM2_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设							 
}

u16 FANCnt[3] = {0};

void FANCheck(void)
{
  u16 fanstate = 0;	 
  u8 i;
  fanstate |= (GPIO_ReadInputData(GPIOB))&0x07;
	
  for(i = 0;i < 3; i ++)
  {
	if((fanstate&0x01) == 0x01)
	{
	  if(FANCnt[i] < 30)
      FANCnt[i] ++;
	}
	else FANCnt[i] = 0;
	
	fanstate >>= 1;
	
	if(FANCnt[i] >= 30)
	{
	   Boards_Info.FANState[i] = 1;
	}
	else Boards_Info.FANState[i] = 0;
  }
}

//extern u32 pwm_num;
//定时器2中断服务程序
void TIM2_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
        FANCheck();
    }
}


void Fan_Init()
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB,PE端口时钟
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;	//LED0-->PB.4 端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	 TIM2_Int_Init(500-1,720-1);
}


