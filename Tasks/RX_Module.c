#include "Struct.h"
#include "sx1276.h"
#include "sx1278.h"

TaskHandle_t RXTT;
TaskHandle_t RXRT;

RINGBUFF_T RX_RX_Send_rxring;//2401���ն�  ����У�������ݻ�����
uint8_t    RX_Rx_Send[512];

bool       Data_RX_Txing;

/*               ���ڽ�������֡
	 bit0 bit1   bit2  bit3    bit4-bitx  bitx+1 bitx+2
   ֡ͷ �غɳ� ����  ������  ��������   У��λ(�����ֽ�)
	 0xAA  -      -     -       -          -
   �ְ����ͺ���
*/
void RX_Tx_LoadPacket(unsigned char TX_col_number,unsigned char *data,unsigned short length)
{
	unsigned int i;
	for(i=0;i<255;i++){RF.Tx_Buffer[i]=0;}//��ջ�����
	RF.Tx_Buffer[0]=TX_col_number;//�����+�ܰ���
	RF.Tx_Buffer[1]=(length&0xFF);//�˰���Ч�ֽڳ���
	for(i=0;i<length;i++)
	{
		 RF.Tx_Buffer[i+2]=*data++; //���ͻ�����װ������
	}
}

//�ɿض˵� 915 ����淢������
static void RX_Tx_Task(void *pvParameters)
{
	unsigned char  cont;
	unsigned char  Tx_Buf[300];   //����õĻ�����
	TickType_t   xLastWakeTime;   //���ھ�׼��ʱ�ı���
	while (1)
	{
		xLastWakeTime = xTaskGetTickCount(); //��ȡ��ǰTick����,�Ը�����ʱ������ֵ
		if((RingBuffer_GetCount(&Receive_USART_rxring)>6)&&(Data_RX_Txing==false))
		{
			Data_RX_Txing=true;cont++;//���ɶ�̬��
			RingBuffer_PopMult(&Receive_USART_rxring,&Tx_Buf[0],2);//�ӻ������е���֡ͷ���غɳ���
			RingBuffer_PopMult(&Receive_USART_rxring,&Tx_Buf[2],Tx_Buf[1]+4);//��������ʣ�µ���Ч���ݰ�
			RX_Tx_LoadPacket(cont,&Tx_Buf[0],Tx_Buf[1]+6);
			SX1276_LoRaTxPacket(RF.Tx_Buffer,Tx_Buf[1]+8);
 			Data_RX_Txing=false;
			LED00=!LED00;
		}
		vTaskDelayUntil(&xLastWakeTime,configTICK_RATE_HZ/1000);
	}
}

//�ɿض� 433 ���յ���վ����
static void RX_Rx_Task(void *pvParameters)
{
	unsigned short i=0;
	static unsigned char  CK_A=0,CK_B=0; //����У���
	static unsigned char  last_num=0,num=0;
	static unsigned short length=0;
	TickType_t xLastWakeTime;         //���ھ�׼��ʱ�ı���
	while (1)
	{
		xLastWakeTime = xTaskGetTickCount(); //��ȡ��ǰTick����,�Ը�����ʱ������ֵ
		length=SX1278_LoRaRxPacket(RF.Rx_Buffer);
		if(length>0)
		{
			last_num=num;num=RF.Rx_Buffer[0];
			if((last_num!=num)&&(length==(RF.Rx_Buffer[1]+2)))//���ݰ�У��
			{
					CK_A=0;CK_B=0;//����У���  ������
					for(i=2;i<(RF.Rx_Buffer[3]+6);i++)
					{
						CK_A=CK_A+RF.Rx_Buffer[i];
						CK_B=CK_B+CK_A;
					}
					//һ֡������Э������ɹ�  �Ƚ�����У��Ͳ���
					if((CK_A==RF.Rx_Buffer[length-2])&&(CK_B==RF.Rx_Buffer[length-1]))
					{
						LED11=!LED11;
						//У��ͨ��  ����׼Э���ѹ���������
						RingBuffer_InsertMult(&RX_RX_Send_rxring,&RF.Rx_Buffer[2],length-2);//������Ч�ֽ�
					}
			}
		}
		vTaskDelayUntil(&xLastWakeTime,configTICK_RATE_HZ/1000);
	}
}
//���յ����ݺ�У��ͨ�� ͨ�����ڷ���
static void RX_Uart_Send(void *pvParameters)
{
	unsigned char ch;
	TickType_t xLastWakeTime;         //���ھ�׼��ʱ�ı���
	while (1)
	{
		xLastWakeTime = xTaskGetTickCount(); //��ȡ��ǰTick����,�Ը�����ʱ������ֵ
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
	RingBuffer_Init(&RX_RX_Send_rxring,&RX_Rx_Send,1,sizeof(RX_Rx_Send));//���ڽ����Ϻõ����ݰ�����
	xTaskCreate(RX_Tx_Task,  "RX_Tx_Task",  512,NULL,2,&RXTT);
	xTaskCreate(RX_Rx_Task,  "RX_Rx_Task",  512,NULL,2,&RXRT);
	xTaskCreate(RX_Uart_Send,"RX_Uart_Send",512,NULL,2,NULL);
}
