/**  @file	Timer_Sys_DRV.h
*    @brief    Driver Systick
*    @version  1.0
*    @copyright 
*    @author    Kent Chou
*	 @see 
* 		Project Name :	
* 		CpuType		:  ST32F030C8
* 		OSC      	:  External OSC 8MHz
*		Compiler 	:  Keil C
* 		IDE      	:  uVision4
* 		Create Date :  2015/12/31
*/
#ifndef	_Timer_Sys_DRV_h
#define	_Timer_Sys_DRV_h

/*****************************************************************************/
/* Included header files                                                     */
/*****************************************************************************/
#include "Hardware.h"

/*****************************************************************************/
/* Public types                                                              */
/*****************************************************************************/


/*****************************************************************************/
/* Public constants                                                          */
/*****************************************************************************/


/*****************************************************************************/
/* Public variables                                                          */
/*****************************************************************************/


/*****************************************************************************/
/* Public API functions                                                      */
/*****************************************************************************/

/* Must be modified : __Vendor_SysTickConfig */
extern void timer_sys_init(void) ; 
extern void timer_sys_enable(void) ;
extern void timer_sys_disable(void) ;
extern void timer_sys_ISR(void) ;

extern bool is_1ms_timer(void) ;
extern bool is_10ms_timer(void) ;
extern bool is_100ms_timer(void) ;
extern bool is_1sec_timer(void) ;
extern bool is_1min_timer(void) ;

extern void RTC_alarm_ISR(void) ;
extern bool is_delay_hour_occured(void) ;
extern void set_delay_one_hour(void) ;

extern void timer_14_ISR(void) ;

extern void do_1min_timer_calibration(void) ;

#endif

