/*
	not work orz
*/

#pragma comment(lib, "user32.lib")

#include "C:\Factory\Common\all.h"

#pragma data_seg(".shareddata")

static HHOOK H_CWP;
static HHOOK H_CWPR;
static HHOOK H_GM;
static HHOOK H_SMF;

#pragma data_seg()

static void Listener(int command, HWND hwnd, uint prm)
{
	cout("%c %08x %08x\n", command, hwnd, prm);

	if(command == 'C')
	{
		HCURSOR cursor = GetCursor();

		cout("cursor=%08x\n", (uint)cursor);
	}
}

static void Monitor(int code, WPARAM prm_wParam, LPARAM prm_lParam)
{
	CWPSTRUCT *info;
	uint message;
	HWND hwnd;
	WPARAM wParam;
	LPARAM lParam;
	RECT rect;
	HANDLE prop;
LOGPOS();

	if(code != HC_ACTION)
		return;

	info    = (CWPSTRUCT *)prm_lParam;
	message = info->message;
	hwnd    = info->hwnd;
	wParam  = info->wParam;
	lParam  = info->lParam;

	switch(message)
	{
	case WM_NCPAINT:
	case WM_NCACTIVATE:
		Listener('B', hwnd, message); // NotifyWindowBorder
		break;

	case BM_SETCHECK:
	case BM_SETSTATE:
	case EM_SETSEL:
	case WM_CHAR:
	case WM_ENABLE:
	case WM_KEYUP:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_PALETTECHANGED:
	case WM_RBUTTONUP:
	case WM_SYSCOLORCHANGE:
	case WM_SETTEXT:
	case WM_SETFOCUS:
	    Listener('A', hwnd, message); // NotifyWindowClientArea
	    break;

	case WM_HSCROLL:
	case WM_VSCROLL:
		if((int)LOWORD(wParam) == SB_THUMBTRACK || (int)LOWORD(wParam) == SB_ENDSCROLL)
		{
			Listener('W', hwnd, message); // NotifyWindow
		}
		break;

	case WM_WINDOWPOSCHANGING:
	case WM_DESTROY:
		if(GetWindowRect(hwnd, &rect))
		{
			Listener('R', hwnd, (uint)&rect); // NotifyRectangle
		}
		break;

	case WM_WINDOWPOSCHANGED:
		Listener('W', hwnd, message); // NotifyWindow
		break;

	case WM_PAINT:
		Listener('A', hwnd, message); // NotifyWindowClientArea
		break;

	case 482:
		Listener('W', hwnd, message); // NotifyWindow
		break;

	case 485:
		Listener('W', hwnd, message); // NotifyWindow ???
		break;

	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
		Listener('C', hwnd, message); // NotifyWindow
		break;
	}
}

/*
	ëSïîìØÇ∂Ç‚ÇÒÅH
*/
LRESULT CALLBACK F_CWP(int code, WPARAM wParam, LPARAM lParam)
{
	Monitor(code, wParam, lParam);
	return CallNextHookEx(NULL, code, wParam, lParam);
}
LRESULT CALLBACK F_CWPR(int code, WPARAM wParam, LPARAM lParam)
{
	Monitor(code, wParam, lParam);
	return CallNextHookEx(NULL, code, wParam, lParam);
}
LRESULT CALLBACK F_GM(int code, WPARAM wParam, LPARAM lParam)
{
	Monitor(code, wParam, lParam);
	return CallNextHookEx(NULL, code, wParam, lParam);
}
LRESULT CALLBACK F_SMF(int code, WPARAM wParam, LPARAM lParam)
{
	Monitor(code, wParam, lParam);
	return CallNextHookEx(NULL, code, wParam, lParam);
}

int main(int argc, char **argv)
{
	HINSTANCE hm = GetModuleHandle(NULL);
	uint tid = GetCurrentThreadId();

	cout("%u\n", hm);
	cout("%u\n", tid);

hm = 0;
//tid = 0;

	H_CWP  = SetWindowsHookEx(WH_CALLWNDPROC,    F_CWP,  hm, tid);
cout("* %u\n", GetLastError());
	H_CWPR = SetWindowsHookEx(WH_CALLWNDPROCRET, F_CWPR, hm, tid);
cout("* %u\n", GetLastError());
	H_GM   = SetWindowsHookEx(WH_GETMESSAGE,     F_GM,   hm, tid);
cout("* %u\n", GetLastError());
	H_SMF  = SetWindowsHookEx(WH_SYSMSGFILTER,   F_SMF,  hm, tid);
cout("* %u\n", GetLastError());

	cout("%08x\n", H_CWP);
	cout("%08x\n", H_CWPR);
	cout("%08x\n", H_GM);
	cout("%08x\n", H_SMF);

	LOGPOS();
	getKey();
	LOGPOS();

	if(H_CWP)  UnhookWindowsHookEx(H_CWP);
	if(H_CWPR) UnhookWindowsHookEx(H_CWPR);
	if(H_GM)   UnhookWindowsHookEx(H_GM);
	if(H_SMF)  UnhookWindowsHookEx(H_SMF);
}
