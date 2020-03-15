/*	Author(s) Name & E-mail: Josh McIntyre (jmcin005)
 *	Lab Section: 022
 *	Assignment: Lab Project

 *	
 *	I acknowledge all content contained herein, excluding template 
 * 	or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

// PWM Manager
void set_PWM(double frequency) {
	static double current_frequency;
	
	if(frequency != current_frequency) {
		if(!frequency) {TCCR3C &= 0x08; }
		else { TCCR3B |= 0x03; }
		
		if(frequency < 0.954) { OCR3A = 0xFFFF; }
		
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		else { OCR3A = (short) (8000000 / (128 * frequency)) - 1; }
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

// Timer Manager
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	
	OCR1A = 125;
	
	TIMSK1 = 0x02;
	
	TCNT1 = 0;
	
	_avr_timer_cntcurr = _avr_timer_M;
	
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}


typedef struct task {
	int state; // Current state of the task
	unsigned long period; // Rate at which the task should tick
	unsigned long elapsedTime; // Time since task's previous tick
	int (*TickFct)(int); // Function to call for task's tick
} task;

task tasks[2];

const unsigned char tasksNum = 2;
const unsigned long alarmPeriod = 1;
const unsigned long menuPeriod = 10;
const unsigned long tasksPeriodGCD = 1;

enum ALARM_States { SOUND };
int ALARM_Tick(int state);

enum MENU_States { ARM_1, DISARM_1 };
int MENU_Tick(int state);

void TimerISR() {
	unsigned char i;
	for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
		if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += tasksPeriodGCD;
	}
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void ADC_Init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

// Shared Variables
unsigned char tmpA;
unsigned char tmpB;
unsigned char tmpC;
unsigned char tmpD;
volatile unsigned char armed;
unsigned short x;

int main(void) {
	
	DDRA = 0x00;		PORTA = 0xFF;
	DDRC = 0x00;		PORTC = 0xFF;
	DDRD = 0xFF;		PORTD = 0x00;
	DDRB = 0xFF;		PORTB = 0x00;
	
	ADC_Init();
	
	unsigned char i = 0;
	tasks[i].state = ARM_1;
	tasks[i].period = menuPeriod;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &MENU_Tick;
	i++;
	tasks[i].state = SOUND;
	tasks[i].period = alarmPeriod;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &ALARM_Tick;

	TimerSet(tasksPeriodGCD);
	TimerOn();
	
	PWM_on();
	
    while (1) { continue; }
	
	return 0;
}

int ALARM_Tick(int state) {
	
	tmpC = ~PINC;
	
	switch(state) {
		case SOUND:
			// If motion detected AND armed
			if(!tmpC && armed) {
				PWM_on();
				set_PWM(261.63);
			}
			else {
				PWM_off();
			}
			
			state = SOUND;
		break;
		
		default:
		break;
	}
	
	return state;
}

int MENU_Tick(int state) {
	
	x = ADC;
	
	switch(state) {
		
		case ARM_1:
			armed = 1;
			
			if(x >= 512) {
				state = ARM_1;
				tmpD = 0xFF;
			}
			else if(x < 300) {
				state = DISARM_1;
				tmpD = 0x00;
			}
		break;
		
		case DISARM_1:
			armed = 0;
			
			if(x <= 512) {
				state = DISARM_1;
				tmpD = 0x00;
			}
			else if(x > 700) {
				state = ARM_1;
				tmpD = 0xFF;
			}
		break;
		
		default:
		break;
	}
	
	PORTD = tmpD;
	
	return state;
}

