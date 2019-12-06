#include "..\SerialPort.h"

int main(int argc, char **argv)
{
	SerialPort_t *i = OpenSerialPort(1);
	int sync;
	int data;

	SetSerialPortState(i, 1, 0);
	GetSerialPortState(i, &sync, &data);

	cout("%d %d\n", sync, data);

	CloseSerialPort(i);
}
