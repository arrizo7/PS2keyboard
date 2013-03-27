/************************************************************************************
**                            PS2��������ģ��
**                     (c) Copyright 2007-2013, Dodge
**                          All Rights Reserved
**                    https://github.com/dodgepudding/PS2keyboard
**                               V1.00.00
---------------------------------�ļ���Ϣ--------------------------------------------
**��  ��: PS2.c  ����ʵ��ģ��
**       PS2.h  �������ö��弰ͷ�ļ�����
**��д��: dodgepudding (dodgepudding#gmail.com)
**��  ���������������ͨPS2��101������
          �궨��˵����
          KEY_XXX 			����״̬���ܼ���
          SetXXXX(x) 		�趨����״̬
          ����ͷ�ļ���������ţ�
          	Key_Data P36 //��������ź�������
			Key_CLK P33	 //�˰汾CLK��ֻ����INT1�ţ��ⲿ�ж�1��ռ��
		  ʹ�÷�����
		  1���޸�PS2.Hͷ�ļ���Ӧ���� ��
		  2�������жϣ��ڳ���ǰλ��д��while (!KeyInit());ֱ������У��ɹ���
		  ����ʼ��Ĭ�ϰ����ظ�����2��/�룬ʹ�ü��̱��뼯2��NumLock״̬����
		  3����ʱ��������SendKeyCmd()����غ꣬��������Ҫ�ļ���״̬��
		  4���жϼ���״̬KeyState()�ķ���ֵ�����м����£�����Decode(GetCode())�����ASCII�롣
		  ��GetCode��ȡԭʼ���뱻ִ�к󣬻��ͷ�KeyV�ݴ棬�Զ�ȡ��һ��ֵ��ʹ���߿����н���
		  ���̻��棬��GetCode()ֵ�������������д���ԭʼ���롣
**��ʷ�汾��
			V1.00.00 �����ж�ģʽ��ģ��ʵ�ʼ��̵Ĳ����������ϵ�������Сд����ͨ���ܼ�
*************************************************************************************/


#include <ADuC842.h>  //���յ�Ƭ����������ͷ�ļ�
#define Key_Data P34 //��������ź�������
#define Key_CLK P33	 //�˰汾CLK��ֻ����INT1��
#define OSC_US 4	//�������Ƶ��1us���еĻ���ָ����Ŀ����У׼��ʱ

/*************************************************************************/
//PS2����ģʽ����
#define ResetKey()		SendKeyCmd(0xFF) //���̸�λ�����̵ȴ��ϳ�
#define SetDefault()    SendKeyCmd(0xF6) //����Ĭ��ģʽ
#define SetKeyRate(r)	SendKeyCmd(0xF3);SendKeyCmd(0x1F&r)	 //���ð�������0x00-0x1f��ֵԽ������Խ��
#define SetKeySet(t)	SendKeyCmd(0xF0);SendKeyCmd(t)		 //�趨����ʹ�ñ��뼯, ȡֵ1/2/3���˰汾ֻ֧��Ĭ��ģʽ2
#define KEY_TYPE_T		0x07    //�ظ�ģʽ
#define KEY_TYPE_MB		0x08	//ͨ/��ģʽ
#define KEY_TYPE_M		0x09	//ֻͨģʽ
#define KEY_TYPE_TMB	0x0A	//ͨ/��/�ظ�ģʽ
#define SetAllKeyType(t) SendKeyCmd(0xF0|t)	 //���ð�����Ӧģʽ��Ϊ�����ĸ�����֮һ�����̱��뼯3ģʽ�´˹��ܲ���Ч
#define ResendKey()		SendKeyCmd(0xFE)	 //�ط���һ��ֵ
#define	KeyDisable()	SendKeyCmd(0xF5)	 //���̽�ֹ
#define KeyEnable()		SendKeyCmd(0xF4)	 //��������
#define KEY_LED_SCROLL  0x01
#define KEY_LED_NUM		0x02
#define KEY_LED_CAPS	0x04
#define SetKeyLED(l)	SendKeyCmd(0xED);SendKeyCmd(0x07&l)	 //�����źŵ�״̬
#define SendEcho()	    SendKeyCmd(0xEE)	 //����ECHOѯ���źţ�����Ҳ�᷵��ECHO
/*************************************************************************/
/*************************************************************************/
//���⹦�ܼ�ASCII��
#define KEY_F1		0x13
#define KEY_F2		0x14
#define KEY_F3		0x15
#define KEY_F4		0x16
#define KEY_F5		0x17
#define KEY_F6		0x18
#define KEY_F7		0x19
#define KEY_F8		0x1A
#define KEY_F9		0x1C
#define KEY_F10		0x1D
#define KEY_F11		0x1E
#define KEY_F12		0x1F
#define KEY_ESC		0x1B
#define KEY_ENTER	0x0D
#define KEY_BACK	0x08
#define KEY_TAB		0x09
#define KEY_ALT		0x12
#define KEY_SHIFT	0x10
#define KEY_CTRL	0x11
#define KEY_INSERT	0x0A
#define KEY_HOME    0x06
#define KEY_PGUP	0x0B
#define KEY_DEL		0x0E
#define KEY_END		0x01
#define KEY_PGDN	0x03
#define KEY_UP		0x07
#define KEY_DOWN	0x02
#define KEY_LEFT	0x04
#define KEY_RIGHT	0x05

/*************************************************************************/
void SendKeyCmd(unsigned char cmd);	//���ͼ�������
unsigned char Decode(unsigned char ScanCode);  //���̽��룬���ص��ǰ�����ASCII�룬���⹦�ܼ�����
unsigned char KeyInit();   //���̳�ʼ��
bit KeyState();	   //����״̬��ѯ��Ϊ1��ʾ����δ����Ϊ0��ʾ�����Ѵ���
unsigned char  GetCode(); //���ԭʼ����
void SetState(bit b);	 //���ü��̰���״̬
unsigned char LEDState();//��ȡ���̵�״̬
void KeyReset();	 //��������
