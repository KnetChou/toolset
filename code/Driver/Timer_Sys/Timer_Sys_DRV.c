/**  @file	Timer_Sys_DRV.c
*    @brief    Driver SysTick
*    @version  1.0
*    @copyright
*    @author    Kent Chou
*	 @see 
* 		Project Name :	
* 		CpuType		:  ST32F030C8
* 		OSC      	:  External OSC 8MHz
*		Compiler 	:  Keil C
* 		IDE      	:  uVision5
* 		Create Date :  2017/07/31
*/

/*****************************************************************************/
/* Included header files                                                     */
/*****************************************************************************/
#include "Timer_Sys_DRV.h"


/*****************************************************************************/
/* Private types                                                             */
/*****************************************************************************/


/*****************************************************************************/
/* Declare Private functions                                               	 */
/*****************************************************************************/


/*****************************************************************************/
/* Private constants						                           	 */
/*****************************************************************************/

/* DRV_SYSTICK_RELOAD_VALUE Set the following conditions:
			SYSCKL = SYSCLK_FREQ_72MHz ,
			AHB Prescaler = SYSCLK (1:1) ,
			Clock Source /8(division) = Enable.
	1mS Formula : (SYSCKL/AHB Precaler/Clock Source)*1mS
*/
#define	DRV_SYSTICK_RELOAD_VALUE	((DRV_TIMER_SYSCLK/DRV_TIMER_AHB_Prescaler/8)/1000)
#define	const_T10mS			10		
#define const_T100mS		100
#define	const_T1000mS		1000

/*****************************************************************************/
/* Private macros                                                            */
/*****************************************************************************/


/*****************************************************************************/
/* Private variables     						                         */
/*****************************************************************************/
static u8	bT10msDEC=0, bT100msDEC=0 ;
static u16 	wT1000msDEC=0 ;


/*****************************************************************************/
/* Private variables & macros					                         */
/*****************************************************************************/
static 	BITS_BYTE 	bbTimerFlag ;
#define fTimer1mS			bbTimerFlag.bits.bit0
#define fTimer10mS			bbTimerFlag.bits.bit1
#define fTimer100mS			bbTimerFlag.bits.bit2
#define fTimer1Sec			bbTimerFlag.bits.bit3
#define fRTC_Alarm			bbTimerFlag.bits.bit4
#define fTimer1min			bbTimerFlag.bits.bit5


/*****************************************************************************/
/* functions                                                                 */
/*****************************************************************************/

/** @brief 
*   @param None.
*   @return 
*   @see 
*/
bool is_delay_hour_occured(void)
{
	if(fRTC_Alarm == FALSE)
	{
		return (FALSE) ;
	}
	fRTC_Alarm = FALSE ;
	return (TRUE) ;	
}

/** @brief 
*   @param None.
*   @return None. 
*   @see 
*/
void RTC_alarm_ISR(void)
{
	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
	{
		fRTC_Alarm = TRUE ;
		RTC_ClearITPendingBit(RTC_IT_ALRA) ;
		RTC_ITConfig(RTC_IT_ALRA, DISABLE) ;
		RTC_AlarmCmd(RTC_Alarm_A, DISABLE) ;
	}
	/* Clear the EXTIL line 17 */
	EXTI_ClearITPendingBit(EXTI_Line17);
 }

/** @brief 
*   @param None.
*   @return None. 
*   @see 
*/
void set_delay_one_hour(void)
{
	RTC_AlarmTypeDef RTC_AlarmStructure ;
	RTC_TimeTypeDef RTC_Current_Time ;

	RTC_GetTime(RTC_Format_BIN, &RTC_Current_Time) ;

	#if 1
	if(++RTC_Current_Time.RTC_Hours > 23)
	{
		RTC_Current_Time.RTC_Hours %= 24 ;  /* RTC_HourFormat_24 */ 
		if(RTC_Current_Time.RTC_H12 == RTC_H12_AM)
		{
			RTC_Current_Time.RTC_H12 = RTC_H12_PM ;
		}
		else
		{
			RTC_Current_Time.RTC_H12 = RTC_H12_AM ;
		}
	}
	#else
	// 測試用
	RTC_Current_Time.RTC_Seconds += 5 ;
	if(RTC_Current_Time.RTC_Seconds > 59)
	{
		RTC_Current_Time.RTC_Seconds %= 60 ;
		RTC_Current_Time.RTC_Minutes++ ;
	}
	#endif
	
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_Current_Time.RTC_H12 ;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = RTC_Current_Time.RTC_Hours ;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = RTC_Current_Time.RTC_Minutes ;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = RTC_Current_Time.RTC_Seconds ;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay ;
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x01 ;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay ;
	
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure) ;

	/* Enable AlarmA interrupt */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	/* Enable the alarm */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

	fRTC_Alarm = FALSE ;
}

/** @brief 
*   @param None.
*   @return None. 
*   @see 
*/
void timer_14_ISR(void)
{
	if(TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM14, TIM_IT_Update) ;
		fTimer1min = TRUE ;
	}
}

/** @brief 
*   @param None.
*   @return None. 
*   @see 
*		1分鐘計時器.
*/
void timer_14_init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* TIM clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

  /* Enable the TIM gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 3 ;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)(SystemCoreClock  / 1000) - 1;
  TIM_TimeBaseStructure.TIM_Period = 60000 ;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0 ;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

  /* TIM Interrupts enable */
  TIM_ClearITPendingBit(TIM14, TIM_IT_Update) ;
  TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);

  /* TIM enable counter */
  TIM_Cmd(TIM14, ENABLE);

}

/** @brief 
*   @param None.
*   @return None. 
*   @see 
*/
void RTC_LSI_init(void)
{
	RTC_InitTypeDef RTC_InitStructure ;
	RTC_TimeTypeDef  RTC_TimeStruct;
	NVIC_InitTypeDef NVIC_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	
	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* Reset RTC Domain */
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

	/* Enable the LSE OSC */
	RCC_LSICmd(ENABLE);

	/* Wait till LSE is ready */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) ;

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	
	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

	/* Configure the RTC data register and RTC prescaler */
	/* ck_spre(1Hz) = RTCCLK(LSI) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
	RTC_InitStructure.RTC_AsynchPrediv = 99 ;
	RTC_InitStructure.RTC_SynchPrediv  = 399 ;
	RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24 ;
	RTC_Init(&RTC_InitStructure);

	/* Set the time to 00h 00mn 00s AM */
	RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours   = 0x00;
	RTC_TimeStruct.RTC_Minutes = 0x00;
	RTC_TimeStruct.RTC_Seconds = 0x00;  
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);	

	/* EXTI configuration */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the RTC Wakeup Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1 ;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/** @brief Timer_Is_1min_Flage
*   @param None.
*   @return Timer_1min_Flage status. 
*   @see 
*/
 bool is_1min_timer (void) 
{
	if(fTimer1min == FALSE)
	{
		return (FALSE) ;
	}
	fTimer1min = FALSE ;
	return (TRUE) ;
}

/** @brief Timer_Is_100mS_Flage
*   @param None.
*   @return Timer_100mS_Flage status. 
*   @see 
*/
 bool is_1sec_timer (void) 
{
	if(fTimer1Sec == FALSE)
	{
		return (FALSE) ;
	}
	fTimer1Sec = FALSE ;
	return (TRUE) ;		
}

/** @brief Timer_Is_100mS_Flage
*   @param None.
*   @return Timer_100mS_Flage status. 
*   @see 
*/
 bool is_100ms_timer (void) 
{
	if(fTimer100mS == FALSE)
	{
		return (FALSE) ;
	}
	fTimer100mS = FALSE ;
	return (TRUE) ;		
}

/** @brief Timer_Is_10mS_Flage
*   @param None.
*   @return Timer_10mS_Flage status. 
*   @see 
*/
 bool is_10ms_timer (void) 
{
	if(fTimer10mS == FALSE)
	{
		return (FALSE) ;
	}
	fTimer10mS = FALSE ;
	return (TRUE) ;	
}

/** @brief Timer_Is_1mS_Flage
*   @param None.
*   @return Timer_1mS_Flage status. 
*   @see 
*/
 bool is_1ms_timer (void) 
{    
    if(fTimer1mS == FALSE)
	{
		return (FALSE) ;
	}
	fTimer1mS = FALSE ;
    
	return (TRUE) ;
}

/** @brief Timer_Sys_Disable
*   @param None.
*   @return None. 
*   @see SysTick_Type.
*/
void timer_sys_disable (void)
{
	SysTick->CTRL &= (~(SysTick_CTRL_ENABLE_Msk)) ;	
	SysTick->VAL = 0 ;
	bbTimerFlag.byte = 0 ;
	bT10msDEC = 0 ; 
	bT100msDEC = 0 ;
	wT1000msDEC = 0 ;
}

/** @brief Timer_Sys_Enable
*   @param None.
*   @return None. 
*   @see SysTick_Type.
*/
void timer_sys_enable (void)
{
	bT10msDEC = const_T10mS ;
	bT100msDEC = const_T100mS ;
	wT1000msDEC = const_T1000mS ;
	bbTimerFlag.byte = 0 ;
	
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk) ;	
}

/** @brief timer_sys_init Initialize.
*   @param None.
*   @return None. 
*   @see   Set the following conditions:
				SYSCKL = SYSCLK_FREQ_48MHz ,
				AHB Prescaler = SYSCLK ,
*/
void timer_sys_init (void)
{	
    #if 0
	SysTick_Type TimerSysTick ;
    #endif
	SysTick->LOAD = DRV_SYSTICK_RELOAD_VALUE ;
	SysTick->VAL = 0 ;
	SysTick->CTRL = (u32)((1<<1)| 	/* TICKINT
					 	  			   SysTick exception request enable 
									   0: not assert the SysTick exception request
					 	  			   1: asserts the SysTick exception request.
					 	  			 */ 
					 	  (0<<2)  ); /* CLKSOURCE
									   Clock source selection  
									   0: AHB/8
									   1: Processor clock (AHB)	
									   */
    #if 0
    /*** RTC config ***/
    RTC_LSI_init() ;        /* 小時計時. */
    timer_14_init() ;       /* 分鐘計時. */
    #endif
    
}

/** 
*	@brief Timer_Systick Interrupt
*   	@param None.
*   	@return None. 
*   	@see   
*/
void timer_sys_ISR (void) 
{
	fTimer1mS = TRUE ;

	bT10msDEC-- ;
	if(bT10msDEC == 0)
	{
		fTimer10mS = TRUE ;
		bT10msDEC = const_T10mS ;
	}

	bT100msDEC-- ;
	if(bT100msDEC == 0)
	{
		fTimer100mS = TRUE ;
		bT100msDEC = const_T100mS ;
	}

	wT1000msDEC-- ;
	if(wT1000msDEC == 0)
	{
		fTimer1Sec = TRUE ;
		wT1000msDEC = const_T1000mS ;
	}

}

/** 
*	@brief 
*   	@param None.
*   	@return None. 
*   	@see   
*/
void do_1min_timer_calibration(void)
{
	u8 bTemp = 10 ;
	TIM_Cmd(TIM14, DISABLE) ;
	TIM_SetCounter(TIM14, 0) ;
	TIM_Cmd(TIM14, ENABLE) ;
	while(--bTemp) ;
	fTimer1min = FALSE ;
}

