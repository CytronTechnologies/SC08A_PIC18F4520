//==========================================================================
//	Author				: Ng Hock Yuan	
//	Project				: Sample code for SC08A using 18F4520
//	Project description	: This source code is used to control 8 servos.
//						  The 8 servos will rotate from one end to another end 
//						  Position reporting subroutine is used to wait for servo reach the other end.    	
//
//
//**************************************************************
//	include
//==========================================================================
#include <p18f4520.h>   					// this sample code is using 18F4520 !!
#include "usart.h"

//	configuration
//==========================================================================
#pragma	config OSC = HS					// HS oscillator
#pragma	config FCMEN = OFF				// Fail-Safe Clock Monitor disabled
#pragma	config IESO = OFF				// Oscillator Switchover mode disabled
#pragma	config PWRT = OFF				// PWRT disabled
#pragma	config BOREN = OFF				// Brown-out Reset disabled in hardware and software
#pragma	config WDT = OFF				// WDT disabled (control is placed on the SWDTEN bit)
#pragma	config MCLRE = ON				// MCLR pin enabled; RE3 input pin disabled 
#pragma	config PBADEN = OFF				// PORTB<4:0> pins are configured as digital I/O on Reset 
#pragma	config CCP2MX = PORTC			// CCP2 input/output is multiplexed with RC1 
#pragma	config LVP = OFF				// Single-Supply ICSP disabled 
#pragma	config XINST = OFF				// Extended Instruction Set


//	define
//==========================================================================
#define sw1		PORTBbits.RB0					
#define sw2		PORTBbits.RB1					
#define	LED1	LATBbits.LATB6
#define LED2	LATBbits.LATB7


//	global variable
//=========================================================================
unsigned int received_servo_position;   


//	function prototype				(every function must have a function prototype)
//==========================================================================
void send_cmd(unsigned int num, unsigned int data, unsigned int ramp); 
void request_feedback(unsigned char num);				
void get_position(void);							
void on_off_cmd(unsigned char num, unsigned char on_off);
//	main function					(main fucntion of the program)
//==========================================================================
void main(void)
{
	unsigned int j,temp;
	unsigned char i;
	//set IO port for led and switch
	TRISC = 0b10000000;					//set input or output
	TRISB = 0b00000011;

	//-------------------------------------------------------------------------
	// Configure UART
  	//-------------------------------------------------------------------------
	OpenUSART( USART_TX_INT_OFF &	// Transmit interrupt off	
		USART_RX_INT_OFF &			// Receive interrupt off
		USART_ASYNCH_MODE &			// Asynchronous mode
		USART_EIGHT_BIT &			// 8-bit data
		USART_CONT_RX &				// Continuous reception
		USART_BRGH_HIGH,			// High baud rate			
		129);					// 20Mhz
	
	on_off_cmd(0,1);
			
	while(1)
	{		
			if(sw1==0)
			{
				while(sw1==0);
			
				while(1)
				{
					for(i=0x01;i<9;i+=1)								
					{												
						send_cmd(i,7000,50);							
					}
						request_feedback(1);						
						get_position();								
						while(received_servo_position!=7000)		
						{
							request_feedback(1);					
							get_position();							
						}			
			
					
					for(i=0x01;i<9;i+=1)
					{
						send_cmd(i,1000,50);
					}	
						request_feedback(1);
						get_position();	
						while(received_servo_position!=1000)		
						{
							request_feedback(1);					
							get_position();							
						}
												
			
				}//while loop
			}//if loop
			
	}//while loop
		
}//main loop

// UART subroutine
//============================================================================	
//command Enable/disable the servo channel
void on_off_cmd(unsigned char num, unsigned char on_off)
{
	unsigned char first_byte=0;

	first_byte=0b11000000|num;
	
		while (BusyUSART());
		putcUSART(first_byte);		
		while (BusyUSART());
		putcUSART(on_off);					
}
void send_cmd(unsigned int num, unsigned int data, unsigned int ramp)
{
	unsigned char higher_byte=0, lower_byte=0;

	num=num|0b11100000;
	higher_byte=(data>>6)&0b01111111;
	lower_byte=data&0b00111111;


			while (BusyUSART());
			putcUSART(num);
			//delay(2000);
			while (BusyUSART());
			putcUSART(higher_byte);
			//delay(2000);
			while (BusyUSART());
			putcUSART(lower_byte);
			//delay(2000);
			while (BusyUSART());
			putcUSART(ramp);
			//delay(2000);
}

void request_feedback(unsigned char num)			
{	

		num=num|0b10100000;
			
			while (BusyUSART());
			putcUSART(num);								
}

void get_position(void)								
{unsigned int i;
static unsigned int higher_byte=0,lower_byte=0,received_position=0;

	while(PIR1bits.RCIF==0);														
	higher_byte=getcUSART();
	while(PIR1bits.RCIF==0);															
	lower_byte=getcUSART();																	
						
	received_servo_position=((higher_byte<<6)|(lower_byte&0x3F)); 		

}


