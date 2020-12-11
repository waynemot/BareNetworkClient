#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
//#include <stdlib.h>
// #include <stdio.h>
// #include <iostream>
#include <SubAuth.h>
struct PasswordFilterAccount {
    PUNICODE_STRING AccountName;
    PUNICODE_STRING FullName;
    PUNICODE_STRING Password;
};

unsigned int __stdcall LinuxConnector(void * params);
char* getUser(void* params);
char* getPwd(void* params);