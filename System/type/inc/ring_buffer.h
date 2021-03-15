#ifndef __RING_BUFFER_H_
#define __RING_BUFFER_H_

#include "sys.h"

/** @defgroup Ring_Buffer CHIP: Simple ring buffer implementation
 * @ingroup CHIP_Common
 * @{
 */

/**
 * @brief Ring buffer structure
 */
typedef struct {
	void *data;   /* the buffer holding the data*/
	int count;
	int itemSz;   /* the size of the allocated buffer*/
	uint32_t head;/* data is added at offset (in % size)*/
	uint32_t tail;/* data is extracted from off. (out % size)*/
} RINGBUFF_T;


#if !defined(MAX)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#if !defined(MIN)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif


/**
 * @def		RB_VHEAD(rb)
 * volatile typecasted head index
 */
#define RB_VHEAD(rb)              (*(volatile uint32_t *) &(rb)->head)

/**
 * @def		RB_VTAIL(rb)
 * volatile typecasted tail index
 */
#define RB_VTAIL(rb)              (*(volatile uint32_t *) &(rb)->tail)

/**
 * @brief	Initialize ring buffer
 * @param	RingBuff	: Pointer to ring buffer to initialize
 * @param	buffer		: Pointer to buffer to associate with RingBuff
 * @param	itemSize	: Size of each buffer item size
 * @param	count		  : Size of ring buffer
 * @note	Memory pointed by @a buffer must have correct alignment of
 * 			@a itemSize, and @a count must be a power of 2 and must at
 * 			least be 2 or greater.
 * @return	Nothing
 */
int RingBuffer_Init(RINGBUFF_T *RingBuff, void *buffer, int itemSize, int count);

/**
 * @brief	Resets the ring buffer to empty
 *        复位缓冲区数据  清空
 * @param	RingBuff	: Pointer to ring buffer
 * @return	Nothing
 */
static __inline void RingBuffer_Flush(RINGBUFF_T *RingBuff)
{
	RingBuff->head = RingBuff->tail = 0;
}

/**
 * @brief	Return size the ring buffer
	        返回缓冲区尺寸
 * @param	RingBuff	: Pointer to ring buffer
 * @return	Size of the ring buffer in bytes
 */
static __inline int RingBuffer_GetSize(RINGBUFF_T *RingBuff)
{
	return RingBuff->count;
}

/**
 * @brief	Return number of items in the ring buffer
           返回缓冲区中的数据数目
 * @param	RingBuff	: Pointer to ring buffer
 * @return	Number of items in the ring buffer
 */
static __inline int RingBuffer_GetCount(RINGBUFF_T *RingBuff)
{
	return RB_VHEAD(RingBuff) - RB_VTAIL(RingBuff);
}

/**
 * @brief	Return number of free items in the ring buffer
           返回空闲空间
 * @param	RingBuff	: Pointer to ring buffer
 * @return	Number of free items in the ring buffer
 */
static __inline int RingBuffer_GetFree(RINGBUFF_T *RingBuff)
{
	return RingBuff->count - RingBuffer_GetCount(RingBuff);
}

/**
 * @brief	Return number of items in the ring buffer
					返回缓冲区是否已满
 * @param	RingBuff	: Pointer to ring buffer
 * @return	1 if the ring buffer is full, otherwise 0
 */
static __inline int RingBuffer_IsFull(RINGBUFF_T *RingBuff)
{
	return (RingBuffer_GetCount(RingBuff) >= RingBuff->count);
}

/**
 * @brief	Return empty status of ring buffer
          返回缓冲区是否为空
 * @param	RingBuff	: Pointer to ring buffer
 * @return	1 if the ring buffer is empty, otherwise 0
 */
static __inline int RingBuffer_IsEmpty(RINGBUFF_T *RingBuff)
{
	return RB_VHEAD(RingBuff) == RB_VTAIL(RingBuff);
}

/**
 * @brief	Insert a single item into ring buffer
 * @param	RingBuff	: Pointer to ring buffer
 * @param	data		: pointer to item
 * @return	1 when successfully inserted,
 *			0 on error (Buffer not initialized using
 *			RingBuffer_Init() or attempted to insert
 *			when buffer is full)
 */
int RingBuffer_Insert(RINGBUFF_T *RingBuff, const void *data);

/**
 * @brief	Insert an array of items into ring buffer
 * @param	RingBuff	: Pointer to ring buffer
 * @param	data		: Pointer to first element of the item array
 * @param	num			: Number of items in the array
 * @return	number of items successfully inserted,
 *			0 on error (Buffer not initialized using
 *			RingBuffer_Init() or attempted to insert
 *			when buffer is full)
 */
int RingBuffer_InsertMult(RINGBUFF_T *RingBuff, const void *data, int num);

/**
 * @brief	Pop an item from the ring buffer
 * @param	RingBuff	: Pointer to ring buffer
 * @param	data		: Pointer to memory where popped item be stored
 * @return	1 when item popped successfuly onto @a data,
 * 			0 When error (Buffer not initialized using
 * 			RingBuffer_Init() or attempted to pop item when
 * 			the buffer is empty)
 */
int RingBuffer_Pop(RINGBUFF_T *RingBuff, void *data);

/**
 * @brief	Pop an array of items from the ring buffer
 * @param	RingBuff	: Pointer to ring buffer
 * @param	data		: Pointer to memory where popped items be stored
 * @param	num			: Max number of items array @a data can hold
 * @return	Number of items popped onto @a data,
 * 			0 on error (Buffer not initialized using RingBuffer_Init()
 * 			or attempted to pop when the buffer is empty)
 */
int RingBuffer_PopMult(RINGBUFF_T *RingBuff, void *data, int num);


/**
 * @}
 */

#endif /* __RING_BUFFER_H_ */
