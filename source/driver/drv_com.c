/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-08 xqzhao
* Initial revision.
*
************************************************/
#include "drv_com.h"
#include "event.h"
#include "ssz_safe_queue.h"
#include "ssz_common.h"
#include "dev_def.h"
#include "ssz_uart.h"

/************************************************
* Declaration
************************************************/
typedef struct {
	SszUART uart;
	SszSafeQueue com_recv_queue;
	EventID com_event;
}DrvComInfo;
/************************************************
* Variable 
************************************************/
static uint8_t g_drv_pc_uart_recv_buff[64];
static uint8_t g_drv_bt_uart_recv_buff[64];
static uint8_t g_drv_mcu_uart_recv_buff[256];
static DrvComInfo g_drv_com_info[kComIDMax];
static SszFile g_drv_com_file[kComIDMax];
/************************************************
* Function 
************************************************/
//usually called at interrupt handler
void drv_com_on_receive_one_byte(ComID com_id, unsigned char ch)
{
	//push to recv buff
	if(ssz_safe_queue_push(&g_drv_com_info[com_id].com_recv_queue, &ch)!=0){
		ssz_traceln("com[%d] is full, ignore byte[%x]\n", com_id, ch);
	}
	//set event
	event_set_quick(g_drv_com_info[com_id].com_event);
}

//return ssz file
SszFile* drv_com_file(ComID com_id)
{
	ssz_assert(com_id < kComIDMax);
	return &g_drv_com_file[com_id];
}

static int drv_com_file_read(struct ssz_file_t* file, void* buff, int buff_size, int* real_read_buff_size) {
	DrvComInfo* dev = file->dev_data;
	for (int i = 0; i < buff_size; i++) {
		//wait until not empty
		while (ssz_safe_queue_is_empty(&dev->com_recv_queue)) {};
		//lint -e{613}
		((uint8_t*)buff)[i] = *(uint8_t*)ssz_safe_queue_front(&dev->com_recv_queue);
		ssz_safe_queue_pop(&dev->com_recv_queue);
	}
	if (real_read_buff_size)*real_read_buff_size = buff_size;
	return kSuccess;
}
static int drv_com_file_write(struct ssz_file_t* file, const void* buff, int buff_size, int* real_write_buff_size) {
	DrvComInfo* dev = file->dev_data;

	if (ssz_uart_write(&dev->uart, buff, buff_size)) {
		if (real_write_buff_size)*real_write_buff_size = buff_size;
		return kSuccess;
	}
	else {
		if (real_write_buff_size)*real_write_buff_size = 0;
		return kError;
	}
}
static int drv_com_file_ctl(struct ssz_file_t* file, int command, void* param) {
	int ret = kSuccess;
	(void)param;
	DrvComInfo* dev = file->dev_data;
	switch (command) {
	case SSZ_IOCTL_SIZE:
		(*(int32_t*)param) = ssz_safe_queue_size(&dev->com_recv_queue);
		break;
	default:
		ret = kNotSupport;
		break;
	}
	return ret;
}

const SszIODevice kDrvComIODev = {
	NULL,
	drv_com_file_read,
	drv_com_file_write,
	drv_com_file_ctl,
};
void drv_com_init()
{
	ComID com_id1 = kComPCUart;
    ComID com_id2 = kComMCUUart;
    ComID com_id3 = kComBTUart;
    
	g_drv_com_info[com_id1].com_event = kEventPCUartReceiveNewData;
	g_drv_com_info[com_id1].uart.obj = &PC_UART;
	g_drv_com_info[com_id1].uart.is_enable_synchronous = false;
	ssz_safe_queue_init(&g_drv_com_info[com_id1].com_recv_queue, g_drv_pc_uart_recv_buff,
		ssz_array_size(g_drv_pc_uart_recv_buff), ssz_array_node_size(g_drv_pc_uart_recv_buff));

	g_drv_com_info[com_id2].com_event = kEventMCUUartReceiveNewData;
	g_drv_com_info[com_id2].uart.obj = &MCU_UART;
	g_drv_com_info[com_id2].uart.is_enable_synchronous = false;
	ssz_safe_queue_init(&g_drv_com_info[com_id2].com_recv_queue, g_drv_mcu_uart_recv_buff,
		ssz_array_size(g_drv_mcu_uart_recv_buff), ssz_array_node_size(g_drv_mcu_uart_recv_buff));

    g_drv_com_info[com_id3].com_event = kEventBTUartReceiveNewData;
	g_drv_com_info[com_id3].uart.obj = &BT_UART;
	g_drv_com_info[com_id3].uart.is_enable_synchronous = false;
	ssz_safe_queue_init(&g_drv_com_info[com_id3].com_recv_queue, g_drv_bt_uart_recv_buff,
		ssz_array_size(g_drv_bt_uart_recv_buff), ssz_array_node_size(g_drv_bt_uart_recv_buff));    
	//init file
	for (int i = 0; i < kComIDMax; i++) {
		ssz_file_init(&g_drv_com_file[i], &kDrvComIODev, &g_drv_com_info[i]);
#ifdef SSZ_TARGET_MACHINE
		//lint -e{506}
		__HAL_UART_ENABLE_IT((UART_HandleTypeDef*)g_drv_com_info[com_id1].uart.obj, UART_IT_RXNE);
		//lint -e{506}
        __HAL_UART_ENABLE_IT((UART_HandleTypeDef*)g_drv_com_info[com_id2].uart.obj, UART_IT_RXNE);
		//lint -e{506}
        __HAL_UART_ENABLE_IT((UART_HandleTypeDef*)g_drv_com_info[com_id3].uart.obj, UART_IT_RXNE);
#endif
	}

}

#ifdef SSZ_TARGET_MACHINE
/**
  * @brief UART error callback.
  * @param huart: UART handle.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	ssz_traceln("uart[0x%x] error",huart->Instance);
  	__HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
}
#endif