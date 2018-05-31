#pragma once
struct message
{
	char buf[200];
};
class Com
{
private:
	void* hahahaha;
public:
	Com() {}
	bool open(const char* port);
	unsigned long read(message* m);
};