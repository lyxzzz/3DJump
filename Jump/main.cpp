#include<Windows.h>
#include<iostream>
using namespace std;
HANDLE hCom; //ȫ�ֱ��������ھ��
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
	hCom = CreateFile("COM4",//COM1��
		GENERIC_READ, //�������д
		0, //��ռ��ʽ
		NULL,
		OPEN_EXISTING, //�򿪶����Ǵ���
		0, //ͬ����ʽ
		NULL);
	if (hCom == (HANDLE)-1)
	{
		cout << "Open Com Error" << endl;
		return FALSE;
	}
	return TRUE;
}
