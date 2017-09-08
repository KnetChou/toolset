/**  @file	Queue_Tool.c
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

/*****************************************************************************/
/* Included header files                                         		     */
/*****************************************************************************/
#include "Queue_DRV.h"

/*****************************************************************************/
/* Private types                                                             */
/*****************************************************************************/


/*****************************************************************************/
/* Private variables               		                                     */
/*****************************************************************************/


/*****************************************************************************/
/* Private constants 				                                         */
/*****************************************************************************/


/*****************************************************************************/
/* Private macros                                                            */
/*****************************************************************************/
#define	QUEUE_Empty_RETURN(Q)		if(Q==0)  return
#define	QUEUE_Empty_RETURN_NULL(Q)	if(Q==0)  return(0x00)

/*****************************************************************************/
/* Private API functions                                                     */
/*****************************************************************************/



/** @brief  push a data to the Buffer.
*   @param  typeDEV_QueueStruct* : QueuStruct memory address.
*   @return u8 : a data.
*   @see 
*/
void Queue_Push_Rear(typeDEV_QueueStruct* Queue, u8 bValue)
{
	u8 bTemp ;
	
	*((Queue->pBuffer)+(Queue->bRearPoint)) = bValue ;
	bTemp = Queue->bRearPoint + 1 ;	
	
	if(bTemp >= (Queue->bMAXsize) )
	{
		bTemp = 0 ;
	}
	
	if(bTemp == (Queue->bFrontPoint) )
	{
		#if 1		
		// 最後進來的不要
		bTemp = Queue->bRearPoint ;		
		#else
		// 最先進來的不要
		if(++(Queue->bFrontPoint) >= (Queue->bMAXsize) ) 
		{
			Queue->bFrontPoint = 0 ;
		}
		#endif
	}
	Queue->bRearPoint = bTemp ;
}


/** @brief  Get a data from the Buffer.
*   @param  typeDEV_QueueStruct* : QueuStruct memory address.
*   @return u8 : a data.
*   @see 
*/
u8 Queue_Pop_Front(typeDEV_QueueStruct* Queue)
{
	u8 bValue ;
	
	if(Queue->bFrontPoint == Queue->bRearPoint)
	{
		return (0xFE) ; //null
	}

	bValue = *((Queue->pBuffer)+(Queue->bFrontPoint)) ;
	if(++(Queue->bFrontPoint) >= (Queue->bMAXsize) )
	{
		Queue->bFrontPoint = 0 ;
	}
	return (bValue) ;	
}


/** @brief  Get information on the length of the buffer. 
*   @param  typeDEV_QueueStruct* : QueuStruct memory address.
*   @return u8 : Buffer Array Size.
*   @see 
*/
u8 Queue_Size(typeDEV_QueueStruct* Queue)
{
	u8 bFrontPoint, bRearPoint ;

	bFrontPoint = Queue->bFrontPoint ;
	bRearPoint = Queue->bRearPoint ;

	if(bRearPoint >= bFrontPoint)
	{
		return (bRearPoint-bFrontPoint) ;
	} 
	else
	{
		return (((Queue->bMAXsize)-bFrontPoint)+bRearPoint) ;	
	}
}

/** @brief  Release a dynamic memory space.
*   @param  typeDEV_QueueStruct* : QueuStruct memory address.
*   @return None.
*   @see 
*/
void Queue_Delete(typeDEV_QueueStruct* Queue)
{	
	free(Queue->pBuffer) ;
	free(Queue) ;
}

#ifdef _QUEUE_USE_MALLOC
/** @brief  Assign a dynamic memory space.
*   @param  bSize : Buffer size.
*   @return typeDEV_QueueStruct* : QueuStruct memory address.
*   @see 
*/
typeDEV_QueueStruct* Queue_Create(u8 bSize)
{
	typeDEV_QueueStruct* Queue ;
	u8* ptrBuffer ;

	Queue = (typeDEV_QueueStruct*)malloc(sizeof(typeDEV_QueueStruct));
	ptrBuffer = (u8*)malloc( (bSize+1) * sizeof(u8) ) ;
	
	Queue->pBuffer = ptrBuffer ;
	Queue->bMAXsize = (bSize+1) ;
	Queue->bFrontPoint = 0 ;
	Queue->bRearPoint = 0 ;

	return (Queue) ;
} 
#else
/** 
*   @brief  Assign a dynamic memory space.
*   @param  Queue : QueuStruct memory address.
*           ptrBuffer : Array memory address.
*           bSize : Buffer size.
*   @return None.
*   @see 
*/
void Queue_Create(typeDEV_QueueStruct* Queue, u8* ptrBuffer, u8 bSize)
{	
	Queue->pBuffer = ptrBuffer ;
	Queue->bMAXsize = (bSize-1) ;   // 20170801, Kent, 需要預留一個指標搬移使用.
	Queue->bFrontPoint = 0 ;
	Queue->bRearPoint = 0 ;
}	
#endif




/** @brief  Get a data to the Buffer.
*   @param  typeDEV_QueueStruct* : QueuStruct memory address.
			u8 bIndex : 
*   @return u8 : a data.
*   @see 
*/
u8 Queue_Get(typeDEV_QueueStruct* Queue, u8 bIndex)
{
	if(bIndex > Queue_Size(Queue) )
	{
		return (0xFE) ;
	}
	else
	{
		u8 bOffer ;

		bOffer = ((Queue->bFrontPoint)+bIndex)%(Queue->bMAXsize) ;
		return (*((Queue->pBuffer)+bOffer)) ;
	}	
}

/** @brief  Queue index point the reset to zero .
*   @param  typeDEV_QueueStruct* : QueuStruct memory address.
*   @return None.
*   @see 
*/
void Queue_Clear(typeDEV_QueueStruct* Queue)
{
	Queue->bFrontPoint = 0 ;
	Queue->bRearPoint = 0 ;
}

/** @brief  Queue index point the reset to zero .
*   @param  typeDEV_QueueStruct* : QueuStruct memory address.
*           u8* pArray : target array is storage the result.
*           u8 bLength : Copy data length.
*   @return None.
*   @see 
*/
void Queue_Copy(typeDEV_QueueStruct* Queue, u8* pArray, u8 bLength) 
{
	u8 bFrontPoint, bMaxPoint ;
	u8 i ;
	
	QUEUE_Empty_RETURN(Queue) ;

	bFrontPoint = Queue->bFrontPoint ;
	bMaxPoint = Queue->bMAXsize ;

	for(i=0 ; i<bLength ; i++ )
	{
		pArray[i] = *((Queue->pBuffer)+(bFrontPoint)) ;
		
		if(++bFrontPoint >= bMaxPoint)
		{
			bFrontPoint = 0 ;
		}
	}
}

/** @brief  Peep a data from the Buffer, Do not move the index point.
*   @param  typeDEV_QueueStruct* : QueuStruct memory address.
*   @return u8 : a data.
*   @see 
*/
u8 Queue_Peeping_Front(typeDEV_QueueStruct* Queue)
{
	u8 bValue ;
	
	if(Queue->bFrontPoint == Queue->bRearPoint)
	{
		return (0x00) ; //null
	}

	bValue = *((Queue->pBuffer)+(Queue->bFrontPoint)) ;
    
	return (bValue) ;	
}
