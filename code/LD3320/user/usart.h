#ifndef __USART_H
#define __USART_H

void UartIni(void);//���ڳ�ʼ��
void UARTSendByte(uint8_t DAT);	//���ڷ���һ�ֽ�����
void PrintCom(uint8_t *DAT); //��ӡ�����ַ�������
uint8_t print_hex(uint8_t *dat, uint8_t len);


#endif
