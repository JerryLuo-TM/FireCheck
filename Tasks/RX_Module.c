#include "Struct.h"
#include "sx1276.h"
#include "sx1278.h"

TaskHandle_t RXTT;
TaskHandle_t RXRT;

RINGBUFF_T RX_RX_Send_rxring;//2401接收端  整合校验后的数据缓冲区
uint8_t    RX_Rx_Send[512];

bool       Data_RX_Txing;

/*               串口接收数据帧
	 bit0 bit1   bit2  bit3    bit4-bitx  bitx+1 bitx+2
   帧头 载荷长 序列  包功能  传输数据   校验位(两个字节)
	 0xAA  -      -     -       -          -
   分包发送函数
*/
void RX_Tx_LoadPacket(unsigned char TX_col_number,unsigned char *data,unsigned short length)
{
	unsigned int i;
	for(i=0;i<255;i++){RF.Tx_Buffer[i]=0;}//清空缓冲区
	RF.Tx_Buffer[0]=TX_col_number;//随机数+总包数
	RF.Tx_Buffer[1]=(length&0xFF);//此包有效字节长度
	for(i=0;i<length;i++)
	{
		 RF.Tx_Buffer[i+2]=*data++; //发送缓冲中装载数据
	}
}

//飞控端的 915 向地面发送数据
static void RX_Tx_Task(void *pvParameters)
{
	unsigned char  cont;
	unsigned char  Tx_Buf[300];   //拆包用的缓冲区
	TickType_t   xLastWakeTime;   //用于精准定时的变量
	while (1)
	{
		xLastWakeTime = xTaskGetTickCount(); //获取当前Tick次数,以赋给延时函数初值
		if((RingBuffer_GetCount(&Receive_USART_rxring)>6)&&(Data_RX_Txing==false))
		{
			Data_RX_Txing=true;cont++;//生成动态数
			RingBuffer_PopMult(&Receive_USART_rxring,&Tx_Buf[0],2);//从缓冲区中弹出帧头和载荷长度
			RingBuffer_PopMult(&Receive_USART_rxring,&Tx_Buf[2],Tx_Buf[1]+4);//继续弹出剩下的有效数据包
			RX_Tx_LoadPacket(cont,&Tx_Buf[0],Tx_Buf[1]+6);
			SX1276_LoRaTxPacket(RF.Tx_Buffer,Tx_Buf[1]+8);
 			Data_RX_Txing=false;
			LED00=!LED00;
		}
		vTaskDelayUntil(&xLastWakeTime,configTICK_RATE_HZ/1000);
	}
}

//飞控端 433 接收地面站数据
static void RX_Rx_Task(void *pvParameters)
{
	unsigned short i=0;
	static unsigned char  CK_A=0,CK_B=0; //计算校验和
	static unsigned char  last_num=0,num=0;
	static unsigned short length=0;
	TickType_t xLastWakeTime;         //用于精准定时的变量
	while (1)
	{
		xLastWakeTime = xTaskGetTickCount(); //获取当前Tick次数,以赋给延时函数初值
		length=SX1278_LoRaRxPacket(RF.Rx_Buffer);
		if(length>0)
		{
			last_num=num;num=RF.Rx_Buffer[0];
			if((last_num!=num)&&(length==(RF.Rx_Buffer[1]+2)))//数据包校验
			{
					CK_A=0;CK_B=0;//计算校验和  数据区
					for(i=2;i<(RF.Rx_Buffer[3]+6);i++)
					{
						CK_A=CK_A+RF.Rx_Buffer[i];
						CK_B=CK_B+CK_A;
					}
					//一帧完整的协议收入成功  比较两个校验和参数
					if((CK_A==RF.Rx_Buffer[length-2])&&(CK_B==RF.Rx_Buffer[length-1]))
					{
						LED11=!LED11;
						//校验通过  将标准协议包压入待发缓冲
						RingBuffer_InsertMult(&RX_RX_Send_rxring,&RF.Rx_Buffer[2],length-2);//拷贝有效字节
					}
			}
		}
		vTaskDelayUntil(&xLastWakeTime,configTICK_RATE_HZ/1000);
	}
}
//接收到数据后校验通过 通过串口发出
static void RX_Uart_Send(void *pvParameters)
{
	unsigned char ch;
	TickType_t xLastWakeTime;         //用于精准定时的变量
	while (1)
	{
		xLastWakeTime = xTaskGetTickCount(); //获取当前Tick次数,以赋给延时函数初值
    if(RingBuffer_GetCount(&RX_RX_Send_rxring)>0)
		{
		  RingBuffer_Pop(&RX_RX_Send_rxring,&ch);
			uart_sendbyte(USART1,ch);
		}
		vTaskDelayUntil(&xLastWakeTime,configTICK_RATE_HZ/2000);
	}
}

void Start_RX_Task(void)
{
	RingBuffer_Init(&RX_RX_Send_rxring,&RX_Rx_Send,1,sizeof(RX_Rx_Send));//用于将整合好的数据包发出
	xTaskCreate(RX_Tx_Task,  "RX_Tx_Task",  512,NULL,2,&RXTT);
	xTaskCreate(RX_Rx_Task,  "RX_Rx_Task",  512,NULL,2,&RXRT);
	xTaskCreate(RX_Uart_Send,"RX_Uart_Send",512,NULL,2,NULL);
}
