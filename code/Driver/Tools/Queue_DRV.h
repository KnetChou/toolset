/**  @file	Queue_Dve.h
*    @brief   
*    @version  1.0
*    @author    Kent Chou
*	 @see 
* 		Project Name :	
* 		CpuType		:  
* 		OSC      	:  External OSC 8MHz
*		Compiler 	:  
* 		IDE      	:  
* 		Create Date :  2017/08/01
*/
#ifndef	__Queue_DRV_h
#define	__Queue_DRV_h

/*** 選擇Queue記憶體分配方式 ***/
#define _QUEUE_USE_MALLOC

/*****************************************************************************/
/* Included header files                                       			 */
/*****************************************************************************/
#include "Global_STM32.h"
#include <stdlib.h>

/*****************************************************************************/
/* Public types                                                              */
/*****************************************************************************/

typedef struct	
{
   u8	*pBuffer ;
   u8	bMAXsize ;
   u8	bFrontPoint ;
   u8	bRearPoint ;
} typeDEV_QueueStruct;

/*****************************************************************************/
/* Public constants 			                        				 */                                                       
/*****************************************************************************/


/*****************************************************************************/
/* Public variables   							  			           */
/*****************************************************************************/


/*****************************************************************************/
/* Public macros                                                             */
/*****************************************************************************/


/*****************************************************************************/
/* Public API functions                                                      */
/*****************************************************************************/

#ifdef _QUEUE_USE_MALLOC
extern typeDEV_QueueStruct* Queue_Create(u8 bSize) ;
#else
extern void Queue_Create(typeDEV_QueueStruct* Queue, u8* ptrBuffer, u8 bSize) ;
#endif

extern void Queue_Delete(typeDEV_QueueStruct* Queue) ;
extern u8 Queue_Size(typeDEV_QueueStruct* Queue) ;
extern u8 Queue_Pop_Front(typeDEV_QueueStruct* Queue) ;
extern void Queue_Push_Rear(typeDEV_QueueStruct* Queue, u8 bValue) ;
extern u8 Queue_Get(typeDEV_QueueStruct* Queue, u8 bIndex) ;
extern void Queue_Clear(typeDEV_QueueStruct* Queue) ;
extern void Queue_Copy(typeDEV_QueueStruct* Queue, u8* pArray, u8 bLength) ;
extern u8 Queue_Peeping_Front(typeDEV_QueueStruct* Queue) ;

#endif

