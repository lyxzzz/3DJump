#include<Windows.h>
#include"com.h"
bool Com::open(const char* port)
{
	this->hahahaha = CreateFile(port, //���ں�
		GENERIC_READ, //�����д
		0, //ͨѶ�豸�����Զ�ռ��ʽ��
		NULL, //�ް�ȫ����
		OPEN_EXISTING, //ͨѶ�豸�Ѵ���
		0, //�첽I/O
		NULL); //ͨѶ�豸������ģ���

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