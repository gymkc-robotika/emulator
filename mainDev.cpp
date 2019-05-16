#pragma ide diagnostic ignored "hicpp-use-nullptr"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
/*
   Name: WinAnim
   Author: Brook Miles
   Description: Making an animation in windows
*/

#include <windows.h>
#include <math.h>
#include <limits.h>
#include "mBotEmul.h"
#include <string>

static char g_szClassName[] = "mBotEmulatorWindowClass";
static HINSTANCE g_hInst = NULL;

const UINT idTimer1 = 1;
UINT nTimerDelay = 10;

HBITMAP hbmBall, hbmRoom;

LONG lastTime;
bool lastTimeSet = false;

MBotPos visual;

class RoomBitmap {
	BITMAP bm;
	unsigned char *bits;
	LONG cb;

	public:
	explicit RoomBitmap(HBITMAP handle) {
		GetObject(hbmRoom, sizeof(bm), &bm);
		cb = bm.bmWidthBytes * bm.bmHeight;
		bits = new unsigned char[cb];
		GetBitmapBits(hbmRoom, cb, bits);
	}

	COLORREF Pixel(int pixelX, int pixelY) {
		int b = bits[pixelX * 4 + pixelY * bm.bmWidthBytes + 0];
		int g = bits[pixelX * 4 + pixelY * bm.bmWidthBytes + 1];
		int r = bits[pixelX * 4 + pixelY * bm.bmWidthBytes + 2];
		return RGB(r, g, b);
	}

	~RoomBitmap() {
		delete[] bits;
	}
};

COLORREF GetRoomPixel(int pixelX, int pixelY) {
	// TODO: cache
	RoomBitmap bitmap(hbmRoom);
	return bitmap.Pixel(pixelX, pixelY);
}

static int colorDist(COLORREF c1, COLORREF c2) {
	int rd = GetRValue(c1) - GetRValue(c2);
	int gd = GetGValue(c1) - GetGValue(c2);
	int bd = GetBValue(c1) - GetBValue(c2);
	return rd * rd + gd * gd + bd * bd;
}

static COLORREF RoomWhiteColor = RGB(255, 255, 255);
static COLORREF RoomBlackColor = RGB(0, 0, 0);
static COLORREF RoomWallColor = RGB(255, 0, 0);

/**
@return 0 white, 1 black, 2 obstacle (red)
*/
RoomColor GetRoomColor(Pos pos) {
	// TODO: consider interpolation to achieve smooth edge behaviour
	// TODO: DRY
	int ballX = 400 + int(pos.x * 100);
	int ballY = 300 + int(pos.y * 100);
	COLORREF pixel = GetRoomPixel(ballX, ballY);
	// check color, return color type
	int whiteDist = colorDist(pixel, RoomWhiteColor);
	int blackDist = colorDist(pixel, RoomBlackColor);
	int wallDist = colorDist(pixel, RoomWallColor);
	RoomColor bestColor = RoomWhite;
	int bestDist = whiteDist;
	if (blackDist < bestDist) {
		bestDist = blackDist;
		bestColor = RoomBlack;
	}
	if (wallDist < bestDist) {
		bestDist = wallDist;
		bestColor = RoomWall;
	}
	return bestColor;
}


void UpdateBall() {
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

POINT ScreenPos(Pos pos) {
	POINT p;
	p.x = 400 + int(pos.x * 100);
	p.y = 300 + int(pos.y * 100);
	return p;
}

void DrawBall(HWND hwnd, HDC hdc) {
	// based on https://docs.microsoft.com/en-us/previous-versions/ms969905(v=msdn.10)
	RECT rc;
	GetClientRect(hwnd, &rc);

	HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
	HDC hdcMemory = CreateCompatibleDC(hdc);
	HGDIOBJ hbmOld = SelectObject(hdcMemory, hbmMem);

	// erase background
	HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(hdcMemory, &rc, hbrBkGnd);
	DeleteObject(hbrBkGnd);

	BITMAP bm;
	GetObject(hbmRoom, sizeof(bm), &bm);
	HDC hdcMem = CreateCompatibleDC(hdc);
	HGDIOBJ hbmOldBitmap = SelectObject(hdcMem, hbmRoom);

	BitBlt(hdcMemory, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmOldBitmap);
	DeleteDC(hdcMem);

	double botScale = 0.1;

	auto drawLine = [=](COLORREF color, double bx, double by, double ex, double ey) {
		// Draw a red line
		HPEN hLinePen = CreatePen(PS_SOLID, 3, color);
		HPEN hPenOld = (HPEN) SelectObject(hdcMemory, hLinePen);

		POINT pos = ScreenPos(visual.local(botScale * bx, botScale * by));
		POINT posE = ScreenPos(visual.local(botScale * ex, botScale * ey));
		MoveToEx(hdcMemory, pos.x, pos.y, NULL);
		LineTo(hdcMemory, posE.x, posE.y);

		SelectObject(hdcMemory, hPenOld);
		DeleteObject(hLinePen);
	};


	drawLine(RGB(100, 200, 0), 0, -1, 0, +1);
	drawLine(RGB(128, 0, 0), 0.3, -1, -0.3, -1);

	BitBlt(hdc,
			 rc.left, rc.top,
			 rc.right - rc.left, rc.bottom - rc.top,
			 hdcMemory,
			 0, 0,
			 SRCCOPY);

	SelectObject(hdcMemory, hbmOld);
	DeleteObject(hdcMemory);
	DeleteDC(hdcMemory);
}

HBITMAP ReadBitmap(const char *resName, const char *fileName) {
	HBITMAP hbm = LoadBitmap(g_hInst, resName);

	if (!hbm) {
		hbm = (HBITMAP) LoadImage(NULL, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (!hbm) {
			std::string name = std::string("../") + fileName;
			hbm = (HBITMAP) LoadImage(NULL, name.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
	}
	return hbm;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
		case WM_CREATE:

			visual = emulatorSetup();

			hbmBall = ReadBitmap("BALLBMP", "../ball.bmp");
			hbmRoom = ReadBitmap("ROOMBMP", "../room.bmp");

			if (!hbmBall || !hbmRoom) {
				MessageBox(hwnd, "Load of resources failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
				return -1;
			}

			SetTimer(hwnd, idTimer1, nTimerDelay, NULL);


			break;
		case WM_TIMER:
			if (hbmBall && hbmRoom) {
				HDC hdcWindow;
				hdcWindow = GetDC(hwnd);

				UpdateBall();
				DrawBall(hwnd, hdcWindow);

				ReleaseDC(hwnd, hdcWindow);
			}
			break;
		case WM_PAINT:
			if (hbmBall && hbmRoom) {
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
			DeleteObject(hbmRoom);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
						 LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX WndClass;
	HWND hwnd;
	MSG Msg;

	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.style = 0;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = g_hInst;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = g_szClassName;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&WndClass)) {
		MessageBox(0, "Window Registration Failed!", "Error!",
					  MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);
		return 0;
	}

	RECT clientRect;
	clientRect.top = 0;
	clientRect.left = 0;
	clientRect.right = 800;
	clientRect.bottom = 600;
	RECT winRect = clientRect;
	AdjustWindowRectEx(&winRect, WS_CAPTION, FALSE, WS_EX_CLIENTEDGE);
	hwnd = CreateWindowEx(
			  WS_EX_CLIENTEDGE,
			  g_szClassName,
			  "GymKC Robotika - mBot Emulator",
			  WS_OVERLAPPEDWINDOW,
			  CW_USEDEFAULT, CW_USEDEFAULT, winRect.right - winRect.left, winRect.bottom - winRect.top,
			  NULL, NULL, g_hInst, NULL);

	if (hwnd == NULL) {
		MessageBox(0, "Window Creation Failed!", "Error!",
					  MB_ICONEXCLAMATION | MB_OK | MB_SYSTEMMODAL);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Msg, NULL, 0, 0)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
