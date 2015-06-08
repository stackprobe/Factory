#include "C:\Factory\Common\Options\SClient.h"
#include "C:\Factory\Common\Options\SockStream.h"

#define COMMAND_READ_MESSAGE 1

#define SPEED_MIN 50
#define SPEED_MAX 300
#define TONE_MIN 50
#define TONE_MAX 200
#define VOLUME_MIN 0
#define VOLUME_MAX 100
#define VOICE_MIN 0
#define VOICE_MAX 8

#define CHARSET_SJIS 2

#define DEFAULT_MESSAGE "‚Ú‚¤‚æ‚Ýƒ“ƒS"

// ----

static char *ServerDomain = "localhost";
static uint ServerPortNo = 50001;
static sint16 Speed  = -1;
static sint16 Tone   = -1;
static sint16 Volume = -1;
static uint Voice = VOICE_MIN;
static char *Message = DEFAULT_MESSAGE;

static int Perform(int sock, uint prm_dummy)
{
	SockStream_t *ss = CreateSockStream(sock, 20);
	uint msgLen = strlen(Message);

	SockSendChar(ss, COMMAND_READ_MESSAGE >> 0 & 0xff);
	SockSendChar(ss, COMMAND_READ_MESSAGE >> 8 & 0xff);
	SockSendChar(ss, Speed  >> 0 & 0xff);
	SockSendChar(ss, Speed  >> 8 & 0xff);
	SockSendChar(ss, Tone   >> 0 & 0xff);
	SockSendChar(ss, Tone   >> 8 & 0xff);
	SockSendChar(ss, Volume >> 0 & 0xff);
	SockSendChar(ss, Volume >> 8 & 0xff);
	SockSendChar(ss, Voice  >> 0 & 0xff);
	SockSendChar(ss, Voice  >> 8 & 0xff);
	SockSendChar(ss, CHARSET_SJIS);
	SockSendChar(ss, msgLen >>  0 & 0xff);
	SockSendChar(ss, msgLen >>  8 & 0xff);
	SockSendChar(ss, msgLen >> 16 & 0xff);
	SockSendChar(ss, msgLen >> 24 & 0xff);
	SockSendToken(ss, Message);
	SockFlush(ss);

	ReleaseSockStream(ss);

	return 1;
}
int main(int argc, char **argv)
{
	int retval;

readArgs:
	if(argIs("/SD"))
	{
		ServerDomain = nextArg();
		goto readArgs;
	}
	if(argIs("/SP"))
	{
		ServerPortNo = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/S"))
	{
		Speed = atoi(nextArg());
		goto readArgs;
	}
	if(argIs("/T"))
	{
		Tone = atoi(nextArg());
		goto readArgs;
	}
	if(argIs("/V"))
	{
		Volume = atoi(nextArg());
		goto readArgs;
	}
	if(argIs("/K"))
	{
		Voice = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/M"))
	{
		Message = nextArg();
		goto readArgs;
	}

	cout("ServerDomain: %s\n", ServerDomain);
	cout("ServerPortNo: %u\n", ServerPortNo);
	cout("Speed: %d\n", Speed);
	cout("Tone: %d\n", Tone);
	cout("Volume: %d\n", Volume);
	cout("Voice: %d\n", Voice);

	if(m_isEmpty(ServerDomain))
		return;

	if(!m_isRange(ServerPortNo, 1, 65535))
		return;

	if(Speed != -1 && (Speed < SPEED_MIN || SPEED_MAX < Speed))
		return;

	if(Tone != -1 && (Tone < TONE_MIN || TONE_MAX < Tone))
		return;

	if(Volume != -1 && (Volume < VOLUME_MIN || VOLUME_MAX < Volume))
		return;

	if(!m_isRange(Voice, VOICE_MIN, VOICE_MAX))
		return;

	if(m_isEmpty(Message))
		return;

	retval = SClient(ServerDomain, ServerPortNo, Perform, 0);

	cout("%s\n", retval ? "¬Œ÷" : "Ž¸”s");
}
