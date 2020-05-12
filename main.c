/*
 * midi.c
 *
 * Created: 3/7/2020 10:19:11 PM
 * Author : Robin Westerik
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>


#define BAUD  31250 //Standard baud rate for midi
#define BRC   (F_CPU/16/BAUD)-1 //Baud rate formula
#define TX_BUFFER_SIZE 128 //Buffer size
#define BPM 110 //Beats per minute
#define VS1053_RESET = 6;    // reset pin for the VS1053
float beatDuration = 60.0 / BPM * 1000; // duration of a beat in ms

// the melody sequence:
int melody[] = {64, 66, 71, 73, 74, 66, 64, 73, 71, 66, 74, 73};
// which note of the melody to play:
int noteCounter = 0;

void delay(int n) 
{
	while(n--) 
	{
		_delay_ms(1);
	}
}

// write data to TX
void write(uint8_t data)
{
	UDR0=data;
	_delay_ms(1);
}

// send a 3-byte midi message
void midiCommand(uint8_t cmd, uint8_t data1, uint8_t data2)
{
	write(cmd);     // command byte (should be > 127)
	write(data1);   // data byte 1 (should be < 128)
	write(data2);   // data byte 2 (should be < 128)
}

int main(void)
{
	/* setup serial */
	UBRR0H = (BRC >> 8); //The baud rate of UART/USART is set using the 16-bit wide UBRR register
	UBRR0L = BRC;        //
	UCSR0B = (1 << TXEN0); //Enable interrupt on receive and Enable transmitter
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //Select 8 bits
	
	delay(4000);
	
	midiCommand(0xC0, 0, 52); //Set instrument
	
    /* loop */
    while (1) 
    {
		midiCommand(0x90, melody[noteCounter], 0x7F);
		int noteDuration = beatDuration/4;
		delay(noteDuration);
		midiCommand(0x80, melody[noteCounter], 0);
		noteCounter++;
		noteCounter = noteCounter % 12;
    }
}
