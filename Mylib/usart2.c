#include "main.h"
//ok

/*-----USART2_RX-----PA3----*/ 
//for D-BUS

unsigned char sbus_rx_buffer[25];
RC_Ctl_t RC_Ctl;

void USART2_Configuration(void)
{
    USART_InitTypeDef usart2;
		GPIO_InitTypeDef  gpio;
    NVIC_InitTypeDef  nvic;
    DMA_InitTypeDef   dma;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_DMA1,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource3 ,GPIO_AF_USART2);
	
		gpio.GPIO_Pin = GPIO_Pin_3 ;
		gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOA,&gpio);
    
    USART_DeInit(USART2);
		usart2.USART_BaudRate = 100000;   //SBUS 100K baudrate
		usart2.USART_WordLength = USART_WordLength_8b;
		usart2.USART_StopBits = USART_StopBits_1;
		usart2.USART_Parity = USART_Parity_Even;
		usart2.USART_Mode = USART_Mode_Rx;
    usart2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2,&usart2);
    
	USART_Cmd(USART2,ENABLE);
    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
    
    nvic.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    DMA_DeInit(DMA1_Stream5);
    dma.DMA_Channel= DMA_Channel_4;
    dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
    dma.DMA_Memory0BaseAddr = (uint32_t)sbus_rx_buffer;
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize = 18;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_Priority = DMA_Priority_VeryHigh;
    dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
    dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    dma.DMA_MemoryBurst = DMA_Mode_Normal;
    dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5,&dma);

    DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);
    DMA_Cmd(DMA1_Stream5,ENABLE);
}

CanTxMsg TxMessage;//CAN总线发出数据

void DMA1_Stream5_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5))
    {
				LED_RED_ON();
        DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);
        DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
			
			//*******************************************************************************************//
			//**********************通道0~3最大值1684；中间值1024；最小值364*****************************//
			//**********************S1、S2值为:   1上；  2下；  3中**************************************//
			//*******************************************************************************************//
				RC_Ctl.rc.ch0 = (sbus_rx_buffer[0]| (sbus_rx_buffer[1] << 8)) & 0x07ff; //!< Channel 0
				RC_Ctl.rc.ch1 = ((sbus_rx_buffer[1] >> 3) | (sbus_rx_buffer[2] << 5)) & 0x07ff; //!< Channel 1
				RC_Ctl.rc.ch2 = ((sbus_rx_buffer[2] >> 6) | (sbus_rx_buffer[3] << 2) | //!< Channel 2
				(sbus_rx_buffer[4] << 10)) & 0x07ff;
				RC_Ctl.rc.ch3 = ((sbus_rx_buffer[4] >> 1) | (sbus_rx_buffer[5] << 7)) & 0x07ff; //!< Channel 3
				RC_Ctl.rc.s1 = ((sbus_rx_buffer[5] >> 4)& 0x000C) >> 2; //!< Switch left
				RC_Ctl.rc.s2 = ((sbus_rx_buffer[5] >> 4)& 0x0003); //!< Switch right
				RC_Ctl.mouse.x = sbus_rx_buffer[6] | (sbus_rx_buffer[7] << 8); //!< Mouse X axis
				RC_Ctl.mouse.y = sbus_rx_buffer[8] | (sbus_rx_buffer[9] << 8); //!< Mouse Y axis
				RC_Ctl.mouse.z = sbus_rx_buffer[10] | (sbus_rx_buffer[11] << 8); //!< Mouse Z axis
				RC_Ctl.mouse.press_l = sbus_rx_buffer[12]; //!< Mouse Left Is Press ?
				RC_Ctl.mouse.press_r = sbus_rx_buffer[13]; //!< Mouse Right Is Press ?
				RC_Ctl.key.v = sbus_rx_buffer[14] | (sbus_rx_buffer[15] << 8); //!< KeyBoard value

//通过遥控器调节PID参数
						Current_1_Pid.pGain +=(float)( RC_Ctl.rc.ch0-1024)*0.00001f;
            if(Current_1_Pid.pGain<0) Current_1_Pid.pGain=0;

            Current_1_Pid.iGain +=(float)( RC_Ctl.rc.ch1-1024)*0.00001f;
            if(Current_1_Pid.iGain<0) Current_1_Pid.iGain=0;

//遥控器设定底盘电机转速
						if(RC_Ctl.rc.s1==1)
						{
							SetSpeed_1=6000;
						}
						else
						{
							SetSpeed_1=0;
						}
//						SetSpeed_1=(RC_Ctl.rc.ch3-1024)*14.545;
			
//向云台发送四个通道信息
				TxMessage.StdId=0x402;//标准标识符为0x402
				TxMessage.RTR=CAN_RTR_DATA;;//消息类型为数据帧，一帧8位
				TxMessage.IDE=CAN_Id_Standard;//不使用扩展标识符
				TxMessage.DLC=8;//发送8字节信息
				
				TxMessage.Data[0]=RC_Ctl .rc.ch0 & 0xff;
        TxMessage.Data[1]=RC_Ctl .rc.ch0>>8;

        TxMessage.Data[2]=RC_Ctl .rc.ch1 & 0xff;
        TxMessage.Data[3]=RC_Ctl .rc.ch1>>8;

        TxMessage.Data[4]=RC_Ctl .rc.ch2 & 0xff;
        TxMessage.Data[5]=RC_Ctl .rc.ch2>>8;

        TxMessage.Data[6]=RC_Ctl .rc.ch3 & 0xff;
        TxMessage.Data[7]=RC_Ctl .rc.ch3>>8;
				
				CAN_Transmit(CAN2,&TxMessage);
	//向云台发送鼠标速度和S1 S2值
        TxMessage.StdId=0x403;			// 标准标识符为0x403
        TxMessage.RTR=CAN_RTR_DATA;		// 消息类型为数据帧，一帧8位
        TxMessage.IDE=CAN_Id_Standard;	// 不使用扩展标识符
        TxMessage.DLC=8;				// 发送8字节信息

        TxMessage.Data[0]=RC_Ctl.mouse.x & 0xff;
        TxMessage.Data[1]=RC_Ctl.mouse.x>>8;

        TxMessage.Data[2]=RC_Ctl.mouse.y & 0xff;
        TxMessage.Data[3]=RC_Ctl.mouse.y>>8;

        TxMessage.Data[4]=RC_Ctl.mouse.press_l;
        TxMessage.Data[5]=RC_Ctl.mouse.press_r;

        TxMessage.Data[6]=RC_Ctl.rc.s1;
        TxMessage.Data[7]=RC_Ctl.rc.s2;

        CAN_Transmit(CAN2, &TxMessage);

//向云台发送鼠标速度和S1 S2值
        TxMessage.StdId=0x404;			// 标准标识符为0x404
        TxMessage.RTR=CAN_RTR_DATA;		// 消息类型为数据帧，一帧8位
        TxMessage.IDE=CAN_Id_Standard;	// 不使用扩展标识符
        TxMessage.DLC=8;				// 发送8字节信息

        TxMessage.Data[0]=RC_Ctl.key.v & 0xff;
        TxMessage.Data[1]=RC_Ctl.key.v>>8;
				
        CAN_Transmit(CAN2, &TxMessage);
		}
}
