#include<Windows.h>
#include<iostream>
using namespace std;
HANDLE hCom; //全局变量，串口句柄
struct message {
	char value;
};
bool openCOM();
int haha()
{
	openCOM();
	message m;
	DWORD readbytes=0;
	while (true)
	{
		cout << ReadFile(hCom, (void *)&m, sizeof(message), &readbytes, NULL) << endl;
		cout << (short)m.value << " " << readbytes << endl;
		Sleep(1000);
	}
}
bool openCOM()
{
	hCom = CreateFile("COM4",//COM1口
		GENERIC_READ, //允许读和写
		0, //独占方式
		NULL,
		OPEN_EXISTING, //打开而不是创建
		0, //同步方式
		NULL);
	if (hCom == (HANDLE)-1)
	{
		cout << "Open Com Error" << endl;
		return FALSE;
	}
	return TRUE;
}
