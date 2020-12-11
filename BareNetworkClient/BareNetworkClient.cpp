// BareNetworkClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>

#include <windows.h>
#include "LinuxConnector.h"

int main() {

	PasswordFilterAccount pf = {};
	WCHAR* uname, * pwd;
	//int size_needed = WideCharToMultiByte(CP_UTF8, 0, User, (int)(pf->AccountName->Length), NULL, 0, NULL, NULL);
	const char* u = "wmotycka";
	int u_size_needed = MultiByteToWideChar(CP_UTF8, 0, u, strlen(u)+1, NULL, 0);
	uname = new WCHAR[u_size_needed];
	MultiByteToWideChar(CP_UTF8, 0, u, strlen(u)+1, uname, u_size_needed);
	//uname[size_needed] = 0;
	int p_size_needed = MultiByteToWideChar(CP_UTF8, 0, "default-pw", 11, NULL, 0);
	pwd = new WCHAR[p_size_needed];
	MultiByteToWideChar(CP_UTF8, 0, "default-pw", 11, pwd, p_size_needed);
	//pwd[size_needed] = 0;
	UNICODE_STRING u_acct;
	UNICODE_STRING u_pw;
	u_acct.Buffer = uname;
	u_acct.Length = wcslen(uname);
	u_acct.MaximumLength = wcslen(uname);
	pf.AccountName = &u_acct;
	u_pw.Buffer = pwd;
	u_pw.Length = wcslen(pwd);
	u_pw.MaximumLength = wcslen(pwd);
	pf.Password = &u_pw;
	LinuxConnector(&pf);
	return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
