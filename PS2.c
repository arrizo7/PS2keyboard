#include "PS2.h"
#include "scancode.h"
//5ms��ʱ
static unsigned char IntNum = 0; //�жϴ�������
static unsigned char KeyV; //��ֵ
static unsigned char Key_UP=0, Shift = 0;//Key_UP�Ǽ��ɿ���ʶ��Shift��Shift�����±�ʶ
static unsigned char BF = 0; //��ʶ�Ƿ����ַ����յ�
static unsigned char PS2_ready = 0; //���̳�ʼ����ϱ�־
static unsigned char LEDS=0; //�����ϵƵ�״̬

void wait_us(unsigned int delay)
{
delay=delay*OSC_US;
while (delay--);
}


void Keyboard_out(void) interrupt 2
{
if ((IntNum > 0) && (IntNum < 9))
{
KeyV = KeyV >> 1; //����������ǵ�>>�ߣ������һ����������һλ
if (Key_Data) KeyV = KeyV | 0x80; //������������Ϊ1ʱΪ1�����λ
}
IntNum++;
while (!Key_CLK); //�ȴ�PS/2CLK����
if (IntNum > 10)
{
IntNum = 0; //���ж�11�κ��ʾһ֡�������꣬�����׼����һ�ν���
BF = 1; //��ʶ���ַ���������
EX1 = 0; //���жϵ���ʾ����ٿ��ж� (ע�������ﲻ��BF�͹��ж�ֱ�ӵ�Decode()����Decode�������õ����к���Ҫ����Ϊ���뺯��)
//PutChar(2,0,KeyV);
}
}

void SendKeyCmd(unsigned char cmd){
bit D0,D1,D2,D3,D4,D5,D6,D7,parity,text1;
text1=EX1;
EX1=0;
	
if (Key_CLK)
 {
    wait_us(2000);
	Key_CLK=0;	
	D0=cmd&0x01;D1=cmd&0x02;D2=cmd&0x04;D3=cmd&0x08;
	D4=cmd&0x10;D5=cmd&0x20;D6=cmd&0x40;D7=cmd&0x80;
	parity=D0^D1^D2^D3^D4^D5^D6^D7;
	wait_us(20);
	Key_Data=0;
	wait_us(2);
	Key_CLK=1;
	while(Key_CLK);
	Key_Data=D0;	//дD0
	while(!Key_CLK);
	while(Key_CLK);
	Key_Data=D1;	//дD1
	while(!Key_CLK);
	while(Key_CLK);
	Key_Data=D2;	//дD2
	while(!Key_CLK);
	while(Key_CLK);
	Key_Data=D3;	//дD3
	while(!Key_CLK);
	while(Key_CLK);
	Key_Data=D4;	//дD4
	while(!Key_CLK);
	while(Key_CLK);
	Key_Data=D5;	//дD5
	while(!Key_CLK);
	while(Key_CLK);
	Key_Data=D6;	//дD6
	while(!Key_CLK);
	while(Key_CLK);
	Key_Data=D7;	//дD7
	while(!Key_CLK);
	while(Key_CLK);
	Key_Data=!parity;	//д��żУ��λ
	while(!Key_CLK);
	while(Key_CLK);
	Key_Data=1;	//дֹͣλ
	wait_us(20);
 }
EX1=text1;
}

unsigned char Decode(unsigned char ScanCode) //ע��:��SHIFT+GΪ12H 34H F0H 34H F0H 12H��Ҳ����˵shift��ͨ��+G��ͨ��+shift�Ķ���+G�Ķ���
{
unsigned char TempCyc,asc=0;

	if (ScanCode==0xAA)	PS2_ready=1;

if (!Key_UP) //�������ɿ�ʱ
{
switch (ScanCode)
 {
 case 0xF0 : // ���յ�0xF0��Key_UP��1��ʾ���뿪ʼ
	Key_UP = 1;
	break;

 case 0xFC :
	asc=0xFC;  //�Լ�ʧ��
	break;

 case 0xEE:
	asc=0xEE;  //���̻ظ�
	break;

 case 0x58: //������CapsLock
	LEDS^=KEY_LED_CAPS;
	SetKeyLED(LEDS);
	wait_us(2000);
	break;
 case 0x77: //������NumLock
	LEDS^=KEY_LED_NUM;
	SetKeyLED(LEDS);
	wait_us(2000);
	break;
 case 0x7E: //������ScrollLock
	LEDS^=KEY_LED_SCROLL;
	SetKeyLED(LEDS);
	wait_us(2000);
	break;
case 0x12 : // �� SHIFT
	Shift = 1;
	asc=0x10;
	break;

case 0x59 : // �� SHIFT
	Shift = 1;
	asc=0x10;
	break;

default:
  		   for (TempCyc = 0;(UnShifted[TempCyc][0]!=ScanCode)&&(TempCyc<26); TempCyc++);
			if (UnShifted[TempCyc][0] == ScanCode) 
				{
				if ((((LEDS&KEY_LED_CAPS)>0)&&(!Shift))||(((LEDS&KEY_LED_CAPS)==0)&&(Shift)))	//�жϴ�д 
						asc=Shifted[TempCyc][1]; 
						else 
						asc=UnShifted[TempCyc][1];
				} 
		if (asc==0)
		{ 
			for (TempCyc = 26;(UnShifted[TempCyc][0]!=ScanCode)&&(TempCyc<47); TempCyc++);  	//�ж�shift
			if (UnShifted[TempCyc][0] == ScanCode) 
				{
					if (Shift) 
						asc=Shifted[TempCyc][1]; 
						else 	
						asc=UnShifted[TempCyc][1];
				}
		}
		if (asc==0) 
		{
			for (TempCyc = 47;(UnShifted[TempCyc][0]!=ScanCode)&&(TempCyc<59); TempCyc++);
			if (UnShifted[TempCyc][0] == ScanCode)
				{																		   //С���̿���
				    if ((LEDS&KEY_LED_NUM)>0)
						asc=Shifted[TempCyc][1];
						else
						asc=UnShifted[TempCyc][1];
				}
		}
    	if (asc==0) 
		{ 
		 for(TempCyc = 0; (FuncKey[TempCyc][0]!=ScanCode)&&(TempCyc<34); TempCyc++);
		 if (FuncKey[TempCyc][0] == ScanCode) asc=FuncKey[TempCyc][1];
		}
 break;
 }
}
else
{
	Key_UP = 0;
	switch (ScanCode) //�����ɿ�ʱ���������룬��G 34H F0H 34H ��ô�ڶ���34H���ᱻ����
	{
		case 0x12 : // �� SHIFT
		Shift = 0;
		break;

		case 0x59 : // �� SHIFT
		Shift = 0;
		break;
	}
}
return asc;
}

unsigned char KeyInit()
{
EX1=1;
if (BF&&KeyV==0xAA) 
 { 
   	SetKeyRate(0x1f);
	//SetKeySet(2);
	//SetAllKeyType(KEY_TYPE_M);
	LEDS^=KEY_LED_NUM;
	SetKeyLED(LEDS);
	//SendEcho();
	BF=0;
	return 1;
 }
return 0; 
}

bit KeyState()
{
	return BF;
}
void SetState(bit b)
{
	BF=b;
}
unsigned char GetCode()
{
unsigned char ktmp=KeyV;
	BF = 0; //��ʶ�ַ���������
	EX1 = 1;//�������жϽ�����һ����
	return ktmp;
}
unsigned char LEDState()
{
	return LEDS;
}

void KeyReset()
{
	BF=0;
	ResetKey();
	EX1=1;
	LEDS=0;
}