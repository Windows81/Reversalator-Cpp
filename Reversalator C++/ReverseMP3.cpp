#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "ReverseMP3.h"
using namespace std;
const unsigned char bitRates[] = {
	32,
	40,
	48,
	56,
	64,
	80,
	96,
	112,
	128,
	160,
	192,
	224,
	256,
	320,
};
const unsigned short sampleRates[] = {
	441,
	480,
	320,
};

unsigned firstFrame;
unsigned START, END;
ifstream IFSTREAM;
ofstream OFSTREAM;

void initVariables(string ifN, string ofN) {
	IFSTREAM; IFSTREAM.open(ifN, ios::binary | ios::ate);
	OFSTREAM; OFSTREAM.open(ofN, ios::binary);
	START = 0, END = IFSTREAM.tellg();
	IFSTREAM.seekg(0);
	firstFrame = 0;
}

void pipe(size_t size) {
	char *b = (char *)malloc(size);
	IFSTREAM.read(b, size);
	OFSTREAM.write(b, size);
	free(b);
}

void processID3() {
	char *a1 = (char *)malloc(10);
	IFSTREAM.read(a1, 10);
	IFSTREAM.seekg(0);
	if (a1[0] == 'I') {
		START =
			(a1[6] << 21) +
			(a1[7] << 14) +
			(a1[8] << 7) +
			a1[9] + 10;
		pipe(START);
	}
	free(a1);

	//Gets the size then goes to the start.
	IFSTREAM.seekg(-128, ios::end);
	char tagFlag[4] = {}, endData[128];
	IFSTREAM.read(tagFlag, 3);
	if (strcmp(tagFlag, "TAG") == 0) {
		memcpy(endData, tagFlag, 3);
		IFSTREAM.read(endData + 3, 125);
		OFSTREAM.seekp(END -= 128);
		OFSTREAM.write(endData, 128);
	}
	IFSTREAM.seekg(START);
}

void reverseMP3(string ifN, string ofN) {
	initVariables(ifN, ofN);
	processID3();

	//Now time for the samplezzz!!!
	unsigned frame = 0, rem = END;
	char data[1441];
	unsigned short bytes = 0;
	do {
		frame++;
		IFSTREAM.read(data, 4);
		if (rem == firstFrame)break;
		else if (data[0] != -1)
			throw exception("You f×××ing messed up.");

		auto rateInfo = data[2];
		auto br = ((rateInfo + 256) >> 4) - 1;
		auto sr = ((rateInfo + 256) >> 2) % 4;
		bool pd = ((rateInfo + 256) >> 1) % 2;
		rem -= (bytes = 1440 * bitRates
			[br] / sampleRates[sr] + pd);

		IFSTREAM.seekg(-4, ios::cur);
		OFSTREAM.seekp(rem);
		pipe(bytes);
	} while (IFSTREAM.tellg() < END);
	cout << rem << ' ' << firstFrame << endl;
	IFSTREAM.close();
	OFSTREAM.close();
}

void ReverseMP3()
{
}

void ReverseMP3(std::string s)
{
}

void ReverseMP3(std::string i, std::string o)
{
}
