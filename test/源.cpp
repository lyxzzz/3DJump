#include<stdlib.h>
#include<iostream>
#include<time.h>
#include<Windows.h>
#include<fstream>
#include<time.h>
using namespace std;
HANDLE hCom;
#define OFFSET 0.001
#define RANGE 32768
struct message
{
	char buf[200];
};
bool open(const char* port);
bool add = true;
float f = -1.0f;
int i = -RANGE;
bool addi = true;
float getfloat()
{
	f += add ? OFFSET : -OFFSET;
	if (f < -1.0f) {
		f = -1.0;
		add = true;
	}
	if (f > 1.0f)
	{
		f = 1.0f;
		add = false;
	}
	return f;
}
int getint()
{
	i += add ? 1 : -1;
	if (i < -RANGE) {
		i = -RANGE;
		add = true;
	}
	if (i > RANGE)
	{
		i = RANGE;
		add = false;
	}
	return i;
}
DWORD read(message* m);
float getRand()
{
	return float(rand() / float(RAND_MAX));
}
int main()
{
	ofstream out("C:\\Files\\Code\\C++\\Jump\\Jump\\test\\testfile",ios::out);
	srand(time(NULL));
	for (size_t i = 0; i < 5000; ++i)
	{
		out << 15 << ";" << (getfloat()*75.0f) << endl;
	}
	getchar();

}
bool open(const char* port)
{
	hCom = CreateFile(port, //���ں�
		GENERIC_READ, //�����д
		0, //ͨѶ�豸�����Զ�ռ��ʽ��
		NULL, //�ް�ȫ����
		OPEN_EXISTING, //ͨѶ�豸�Ѵ���
		0, //�첽I/O
		NULL); //ͨѶ�豸������ģ���

	if (hCom == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hCom);
		return FALSE;
	}
	else
		return true;
}
DWORD read(message* m)
{
	DWORD bytes = 0;
	ReadFile(hCom, (void *)&m, sizeof(message), &bytes, NULL);
	return bytes;
}
