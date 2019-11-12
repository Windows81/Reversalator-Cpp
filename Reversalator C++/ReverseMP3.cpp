#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <bitset>
#include "ReverseMP3.h"
using namespace std;
const unsigned short bitRates[] = {
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
streampos START, END;
ifstream IFSTREAM;
ofstream OFSTREAM;

void initVariables(string ifN, string ofN) {
	IFSTREAM; IFSTREAM.open(ifN, ios::binary | ios::ate);
	OFSTREAM; OFSTREAM.open(ofN, ios::binary);
	START = 0, END = IFSTREAM.tellg();
	IFSTREAM.seekg(0);
	firstFrame = 0;
}

void pipe(size_t size, void process(char *, size_t) = NULL) {
	char *b = (char *)malloc(size);
	IFSTREAM.read(b, size);
	if (process != NULL)
		process(b, size);
	OFSTREAM.write(b, size);
	free(b);
}

struct FrameParams {
	bool Stereo;
	unsigned Length;
	bool Protection;
	bool Channels[2];
} frameParams;

void removeCRC(char *pos, const size_t size) {
	if (pos[0] % 2 == 0) {
		memmove(pos + 4, pos + 6, size - 4);
		memset(pos + size - 4, 0, 4);
		pos[0] |= 1, pos += 2;
	}
}

template<const size_t size>
bitset<size> getBitset(char *data) {
	bitset<size> bs = bitset<size>();
	unsigned char ch;
	for (size_t c = 0; c < size * 8; c++) {
		ch = (unsigned char)data[c];
		for (c += 7; c % 8 == 0; c--, ch >>= 1)
			bs[c] = ch % 2;
	}
	return bs;
}

void processFrame(char *data, const size_t size) {

	char *pos = data;
	removeCRC(pos, size);
	auto bitSet = getBitset<size>(data);
	//char main_data_begin[8] = (char[])data;
	bool stereo = frameParams.Channels[0] && !frameParams.Channels[1];
	char main_data_begin = pos[0];
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
		pipe((size_t)START);
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

void iterateFrames(bool reverse = true) {
	char data[1441];
	streampos rem = END;
	unsigned int frame = 0;
	unsigned short bytes = 0;
	if (!reverse)OFSTREAM.seekp(START);
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
		rem -= (frameParams.Length = bytes = 1440 *
			bitRates[br] / sampleRates[sr] + pd);
		memcpy(frameParams.Channels, data + 24, 2);

		IFSTREAM.seekg(-4, ios::cur);
		if (reverse)OFSTREAM.seekp(rem);
		pipe(bytes, processFrame);
	} while (IFSTREAM.tellg() < END);
	cout << rem << ' ' << firstFrame << endl;
}

void reverseMP3(string ifN, string ofN) {
	initVariables(ifN, ofN);
	processID3();
	iterateFrames(false);
	IFSTREAM.close();
	OFSTREAM.close();
}