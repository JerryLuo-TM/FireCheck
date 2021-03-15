/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "LED.h"
#include "delay.h"


/*
*********************************************************************************************************
*	�� �� ��: NMI_Handler
*	����˵��: ���������жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void NMI_Handler(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: HardFault_Handler
*	����˵��: Ӳ��ʧЧ�жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  debug_printf("HardFault_Handler \r\n");
  while (1) {
    LED_R = 1;
    LED_B = 1;
    delay_ms(300);
    LED_R = 0;
    LED_B = 1;
    delay_ms(300);
  }
}

/*
*********************************************************************************************************
*	�� �� ��: MemManage_Handler
*	����˵��: �ڴ�����쳣�жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*
*********************************************************************************************************
*	�� �� ��: BusFault_Handler
*	����˵��: ���߷����쳣�жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/*
*********************************************************************************************************
*	�� �� ��: UsageFault_Handler
*	����˵��: δ�����ָ���Ƿ�״̬�жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/*
*********************************************************************************************************
*	�� �� ��: SVC_Handler
*	����˵��: ͨ��SWIָ���ϵͳ��������жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SVC_Handler(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: DebugMon_Handler
*	����˵��: ���Լ������жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DebugMon_Handler(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: PendSV_Handler
*	����˵��: �ɹ����ϵͳ��������жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void PendSV_Handler(void)
{
}


/*
*********************************************************************************************************
*	�� �� ��: SysTick_Handler
*	����˵��: ϵͳ��શ�ʱ���жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SysTick_Handler(void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: USB_LP_CAN1_RX0_IRQHandler
*	����˵��: �����ȼ�USB�жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	//usb_Istr();
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

