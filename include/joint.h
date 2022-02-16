#ifndef __JOINT_H__
#define __JOINT_H__

#include <iostream>

using namespace std;

char *OTHER;
char *WHOLE;
std::string head = "aplay -D plughw:CARD=Device,DEV=0 ";

std::string audio_path = "";

//字符串与字符数组拼接
char *join(std::string b, char *s2)
{
	char s1[600] = "";
	try
	{
		strcpy(s1, b.c_str());
	}
	catch (...)
	{
		cout << ">>>>>join拷贝失败" << endl;
	}
	char *result = (char *)malloc(strlen(s1) + strlen(s2) + 1);
	if (result == NULL)
		exit(1);

	try
	{
		strcpy(result, s1);
		strcat(result, s2);
	}
	catch (...)
	{
		cout << ">>>>>join拷贝失败" << endl;
	}
	return result;
}

#endif