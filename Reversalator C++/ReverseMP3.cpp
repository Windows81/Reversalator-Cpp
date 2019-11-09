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
	ifstream IFSTREAM; IFSTREAM.open(ifN, ios::binary | ios::ate);
	ofstream OFSTREAM; OFSTREAM.open(ofN, ios::binary);
	START = 0, END = IFSTREAM.tellg();
	firstFrame = 0;
}

void processID3() {
	char a1[10];
	IFSTREAM.read(a1, 10);
	if (a1[0] == 'I') {
		firstFrame =
			(a1[6] << 21) +
			(a1[7] << 14) +
			(a1[8] << 7) +
			a1[9] + 10;
		char *a2 = new char[firstFrame];
		IFSTREAM.seekg(0);
		IFSTREAM.read(a2, firstFrame);
		OFSTREAM.write(a2, firstFrame);
		delete[]a2;
	}

	//Gets the size then goes to the start.
	IFSTREAM.seekg(-128, ios::cur);
	char tagFlag[5];
	IFSTREAM.read(tagFlag, 4);
	tagFlag[4] = 0;
	if (strcmp(tagFlag, "TAGT") == 0)
		END -= 128;
	IFSTREAM.seekg(0);
}

void pipe(size_t size) {
	char *b = (char *)malloc(size);
	IFSTREAM.read(b, size);
	OFSTREAM.write(b, size);
	free(b);
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
		else if (data[0] != -1) {
			throw exception();
		}
		auto rateInfo = data[2 + bytes];
		auto br = ((rateInfo + 256) >> 4) - 1;
		auto sr = ((rateInfo + 256) >> 2) % 4;
		bool pd = ((rateInfo + 256) >> 1) % 2;
		bytes = 1440 * bitRates[br] / sampleRates[sr] + pd;
		IFSTREAM.seekg(bytes - 4, ios::cur);
		OFSTREAM.seekp(rem -= bytes);
		pipe(bytes);
	} while (!IFSTREAM.eof());
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
