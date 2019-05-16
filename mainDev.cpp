/* 
   Name: WinAnim
   Author: Brook Miles
   Description: Making an animation in windows
*/

#include <windows.h>
#include "mBotEmul.h"

static char g_szClassName[] = "mBotEmulatorWindowClass";
static HINSTANCE g_hInst = NULL;

const UINT idTimer1 = 1;
UINT nTimerDelay = 10;

HBITMAP hbmBall, hbmMask;
BITMAP bm;

int deltaValue = 200;


LONG lastTime;
bool lastTimeSet = false;

struct BallScreenPos {
	int x, y;
};

mBotVisual visual;
BallScreenPos lastBall;
boolean lastBallSet = false;

void UpdateBall(HWND hwnd)
{
	LONG now = GetTickCount();
	LONG deltaT = now - lastTime;
	if (!lastTimeSet) {
		deltaT = 0;
		lastTimeSet = true;
	}
	lastTime = now;
	
	double dt = deltaT / 1000.0;
	visual = emulatorLoop(dt);
}

BallScreenPos computeScreenPos(HWND hwnd, mBotVisual visual) {
   RECT rc;
   GetClientRect(hwnd, &rc);

	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	
	int ballX = width / 2 + int(visual.x * 200);
	int ballY = height / 2 + int(visual.y * 200);
	BallScreenPos pos;
	pos.x = ballX;
	pos.y = ballY;
	return pos;
}
void EraseBall(HWND hwnd, HDC hdc)
{
   if (!lastBallSet) return;
   
	BallScreenPos pos = lastBall;
	
   RECT rc;
   rc.left = pos.x;
   rc.top = pos.y;
   rc.right = pos.x + bm.bmWidth;
   rc.bottom = pos.y + bm.bmHeight;
   FillRect(hdc, &rc, (HBRUSH)(COLOR_BTNFACE+1));
}

void DrawBall(HWND hwnd, HDC hdc)
{
	BallScreenPos pos = computeScreenPos(hwnd, visual);
	
	lastBall = pos;
	lastBallSet = true;
	
   HDC hdcMemory;
   hdcMemory = CreateCompatibleDC(hdc);

   SelectObject(hdcMemory, hbmMask);
   BitBlt(hdc, pos.x, pos.y, bm.bmWidth, bm.bmHeight, hdcMemory, 0, 0, SRCAND);

   SelectObject(hdcMemory, hbmBall);
   BitBlt(hdc, pos.x, pos.y, bm.bmWidth, bm.bmHeight, hdcMemory, 0, 0, SRCPAINT);

   DeleteDC(hdcMemory);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch(Message)
   {
      case WM_CREATE:
      
         visual = emulatorSetup();
		
         hbmBall = LoadBitmap(g_hInst, "BALLBMP");
         hbmMask = LoadBitmap(g_hInst, "MASKBMP");
         if(!hbmBall || !hbmMask){
            MessageBox(hwnd, "Load of resources failed.", "Error",
               MB_OK | MB_ICONEXCLAMATION);
            return -1;
         }

         GetObject(hbmBall, sizeof(bm), &bm);
         SetTimer(hwnd, idTimer1, nTimerDelay, NULL);


      break;
      case WM_TIMER:
         if(hbmBall && hbmMask)
         {
            HDC hdcWindow;
            hdcWindow = GetDC(hwnd);

            EraseBall(hwnd, hdcWindow);
            UpdateBall(hwnd);
            DrawBall(hwnd, hdcWindow);

            ReleaseDC(hwnd, hdcWindow);
         }
      break;
      case WM_PAINT:
         if(hbmBall && hbmMask)
         {
            PAINTSTRUCT ps;
            HDC hdcWindow;
            hdcWindow = BeginPaint(hwnd, &ps);

            DrawBall(hwnd, hdcWindow);
            
            EndPaint(hwnd, &ps);
         }
      break;
      case WM_CLOSE:
         DestroyWindow(hwnd);
      break;
      case WM_DESTROY:
         KillTimer(hwnd, idTimer1);
         
         DeleteObject(hbmBall);
         DeleteObject(hbmMask);
         PostQuitMessage(0);
      break;
      default:
         return DefWindowProc(hwnd, Message, wParam, lParam);
   }
   return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpCmdLine, int nCmdShow)
{
   WNDCLASSEX WndClass;
   HWND hwnd;
   MSG Msg;

   g_hInst = hInstance;

   WndClass.cbSize        = sizeof(WNDCLASSEX);
   WndClass.style         = 0;
   WndClass.lpfnWndProc   = WndProc;
   WndClass.cbClsExtra    = 0;
   WndClass.cbWndExtra    = 0;
   WndClass.hInstance     = g_hInst;
   WndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
   WndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
   WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
   WndClass.lpszMenuName  = NULL;
   WndClass.lpszClassName = g_szClassName;
   WndClass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

   if(!RegisterClassEx(&WndClass))
   {
      MessageBox(0, "Window Registration Failed!", "Error!",
         MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);
      return 0;
   }

   hwnd = CreateWindowEx(
      WS_EX_CLIENTEDGE,
      g_szClassName,
      "A Bitmap Program",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 320, 240,
      NULL, NULL, g_hInst, NULL);

   if(hwnd == NULL)
   {
      MessageBox(0, "Window Creation Failed!", "Error!",
         MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);
      return 0;
   }

   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

   while(GetMessage(&Msg, NULL, 0, 0))
   {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
   }
   return Msg.wParam;
}

