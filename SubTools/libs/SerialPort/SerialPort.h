#pragma once

#include "C:\Factory\Common\all.h"

typedef struct SerialPort_st
{
	HANDLE Handle;
}
SerialPort_t;

SerialPort_t *OpenSerialPort(uint portNo);
void CloseSerialPort(SerialPort_t *i);

void SetSerialPortState(SerialPort_t *i, int sync, int data);
void GetSerialPortState(SerialPort_t *i, int *p_sync, int *p_data);
