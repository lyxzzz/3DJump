#include<Windows.h>
#include"com.h"
bool Com::open(const char* port)
{
	this->hahahaha = CreateFile(port, //串口号
		GENERIC_READ, //允许读写
		0, //通讯设备必须以独占方式打开
		NULL, //无安全属性
		OPEN_EXISTING, //通讯设备已存在
		0, //异步I/O
		NULL); //通讯设备不能用模板打开

	if (this->hahahaha == INVALID_HANDLE_VALUE)
	{
		CloseHandle(this->hahahaha);
		return FALSE;
	}
	else
		return true;
}
DWORD Com::read(message* m)
{
	DWORD bytes = 0;
	ReadFile(this->hahahaha, (void *)&m, sizeof(message), &bytes, NULL);
	return bytes;
}