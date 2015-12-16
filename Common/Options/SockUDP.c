#include "SockUDP.h"

static uchar *GetDefIP(char *domain)
{
	static uchar ip[4];

	memset(ip, 0x00, 4);
	return ip;
}
int sockUDPOpenSend(uchar ip[4], char *domain, uint portno)
{
	char *strip;
	int sock;
	struct sockaddr_in sa;
	int retval;

	if(!ip)
		ip = GetDefIP(domain);

	if(!*(uint *)ip) // ? 0.0.0.0
	{
		errorCase(!domain);
		sockLookup(ip, domain);

		if(!*(uint *)ip) return -1;
	}
	strip = SockIp2Line(ip);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	errorCase(sock == -1);
	SockPostOpen(sock);

	memset(&sa, 0x00, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(strip);
	sa.sin_port = htons((unsigned short)portno);

	return sock;
}
int sockUDPOpenRecv(uint portno)
{
	int retval;
	int sock;
	struct sockaddr_in sa;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	errorCase(sock == -1);
	SockPostOpen(sock);

	memset(&sa, 0x00, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons((unsigned short)portno);

	retval = bind(sock, (struct sockaddr *)&sa, sizeof(sa));
	errorCase(retval != 0); // ? == -1

	return sock;
}
void sockUDPSend(int sock, autoBlock_t *block)
{
	// TODO
}
autoBlock_t *sockUDPRecv(int sock, uint szMax)
{
	return NULL; // TODO
}
void sockUDPClose(int sock)
{
	int retval;

	SockPreClose(sock);
	retval = closesocket(sock);
	errorCase(retval);
}
