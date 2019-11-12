#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>
#include <fstream>
#include <winhttp.h>
#include "ReverseMP3.h"
#pragma comment(lib, "winhttp.lib")
using namespace std;

bool checkChar(int c, char ch) {
	switch (c) {
		case +0: return ch == 'h';
		case +1: return ch == '2';
		case +2: return ch == '>';
		case +3: return ch == '<';
		case +4: return ch == 'a';
		case +5: return ch == ' ';
		case +6: return ch == 'h';
		case +7: return ch == 'r';
		case +8: return ch == 'e';
		case +9: return ch == 'f';
		case 10: return ch == '=';
		case 11: return ch == '"';
		case 12: return ch == 'h';
		case 13: return ch == 't';
		case 14: return ch == 't';
		case 15: return ch == 'p';
		case 16: return ch == 's' || ch == ':';
		case 17: return ch == ':' || ch == '/';
		case 18: return ch == '/';
		default: return ch != ',' && ch != ')';
	}
}

list<string *> *searchURLs(const string url, const int start = 0, const int end = 10) {

	char b[2];
	list<string *> result;
	auto str = new string();
	int l = 0, c = 0;
	char command[200];
	for (int s = start; s < end; s += 10) {

		sprintf_s(command, "%s%%26start=%d", url.c_str(), s);
		FILE *pipe = _popen(command, "r");
		while (fgets(b, 2, pipe) != NULL) {

			char ch = b[0];
			if (checkChar(c, ch)) {
				if (c >= 12)
					str->append(b), l++;
				c++;
			}

			else if (c > 13) {
				if (str->at(l - 1) == 'P') {
					str->resize(str->size() - 12);
					bool b = true;
					for (auto it = result.begin(); it != result.end(); it++)
						if (str->compare(**it) == 0) {
							b = false;
							break;
						};
					if (b)result.push_back(str);
				}
				str = new string();
				l = c = 0;
			}
		}
		_pclose(pipe);
	}
	cout << flush;
	return &result;
}

void func2(const wchar_t *url, const int start = 0, const int end = 10) {
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer;
	BOOL  bResults = FALSE;
	HINTERNET  hSession = NULL,
		hConnect = NULL,
		hRequest = NULL;

	hSession = WinHttpOpen(L"WinHTTP Example/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	wchar_t *wurlP = new wchar_t[200];
	auto s = sizeof(*wurlP);
	memcpy(wurlP, url, 200);
	size_t urlLen = wcslen(url);
	swprintf_s(wurlP + urlLen, 200 - urlLen, L"&start=%d", 1);
	//delete wurlP;

	if (hSession)
		hConnect = WinHttpConnect(hSession, wurlP,
			INTERNET_DEFAULT_HTTPS_PORT, 0);
	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"GET", NULL,
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE);
	if (hRequest)
		bResults = WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			WINHTTP_NO_REQUEST_DATA, 0,
			0, 0);

// End the request.
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);

	  // Keep checking for data until there is nothing left.
	if (bResults) {
		do {
		  // Check for available data.
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				printf("Error %u in WinHttpQueryDataAvailable.\n",
					GetLastError());

		  // Allocate space for the buffer.
			pszOutBuffer = new char[dwSize + 1];
			if (!pszOutBuffer) {
				printf("Out of memory\n");
				dwSize = 0;
			} else {
			  // Read the data.
				ZeroMemory(pszOutBuffer, dwSize + 1);

				if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
					dwSize, &dwDownloaded))
					printf("Error %u in WinHttpReadData.\n", GetLastError());
				else
					printf("%s", pszOutBuffer);

				  // Free the memory allocated to the buffer.
				delete[] pszOutBuffer;
			}
		} while (dwSize > 0);
	}


	// Report any errors.
	if (!bResults)
		printf("Error %d has occurred.\n", GetLastError());

	  // Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);
}

int main()
{
	reverseMP3("C:\\Users\\( USERNAME )\\Music\\Tiimy mono.mp3", "C:\\Users\\( USERNAME )\\Music\\Tiimy mess2.mp3");
	//func2(L"https://www.bing.com/search?q=roblox+filetype%3A.zip", 0, 30);
	return 0;
}

/*
list<char*> GetBingResultUrls(char* query, int start = 1, int end = 10)
{
	var ma = new List<Match>();
	for (int c = start; c <= end; c += 10)
		ma.AddRange(Regex.Matches(new StreamReader(cl.OpenRead("https://www.bing.com/search?q=" + Uri.
			EscapeUriString(query) + "&first=" + c)).ReadToEnd(), "<h2>\\s*<a href=\"(http.*?)\" h").Cast<Match>());
	return ma.Select(m = > m.Groups[1].Value).ToList().GetRange(0, Math.Min(ma.Count - 1, end - start));
}

list<char*> GetPooledAlbumResults(char* artist, char* album, int start = 1, int end = 20)
{
	var urls = GetBingResultUrls(string:: .Format("download \"{0}\" \"{1}\" contains:.zip", artist, album), start, end);
	return urls.OrderByDescending(s = > GetURLRank(s)).SelectMany(
		url = >
	{
		if (url.Contains("vk.com"))
			return new List<string>();
		try
		{
			return Regex.Matches(new StreamReader(cl.OpenRead(url)).ReadToEnd(),
				@"http.{7, 127}\.zip").Cast<Match>().Select(m => m.Value).Where(s => s.Length > 0);
		}
		catch (WebException) { return new List<string>(); }
	}).Distinct().Where(s = > s != null).OrderByDescending(s = > GetURLRank(s, artist, album)).ToList();
}
*/
