#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <codecvt>
#include <time.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <comdef.h>
#include <Wbemidl.h>
#include <comutil.h>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <WinCred.h>
#include <Windows.h>
#include <shobjidl.h> 

#include <tlhelp32.h>
#include <tchar.h>

#pragma comment(lib, "Credui.lib")

#include "Include/nlohmann/json.hpp"
#include "Include/WinHttpClient.h"
#include "sha256.cpp"

using namespace std;
using json = nlohmann::json;

void throwError(LPCSTR s)
{
	MessageBoxA(0, s, "Auth Error", MB_OK);
	system("pause");
	exit(EXIT_FAILURE);
}


string DisplayLoginDialog()
{

	BOOL save = false;
	DWORD authPackage = 0;
	LPVOID authBuffer;
	ULONG authBufferSize = 0;

	CREDUI_INFO credUiInfo;

	static WCHAR username[CREDUI_MAX_USERNAME_LENGTH * sizeof(WCHAR)] = { 0 };
	static WCHAR password[CREDUI_MAX_PASSWORD_LENGTH * sizeof(WCHAR)] = { 0 };
	DWORD uLen = CREDUI_MAX_USERNAME_LENGTH;
	DWORD pLen = CREDUI_MAX_PASSWORD_LENGTH;

	credUiInfo.pszCaptionText = TEXT("Cracked.io Authentication");
	credUiInfo.pszMessageText = TEXT("Please enter your Auth Key in \"Password\". Username doesn't matter");
	credUiInfo.cbSize = sizeof(credUiInfo);
	credUiInfo.hbmBanner = NULL;
	credUiInfo.hwndParent = NULL;

	HRESULT rc = CredUIPromptForWindowsCredentials(&(credUiInfo), 0, &(authPackage), NULL, 0, &authBuffer, &authBufferSize, &(save), CREDUIWIN_GENERIC);

	if (rc == ERROR_SUCCESS)
	{
		CredUnPackAuthenticationBufferW(0, authBuffer, authBufferSize, username, &uLen, NULL, 0, password, &pLen);

		wstring ws(password);
		string res(ws.begin(), ws.end());
		return res;
	}

	return "";
}

void Authenticate() {
	SetConsoleTitleA("Cracked.io Authentification");

	vector<string> textLines;

	ifstream inputFile("key.crackedto");

	string key;

	if (inputFile)
	{
		cout << "Getting auth key from key.crackedto..." << endl;
		while (getline(inputFile, key))
		{
			textLines.push_back(key);
		}
		inputFile.close();
	}
	else {
		key = DisplayLoginDialog();
	}


	while (key.empty())
	{
		key = DisplayLoginDialog();
	}

	DWORD serNumb;
	GetVolumeInformation(_T("C:\\"), nullptr, 0, &serNumb, nullptr, nullptr, nullptr, 0);


	string line;
	bool found = false;
	bool fucker = false;
	ifstream infile("C:\\Windows\\System32\\drivers\\etc\\hosts");
	if (infile.is_open())
	{
		while (getline(infile, line))
		{
			if (line.find("cracked.io") != string::npos || line.find("cracked.to") != string::npos)
			{
				fucker = true;
				break;
			}
		}
		infile.close();
	}
	if (fucker)
	{
		exit(-1);
	}

	string hwid = to_string(serNumb);

	if (hwid.find("Error:") != std::string::npos) {
		throwError(hwid.c_str());
	}

	ostringstream tempShit;
	tempShit << "a=auth&k=" << key << "&hwid=" << hwid;

	string postData = tempShit.str();

	tempShit.clear();

	string response;

	WinHttpClient client(L"https://cracked.io/auth.php");

	client.SetAdditionalDataToSend((BYTE*)postData.c_str(), postData.size());

	wchar_t szSize[50] = L"";
	swprintf_s(szSize, L"%d", postData.size());
	wstring headers = L"Content-Length: ";
	headers += szSize;
	headers += L"\r\nContent-Type: application/x-www-form-urlencoded\r\nUser-Agent: Mozilla/5.0 (compatible; Program/1.0.0; Auth)\r\n";
	client.SetAdditionalRequestHeaders(headers);

	client.SendHttpRequest(L"POST");

	wstring httpResponseContent = client.GetResponseContent();
	static std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t > converter;
	response = converter.to_bytes(httpResponseContent);

	char outstr[200];
	time_t t;
	struct tm* tmp;
	const char* fmt = "%F %R";

	t = time(NULL);
	tmp = gmtime(&t);
	if (tmp == NULL) {
		throwError("Error CRCK105: gmtime error");
	}

	if (strftime(outstr, sizeof(outstr), fmt, tmp) == 0) {
		throwError("Error CRCK106: strftime returned 0");
	}

	string unixTime = outstr;

	json dictionary = json::parse(response);

	if (response.find("error") != string::npos) {

		MessageBoxA(0, (dictionary["error"].get<string>()).c_str(), "Cracked.to Auth", MB_OK);
		Authenticate();
		return;
	}
	else {
		string grps[13] = { "11", "12", "96", "97", "99", "100", "101", "4", "3", "6", "94", "92", "93" };

		if (find(begin(grps), end(grps), dictionary["group"].get<string>()) != end(grps)) {

			fstream fs;
			fs.open("key.crackedto", fstream::out);
			fs << key;
			fs.close();
			MessageBoxA(0, ("Welcome " + dictionary["username"].get<string>()).c_str(), "Cracked.to Auth", MB_OK);
			Sleep(2000);
			system("cls");

		}
		else {
			MessageBoxA(0, "You need premium+ for this tool!", "Cracked.to Auth", MB_OK);
			exit(-1);
		}
	}
}

int main()
{
    Authenticate();
}
