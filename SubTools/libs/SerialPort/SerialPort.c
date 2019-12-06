#include "SerialPort.h"

SerialPort_t *OpenSerialPort(uint portNo)
{
	char *portName;
	HANDLE hdl;
	SerialPort_t *i;

	errorCase(!m_isRange(portNo, 1, 9));

	portName = xcout("COM%u", portNo);

	hdl = CreateFile(
		portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	memFree(portName);

	errorCase(hdl == INVALID_HANDLE_VALUE);

	i = (SerialPort_t *)memAlloc(sizeof(SerialPort_t));
	i->Handle = hdl;
	return i;
}
void CloseSerialPort(SerialPort_t *i)
{
	handleClose((uint)i->Handle);
	memFree(i);
}

void SetSerialPortState(SerialPort_t *i, int sync, int data)
{
	DCB dcb;

	zeroclear(dcb);
	dcb.DCBlength = sizeof(DCB);

	dcb.fDtrControl = m_01(sync);
	dcb.fRtsControl = m_01(data);

	SetCommState(i->Handle, &dcb);
}
void GetSerialPortState(SerialPort_t *i, int *p_sync, int *p_data)
{
	DCB dcb;

	zeroclear(dcb);
	dcb.DCBlength = sizeof(DCB);

	SetCommState(i->Handle, &dcb);

	*p_sync = (int)dcb.fDtrControl;
	*p_data = (int)dcb.fRtsControl;
}
