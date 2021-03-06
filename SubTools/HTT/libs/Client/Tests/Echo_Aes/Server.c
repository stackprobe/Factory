/*
	Service.dat に以下を登録すること。

		EchoTest_Aes C:\Factory\SubTools\HTT\libs\Client\Test\Echo_Aes\Server.exe
*/

#include "C:\Factory\SubTools\HTT\libs\CRPC_Aes.h"

autoBlock_t *CRPC_GetRawKey(void)
{
	return makeBlockHexLine("6d636a95a420a7780639a2a1d13eacb4"); // .\\Client.c と合わせること。
}
void CRPC_Init(void)
{
	// noop
}
autoBlock_t *ServiceCRPC(autoBlock_t *recvData, uint phase)
{
	autoBlock_t *sendData = newBlock();

	ab_addLine(sendData, "You sent [");
	addBytes(sendData, recvData);
	ab_addLine(sendData, "]");

	RPC_SendCompleteAndDisconnect();
	return sendData;
}
