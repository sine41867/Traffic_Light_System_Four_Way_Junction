#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void InitialiseGeneral();
void InitialiseTimer1();
void Initialise_INT();

void AllStopState();

void GoStateTrafficLight1();
void GoStateTrafficLight2();
void GoStateTrafficLight3();
void GoStateTrafficLight4();

void ReadyStateTrafficLight1();
void ReadyStateTrafficLight2();
void ReadyStateTrafficLight3();
void ReadyStateTrafficLight4();

void BeforeStopStateTrafficLight1();
void BeforeStopStateTrafficLight2();
void BeforeStopStateTrafficLight3();
void BeforeStopStateTrafficLight4();

void Enable_INT();
void Disable_INT();

void Blinker();
void DisplayNumber(int Number);

#define IDLE_MODE 0
#define NORMAL_MODE 1

unsigned char COUNTER;
unsigned char TRAFFIC_LIGHT_NO;
unsigned char MODE;
unsigned char YELLOW_ON;
unsigned char PREVIOUS_COUNT;

unsigned char SEGMENT_NUMBERS[10] = {
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01101111  // 9
};

int main( void )
{
	InitialiseGeneral();
	Initialise_INT();
	InitialiseTimer1();
	
	EIMSK = 0b00110000;		
	
    while(1)        	
    {
		if(MODE == IDLE_MODE && COUNTER != PREVIOUS_COUNT)
		{
			PREVIOUS_COUNT = COUNTER;
			Blinker();
		}
		else if(MODE == NORMAL_MODE)
		{
			DisplayNumber(COUNTER);
			
			switch(TRAFFIC_LIGHT_NO)
			{
				case 1:
					if(COUNTER == 0)
					{
						COUNTER = 9;
						GoStateTrafficLight2();
						TRAFFIC_LIGHT_NO = 2;
					}
					else if(COUNTER == 4)
					{
						ReadyStateTrafficLight2();
					}
					break;
				case 2:
					if(COUNTER == 0)
					{
						COUNTER = 9;
						GoStateTrafficLight3();
						TRAFFIC_LIGHT_NO = 3;
					}
					else if(COUNTER == 4)
					{
						ReadyStateTrafficLight3();
					}
					break;
				case 3:
					if(COUNTER == 0)
					{
						COUNTER = 9;
						GoStateTrafficLight4();
						TRAFFIC_LIGHT_NO = 4;
					}
					else if(COUNTER == 4)
					{
						ReadyStateTrafficLight4();
					}
					break;
				case 4:
					if(COUNTER == 0)
					{
						COUNTER = 9;
						GoStateTrafficLight1();
						TRAFFIC_LIGHT_NO = 1;
					}
					else if(COUNTER == 4)
					{
						ReadyStateTrafficLight1();
					}
					break;
				default:
					break;;
			}
		}
		else
		{
		}
	}
}

void InitialiseGeneral()
{
	DDRA = 0xFF;
	DDRB = 0xFF;
	DDRE = 0x00;
	DDRD = 0xFF;
	DDRF = 0xFF;
	
	AllStopState();	
	PORTF = 0x00;
	PORTD = 0x00;
	
	sei();					
	
	COUNTER = 9;
	PREVIOUS_COUNT = 0;
	TRAFFIC_LIGHT_NO = 1;	
	MODE = IDLE_MODE;	
	DisplayNumber(9);
	YELLOW_ON = 0;		
}

void InitialiseTimer1()		
{
	TCCR1A = 0b00000000;	
	TCCR1B = 0b00001101;
	TCCR1C = 0b00000000;
	
	
	OCR1AH = 0x01; 
	OCR1AL = 0xD0;

	TCNT1H = 0b00000000;	
	TCNT1L = 0b00000000;
	TIMSK1 = 0b00000010;	
							
}

void Initialise_INT()
{
	EICRA = 0b00000000;	
	EICRB = 0b00000101;
	EIMSK = 0b00000000;		
	EIFR = 0b11111111;		
}

void AllStopState()
{
	PORTA = 0b00001001;
	PORTB = 0b00001001;
	
}

void GoStateTrafficLight1()
{
	PORTA = 0b00001100;
	PORTB = 0b00001001;
	
	PORTD = 0b00000001;
}

void GoStateTrafficLight2()
{
	PORTA = 0b00100001;
	PORTB = 0b00001001;
	
	PORTD = 0b00000010;
}

void GoStateTrafficLight3()
{
	PORTA = 0b00001001;
	PORTB = 0b00001100;
	
	PORTD = 0b00000100;
	
}

void GoStateTrafficLight4()
{
	PORTA = 0b00001001;
	PORTB = 0b00100001;
	
	PORTD = 0b00001000;
	
}

void ReadyStateTrafficLight1()
{
	PORTA = 0b00001011;
	PORTB = 0b00110001;
	
}

void ReadyStateTrafficLight2()
{
	PORTA = 0b00011110;
	PORTB = 0b00001001;
	
}

void ReadyStateTrafficLight3()
{
	PORTA = 0b00110001;
	PORTB = 0b00001011;
	
}

void ReadyStateTrafficLight4()
{
	PORTA = 0b00001001;
	PORTB = 0b00011110;
	
}

void Blinker()
{
	if (YELLOW_ON == 1)
	{
		PORTA = 0x00;
		PORTB = 0x00;
		YELLOW_ON = 0;
		
	}
	else
	{
		PORTA = 0b00010010;
		PORTB = 0b00010010;
		
		YELLOW_ON = 1;
		
	}
}

void Enable_INT()
{
	EIMSK = 0b00110000;		
}

void Disable_INT()
{
	EIMSK = 0b00000000;
}

void DisplayNumber(int Number)
{
	PORTF = ~SEGMENT_NUMBERS[Number]; 
}

ISR(TIMER1_COMPA_vect) 
{	
	COUNTER--;
}

ISR(INT4_vect) 
{
	Disable_INT();
	MODE = IDLE_MODE;
	PORTD = 0x00;
	COUNTER = 9;
	PREVIOUS_COUNT = 0;
	PORTF = 0x00;
	Enable_INT();
}

ISR(INT5_vect) 
{
	Disable_INT();
	COUNTER = 9;
	MODE = NORMAL_MODE;
	PREVIOUS_COUNT = 0;
	TRAFFIC_LIGHT_NO = 1;
	AllStopState();
	GoStateTrafficLight1();
	Enable_INT();
}
