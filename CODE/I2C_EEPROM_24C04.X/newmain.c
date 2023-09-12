
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#define RS RE0
#define EN RE1
#define LCD PORTD
#define _XTAL_FREQ 4000000

void puls();
void cmd(char,char);
void st(char*);
void i2c_ins();
void wait();
void i2c_st();
void i2c_restart();
void i2c_write(char data);
char i2c_read();
void i2c_stop();

int main()
{
	int data;
	PORTC=PORTD=PORTE=0X00;
	TRISD=TRISE=0X00;
	TRISC=0X18;
	
	ANSEL=ANSELH=0X00;

	cmd(0,0x0e);
	cmd(0,0x38);
	cmd(0,0x80);

	i2c_ins();
	
	i2c_st();
	i2c_write(0xa0);
	i2c_write(0x00);
	i2c_write('1');
	i2c_stop();
	__delay_ms(1);
//	i2c_st();
//	i2c_write(0xa0);
//	i2c_write(0x00);
//	i2c_write('K');
//	i2c_stop();

	while(1)
	{
		i2c_st();
		i2c_write(0XA0);
		i2c_write(0x00);
		i2c_restart();
		i2c_write(0xa1);
		data=i2c_read();
		ACKDT=1;
		i2c_stop();
		cmd(1,data);
		
	}
}
void puls()
{
	EN=1;
	__delay_us(50);
	EN=0;
	__delay_us(50);
}
void cmd(char command,char data)
{
	RS=command;
	LCD=data;
	puls();
}
void i2c_ins()
{
	SSPSTAT=0x80;
	SSPCON=0x28;
	SSPCON2=0x00;
}
void st(char *data)
{
	while(*data)cmd(1,*data++);
}
void i2c_wait()
{
	while(SSPIF==0);
	SSPIF=0;
}
void i2c_st()
{
	SEN=1;
	while(SEN==1);
}
void i2c_restart()
{
	RSEN=1;
	while(RSEN==1);
}
void i2c_write(char data)
{
	SSPBUF=data;
	while(BF==1);
	while(ACKSTAT==1);
	//SSPIF=0;
}	
char i2c_read()
{
	int data;
	RCEN=1;
	while(BF==0);
	data=SSPBUF;
	ACKEN=1;
	return data;
}
void i2c_stop()
{
	PEN=1;
	while(PEN==1);
}
