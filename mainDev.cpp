/* 
   Name: WinAnim
   Author: Brook Miles
   Description: Making an animation in windows
*/

#include <windows.h>
#include <math.h>
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

mBotVisual visual;

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

void DrawBall(HWND hwnd, HDC hdc)
{
   // based on https://docs.microsoft.com/en-us/previous-versions/ms969905(v=msdn.10)
   RECT rc;
   GetClientRect(hwnd, &rc);
   
   HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rc.right-rc.left, rc.bottom-rc.top);
   HDC hdcMemory = CreateCompatibleDC(hdc);
   HGDIOBJ hbmOld = SelectObject(hdcMemory, hbmMem);
	
	// erase background
   HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
   FillRect(hdcMemory, &rc, hbrBkGnd);
   DeleteObject(hbrBkGnd);


	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	
	int ballX = width / 2 + int(visual.x * 200);
	int ballY = height / 2 + int(visual.y * 200);
	double sh = sin(visual.heading);
	double ch = cos(visual.heading);
	int ballSize = 20;
	
	auto drawLine = [=] (COLORREF color, double bx, double by, double ex, double ey) {
      // Draw a red line
      HPEN hLinePen = CreatePen(PS_SOLID, 3, color);
      HPEN hPenOld = (HPEN)SelectObject(hdcMemory, hLinePen);
      
   	int posX = ballX + ch * ballSize * bx + sh * ballSize * by;
   	int posY = ballY - sh * ballSize * bx + ch * ballSize * by;
   	int posXE = ballX + ch * ballSize * ex + sh * ballSize * ey;
   	int posYE = ballY - sh * ballSize * ex + ch * ballSize * ey;
      MoveToEx(hdcMemory, posX, posY, NULL);
      LineTo(hdcMemory, posXE, posYE);
      
      SelectObject(hdcMemory, hPenOld);
      DeleteObject(hLinePen);
   };


   drawLine(RGB(255, 0, 0), 0, -1, 0, +1);
   drawLine(RGB(128, 0, 0), 0.2, -1, -0.2, -1);

   BitBlt(hdc,
      rc.left, rc.top,
      rc.right-rc.left, rc.bottom-rc.top,
      hdcMemory,
      0, 0,
      SRCCOPY);

   SelectObject(hdcMemory, hbmOld);
   DeleteObject(hdcMemory);
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
      case WM_ERASEBKGND:
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
      "GymKC Robotika - mBot Emulator",
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

