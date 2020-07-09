#include <RF24.h>
#include <SPI.h>	   // SPI Library for nRF24L01
#include "nRF24L01.h"  // Radio hardware library
#include "RF24Audio.h" // Audio library
#include "printf.h"	   // General includes for radio and audio lib

const byte btnVoiceToggle = 2;
const byte btnSerialRxToggle = 3;

const byte ledVoiceToggle = 4;
const byte ledSerialRxToggle = 5;

/*
	Initializing radio and audio hardwares
*/

RF24 radio(7, 8);			 // Set radio up using pins 7 (CE) 8 (CS)
RF24Audio rfAudio(radio, 1); // Set up the audio using the radio, and set to radio number 0

volatile byte voiceState = LOW;

// For Serial Out
const uint64_t addresses[14] = {0xABCDABCD71LL, 0x544d52687CLL,
								0x544d526832LL, 0x544d52683CLL,
								0x544d526846LL, 0x544d526850LL,
								0x544d52685ALL, 0x544d526820LL,
								0x544d52686ELL, 0x544d52684BLL,
								0x544d526841LL, 0x544d526855LL,
								0x544d52685FLL, 0x544d526869LL};

void StartAudioStransmission()
{
	digitalWrite(ledVoiceToggle, HIGH);
	rfAudio.transmit();
	rfAudio.broadcast(1);
	return;
}

void EndAudioTransmission()
{
	rfAudio.receive();
	digitalWrite(ledVoiceToggle, LOW);
	return;
}

void sendDebugString(String buttonState)
{
	const String buttonString = ".<inpt>0000000000000000</inpt>.";

	char buttonStateBuff[32];
	buttonString.toCharArray(buttonStateBuff, 32);

	rfAudio.transmit();

	if (radio.write(buttonStateBuff, 32))
	{
		Serial.println("Signal Sending Success: " + String(buttonStateBuff) + "Size: " + sizeof(buttonStateBuff));
	}
	else
	{
		Serial.println("Signal Sending Failed: " + String(buttonStateBuff) + "Size: " + sizeof(buttonStateBuff));
	}

	// Closing transmission mode
	rfAudio.receive();
}

void readDebugString()
{
	byte audioData[32];			// Set up a buffer for the received data
	byte samplesToDisplay = 32; // Change this to 32 to send the entire payload over USB/Serial
	while (radio.available())
	{
		digitalWrite(ledSerialRxToggle, HIGH);
		radio.read(&audioData, 32);

		for (int i = 0; i < samplesToDisplay; i++)
		{
			Serial.write(audioData[i]);
		}
		Serial.println();
	}
	digitalWrite(ledSerialRxToggle, LOW);
}


void audioToggle()
{
	voiceState = !voiceState;
	
	if (voiceState)
	{
		StartAudioStransmission();
	}
	else
	{
		EndAudioTransmission();
	}
}

void setup()
{

	pinMode(btnVoiceToggle, INPUT);
	pinMode(btnSerialRxToggle, INPUT);

	pinMode(ledVoiceToggle, OUTPUT);
	pinMode(ledSerialRxToggle, OUTPUT);

	digitalWrite(ledVoiceToggle, LOW);
	digitalWrite(ledSerialRxToggle, LOW);

	attachInterrupt(digitalPinToInterrupt(btnVoiceToggle), audioToggle, CHANGE);
	attachInterrupt(digitalPinToInterrupt(btnSerialRxToggle), readDebugString, CHANGE);

	printf_begin();	 // Radio library uses printf to output debug info
	radio.begin();	 // Must start the radio here, only if we want to print debug info
	rfAudio.begin(); // Start up the radio and audio libraries

	radio.printDetails();

	rfAudio.receive();
}

void loop()
{
}