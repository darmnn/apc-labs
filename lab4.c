#include <stdio.h>
#include <conio.h>
#include <dos.h>

#define BASE_FREQUENCY 1193180
#define NUMBER_OF_NOTES 9

void playSound();
void showStatesWords();
void showDivisionsRatios();
void getRandomNumber();

void main() {
	clrscr();
	while(1) {
		printf("1 - Play sound;\n"
			"2 - Show states words;\n"
			"3 - Show divisions ratios;\n"
			"4 - Get random number;\n"
			"0 - Exit.\n");
		switch (getch()) {
		case '1':
			playSound();
			clrscr();
			break;
		case '2':
			clrscr();
			showStatesWords();
			break;
		case '3':
			clrscr();
			showDivisionsRatios();
			break;
		case '4':
			clrscr();
			getRandomNumber();
			break;
		case '0':
			clrscr();
			return;
		default:
			clrscr();
		}
	}
}


void playSound() {
	int frequency[NUMBER_OF_NOTES] = {392, 392, 392, 329, 349, 349, 349, 293, 293};
	int duration[NUMBER_OF_NOTES] = {400, 400, 400, 1000, 400, 400, 400, 800, 800};
	int divisionRatio;
	for (int i = 0; i < NUMBER_OF_NOTES; i++) {
		if (frequency[i] == 0) {
			delay(duration[i]);
			continue;
		}
		outp(0x61, inp(0x61) | 3);  //turn ON
		outp(0x43, 0xB6); 
		
		divisionRatio = BASE_FREQUENCY / frequency[i];
		outp(0x42, divisionRatio % 256); //Low
		divisionRatio /= 256;
		outp(0x42, divisionRatio); //High
		delay(duration[i]);
		outp(0x61, inp(0x61) & 0xFC); //turn OFF
	}
	delay(50);
}

void showStatesWords() {
	clrscr();
	unsigned char state, stateWord[9];
	int ports[] = {0x40, 0x41, 0x42}; 
	int controlWords[] = { 0xE2, 0xE4, 0xE8 };

	stateWord[8] = '\0';
	for (int iChannel = 0; iChannel < 3; iChannel++) {
		outp(0x43, controlWords[iChannel]); //select a channel
		state = inp(ports[iChannel]);  
		for (int i = 7; i >= 0; i--) {
			stateWord[i] = state % 2 + '0';
			state /= 2;
		}
		printf("Channel %d: %s \n", iChannel, stateWord);
	}
}

void showDivisionsRatios() {
	int ports[] = { 0x40, 0x41, 0x42 };
	int controlWords[] = { 0x00, 0x40, 0x80 };
	
	unsigned  divisionRatio, maxDivisionRatio, divisionRatioHigh, divisionRatioLow;
	printf("Division ratio:\n");

	for (int iChannel = 0; iChannel < 3; iChannel++) {
		divisionRatio = 0;
		maxDivisionRatio = 0;

		if (iChannel == 2) {
			outp(0x61, inp(0x61) | 1); //turn ON for kd2
		}

		for (unsigned long j = 0; j < 1000000; j++) {
			outp(0x43, controlWords[iChannel]); //select a channel
			divisionRatioLow = inp(ports[iChannel]);
			divisionRatioHigh = inp(ports[iChannel]);
			divisionRatio = divisionRatioHigh * 256 + divisionRatioLow;
			if (divisionRatio > maxDivisionRatio) {
				maxDivisionRatio = divisionRatio;
			}
		}

		if (iChannel == 2) {
			outp(0x61, inp(0x61) & 0xFC); ////turn OFF for kd2
		}

		printf("Channel %d - %u\n", iChannel, maxDivisionRatio);
	}
}



void getRandomNumber() {
	unsigned int limit = 65535;
	unsigned int number, numberLow, numberHigh;
	clrscr();
	outp(0x43, 0xB4);
	
	outp(0x42, limit % 256); //Low
	limit /= 256;
	outp(0x42, limit); //High
	outp(0x61, inp(0x61) | 1);  //turn ON without sound
	do {
		printf("1 - Set random limit;\n"
			"2 - Get a number;\n"
			"0 - Exit.\n");
		fflush(stdin);
		switch (getch()) {
		case '1':
			do {
				printf("Enter number limit: from 1 to 65535\n");
				fflush(stdin);
			} while (!scanf("%u", &limit));
			outp(0x43, 0xB4);
			outp(0x42, limit % 256); //Low
			limit /= 256;
			outp(0x42, limit); //High
			break;
		case '2':
			printf("Random number:\n");
			outp(0x43, 0x80); //select a channel
			numberLow = inp(0x42);
			numberHigh = inp(0x42);
			number = numberHigh * 256 + numberLow;
			printf("Number %u\n\n", number);
			break;
		case '0':
			outp(0x61, inp(0x61) & 0xFC); //turn OFF
			clrscr();
			return;
		}
	} while (1);
}
