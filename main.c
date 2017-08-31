/*
 * [jlam038@ucr.edu]_FINAL_PROJECT_SIMON.c
 *
 * Created: 8/22/2017 8:22:59 PM
 * Author : JC Lam
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "io.c"
//#include <avr/interrupt.h>

double Notes[] = {261.63, 329.63, 392.00, 493.88};
unsigned char randnumsgiven[9];
unsigned char owninput[9];
unsigned char i = 0x00;
unsigned char j = 0x00;
unsigned char push = 0;
unsigned char diffpattern;
unsigned char waitforbutton = 0;
unsigned char increment = 0;
unsigned char checkactive = 1;

void set_PWM(double frequency) {
	
	
	// Keeps track of the currently set frequency
	// Will only update the registers when the frequency
	// changes, plays music uninterrupted.
	static double current_frequency;
	if (frequency != current_frequency) {

		if (!frequency) TCCR3B &= 0x08; //stops timer/counter
		else TCCR3B |= 0x03; // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using Pre-scaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) OCR3A = 0xFFFF;
		
		// prevents OCR3A from underflowing, using Pre-scaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) OCR3A = 0x0000;
		
		// set OCR3A based on desired frequency
		else OCR3A = (short)(8000000 / (128 * frequency)) - 1;

		TCNT3 = 0; // resets counter
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB6 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a Pre-scaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}
	
void Starting()
{
	volatile unsigned char button = ~PINA & 0x01;
	volatile unsigned char button1 = ~PINA & 0x02;
	volatile unsigned char button2= ~PINA & 0x04;
	volatile unsigned char button3 = ~PINA & 0x08;
	if(button || button1 || button2 || button3)
	{
		push = 1;
	}	
	else if(!button || !button1 || !button2 || !button3)
	{
		push = 0;
	}
}
	
unsigned char rand_number(unsigned char min, unsigned char max) //generates random within a certain range
{
	return min + rand() % (max + 1 - min);	
}

unsigned char outputlights()
{
	unsigned char grabnum;
	unsigned char check0;
		grabnum = rand_number(0, 3);
		if(grabnum == 0)
		{
			check0 = 0x01; 
			//set_PWM(Notes[0]);
		}
		else if(grabnum == 1)
		{
			check0 = 0x02;
			//set_PWM(Notes[1]);
		}
		else if(grabnum == 2)
		{
			check0 = 0x04;
			//set_PWM(Notes[2]);
		}
		else if(grabnum == 3)
		{
			check0 = 0x08;
			//set_PWM(Notes[3]);
		}
		return check0;
}

unsigned char led = 0x00;
volatile unsigned char pressed = 0;
unsigned char pressbutton()
{
	volatile unsigned char button = ~PINA & 0x01;
	volatile unsigned char button1 = ~PINA & 0x02;
	volatile unsigned char button2= ~PINA & 0x04;
	volatile unsigned char button3 = ~PINA & 0x08;
	if(pressed == 0x00)
	{
		if(button || button1 || button2 || button3)
		{
			pressed = 1;
			waitforbutton = 1;
		}
		else if(!button || !button1 || !button2 || !button3)
		{
			led = 0x00;
			pressed = 0;
		}
	}
	if(pressed == 0x01)
	{
		if(button)
		{
			pressed = 0;
			led = 0x01;
			PORTB = 0x01;
			set_PWM(Notes[0]);
			_delay_ms(3000);
			PORTB = 0x00;
			set_PWM(0);
		}
		else if(button1)
		{
			pressed = 0;
			led = 0x02;
			PORTB = 0x02;
			set_PWM(Notes[1]);
			_delay_ms(3000);
			PORTB = 0x00;
			set_PWM(0);
		}
		else if(button2)
		{
			pressed = 0;
			led = 0x04;
			PORTB = 0x04;
			set_PWM(Notes[2]);
			_delay_ms(3000);
			PORTB = 0x00;
			set_PWM(0);
		}
		else if(button3)
		{
			pressed = 0;
			led = 0x08;
			PORTB = 0x08;
			set_PWM(Notes[3]);
			_delay_ms(3000);
			PORTB = 0x00;
			set_PWM(0);
		}	
	}
	return led;
}

unsigned char comparearrays()
{
	unsigned char repeatpattern = 0;
	while(repeatpattern <= i)
	{
		if(randnumsgiven[repeatpattern] == owninput[repeatpattern])
		{
			++repeatpattern;
		}
		else if(randnumsgiven[repeatpattern] != owninput[repeatpattern])
		{
			repeatpattern = 0;
			i = 0;
			j = 0;
			Lose_Screen();
		}
	}
}
void repeatarrayoutput()
{
	for(unsigned char q = 0; q <= i; ++q)
	{
		if(randnumsgiven[q] == 0x01)
		{
			PORTB = randnumsgiven[q];
			set_PWM(Notes[0]);
			_delay_ms(3000);
			PORTB = 0x00;
			set_PWM(0);
			_delay_ms(3000);
		}
		else if(randnumsgiven[q] == 0x02)
		{
			PORTB = randnumsgiven[q];
			set_PWM(Notes[1]);
			_delay_ms(3000);
			PORTB = 0x00;
			set_PWM(0);
			_delay_ms(3000);
		}
		else if(randnumsgiven[q] == 0x04)
		{
			PORTB = randnumsgiven[q];
			set_PWM(Notes[2]);
			_delay_ms(3000);
			PORTB = 0x00;
			set_PWM(0);
			_delay_ms(3000);
		}
		else if(randnumsgiven[q] == 0x08)
		{
			PORTB = randnumsgiven[q];
			set_PWM(Notes[3]);
			_delay_ms(3000);
			PORTB = 0x00;
			set_PWM(0);
			_delay_ms(3000);
		}
	}
}

void Start_again()
{
	LCD_DisplayString(1, "Press any button to start");
}
void Lose_Screen()
{
	LCD_DisplayString(1, "You lost. Try again");
	set_PWM(Notes[0]);
	_delay_ms(3000);
	set_PWM(Notes[1]);
	_delay_ms(3000);
	set_PWM(Notes[2]);
	_delay_ms(3000);
	set_PWM(Notes[3]);
	_delay_ms(3000);
	i = 0;
	j = 0;
	_delay_ms(10000);
}
void Display_Victory()
{
	//checkactive = 0;
	LCD_DisplayString(1, "Victory!!!");
	set_PWM(Notes[3]);
	_delay_ms(3000);
	set_PWM(Notes[2]);
	_delay_ms(3000);
	set_PWM(Notes[1]);
	_delay_ms(3000);
	set_PWM(Notes[0]);
	_delay_ms(3000);
	i = 0;
	j = 0;
}

void letbuttongo()
{
	volatile unsigned char button = ~PINA & 0x01;
	volatile unsigned char button1 = ~PINA & 0x02;
	volatile unsigned char button2= ~PINA & 0x04;
	volatile unsigned char button3 = ~PINA & 0x08;
	if(button || button1 || button2 || button3)
	{
		waitforbutton = 1;
	}
	else if(!button || !button1 || !button2 || !button3)
	{
		waitforbutton = 0;
	}
}

int main(void)
{
	Beginning:
    DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	 srand(time(0)+ rand_number(0,100));
	unsigned char checkval;
	unsigned char holdval;
	waitforbutton = 0;
	PWM_on();
	set_PWM(0);
	LCD_init();
	LCD_DisplayString(1, "Welcome to Simon");
	_delay_ms(12000);
	Start_again();
	while(push == 0)
	{
		Starting();
	}
	LCD_DisplayString(1, "Let's Start");
	LCD_Cursor(12);
	push = 0;
	_delay_ms(10000);
    while (1) 
    {
		while(checkactive)
		{
			if(i < 9)
			{
				LCD_DisplayString(1, "Score: ");
				LCD_WriteData(i + '0');
				holdval = outputlights(); //puts in value of random light
				randnumsgiven[i] = holdval; //puts each individual value into randnumsgiven
				repeatarrayoutput();
				while(j < (i + 1))
				{
					while(waitforbutton == 0) //goes to waitforbutton == 0
					{
						checkval = pressbutton(); //check what button is pressed
					}							//exits with waitforbutton == 1
					owninput[j] = checkval; // get inputs and reset after every new pattern
					if(owninput[j] != randnumsgiven[j])
					{
						Lose_Screen();
						//++diffpattern;
						goto Beginning;
					}
					while(waitforbutton == 1)
					{
						letbuttongo();
					}
					++j;
				}
				if((i + 1) == j)
				{
					comparearrays(); //compares owninput[i] and randnumsgiven[i]
					++i;
					j = 0;
				}
				_delay_ms(5000);
			}
			if(i == 9)
			{
				LCD_DisplayString(1, "Score: ");
				LCD_WriteData(i + '0');
				_delay_ms(3000);
				Display_Victory();
				//++diffpattern;
				goto Beginning; 
			
			}	
		}
	}
}