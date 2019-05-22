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
#include <memory>
#include <windowsx.h>

static char g_szClassName[] = "mBotEmulatorWindowClass";
static HINSTANCE g_hInst = NULL;

const UINT idTimer1 = 1;
UINT nTimerDelay = 10;

HBITMAP hbmBall, hbmRoom;


LONG lastTime;
bool lastTimeSet = false;
bool accelerate = false;


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

	COLORREF Pixel(int pixelX, int pixelY) const {
		int b = bits[pixelX * 4 + pixelY * bm.bmWidthBytes + 0];
		int g = bits[pixelX * 4 + pixelY * bm.bmWidthBytes + 1];
		int r = bits[pixelX * 4 + pixelY * bm.bmWidthBytes + 2];
		return RGB(r, g, b);
	}

	bool inside(int pixelX, int pixelY) const {
		return pixelX >= 0 && pixelX < bm.bmWidth && pixelY >= 0 && pixelY < bm.bmHeight;
	}

	~RoomBitmap() {
		delete[] bits;
	}
};

static int ScreenScale = 100;

POINT ScreenPos(Pos pos) {
	POINT p;
	p.x = 400 + int(pos.x * ScreenScale);
	p.y = 300 + int(pos.y * ScreenScale);
	return p;
}

double PixelSize() {
	return 1.0 / ScreenScale;
}


std::unique_ptr<RoomBitmap> roomBitmap;

COLORREF GetRoomPixel(int pixelX, int pixelY) {
	// TODO: cache
	if (!roomBitmap->inside(pixelX, pixelY)) return RGB(255, 0, 0); // outside is the wall
	return roomBitmap->Pixel(pixelX, pixelY);
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


COLORREF LerpColor(COLORREF a, COLORREF b, double f) {
	int ra = GetRValue(a);
	int ga = GetGValue(a);
	int ba = GetBValue(a);
	int rb = GetRValue(b);
	int gb = GetGValue(b);
	int bb = GetBValue(b);
	int xr = int(ra + (rb - ra) * f);
	int xg = int(ga + (gb - ga) * f);
	int xb = int(ba + (bb - ba) * f);
	if (xr < 0) xr = 0; else if (xr > 255) xr = 255;
	if (xg < 0) xg = 0; else if (xg > 255) xg = 255;
	if (xb < 0) xb = 0; else if (xb > 255) xb = 255;
	return RGB(xr, xg, xb);
}
/**
@return 0 white, 1 black, 2 obstacle (red)
*/
RoomColor GetRoomColor(Pos pos) {
	// TODO: DRY
	double ballXD = 400 + (pos.x * 100);
	double ballYD = 300 + (pos.y * 100);
	int ballX = int(floor(ballXD));
	int ballY = int(floor(ballYD));
	double ballXF = ballXD - ballX;
	double ballYF = ballYD - ballY;

	COLORREF pixel00 = GetRoomPixel(ballX, ballY);
	COLORREF pixel01 = GetRoomPixel(ballX, ballY + 1);
	COLORREF pixel10 = GetRoomPixel(ballX + 1, ballY);
	COLORREF pixel11 = GetRoomPixel(ballX + 1, ballY + 1);
	COLORREF pixelX0 = LerpColor(pixel00, pixel10, ballXF);
	COLORREF pixelX1 = LerpColor(pixel01, pixel11, ballXF);
	COLORREF pixel = LerpColor(pixelX0, pixelX1, ballYF);

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


double RoomRayCast(Pos beg, Pos end) {
	// TODO: proper DDA implementation
	double dist = end.dist(beg);

	double pixel = PixelSize();
	int pixels = (int) ceil(dist / pixel);
	Pos pos = beg;
	Pos step = (end - beg) / pixels;
	for (int i = 0; i <= pixels; i++, pos += step) {
		auto c = GetRoomColor(pos);
		if (c == RoomWall) {
			break;
		}
	}
	return pos.dist(beg);
}


static LONG simulatedTime = 0;

static bool buttonL = false;
static bool buttonR = false;

long millis() {
	return simulatedTime;
}

void UpdateBall() {
	LONG now = GetTickCount();
	LONG deltaT = now - lastTime;
	if (!lastTimeSet) {
		deltaT = 0;
		lastTimeSet = true;
	}

	lastTime = now;
	int acceleration = accelerate ? 10 : 1;

	long toSimulateMs = deltaT * acceleration;
	while (toSimulateMs > 0) {
		long ms = std::min(toSimulateMs, 20L);

		visual = emulatorLoop(ms * 0.001);
		simulatedTime += ms;
		toSimulateMs -= ms;
	}

}

COLORREF DisplaySensor(RoomColor roomColor) {
	switch (roomColor) {
		case RoomWhite:
			return RGB(200, 150, 0);
		case RoomBlack:
			return RGB(0, 255, 0);
		case RoomWall:
			return RGB(0, 0, 255);
		default:
			return RGB(0, 0, 0);
	}
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

	DeleteObject(hbmMem);

	auto drawLine = [=](COLORREF color, double bx, double by, double ex, double ey, int lineWidth = 3) {
		// Draw a red line
		HPEN hLinePen = CreatePen(PS_SOLID, lineWidth, color);
		HPEN hPenOld = (HPEN) SelectObject(hdcMemory, hLinePen);

		POINT pos = ScreenPos(visual.local(bx, by));
		POINT posE = ScreenPos(visual.local(ex, ey));
		MoveToEx(hdcMemory, pos.x, pos.y, NULL);
		LineTo(hdcMemory, posE.x, posE.y);

		SelectObject(hdcMemory, hPenOld);
		DeleteObject(hLinePen);
	};


	drawLine(RGB(100, 200, 0), 0, -1 * botScale, 0, +1 * botScale);
	drawLine(RGB(128, 0, 0), 0.3 * botScale, -1 * botScale, -0.3 * botScale, -1 * botScale);

	drawLine(DisplaySensor(visual.sensorLeft),  MBotConfig::lineSensorPosL, MBotConfig::lineSensorPosFront, MBotConfig::lineSensorPosL, MBotConfig::lineSensorPosFront, 5);
	drawLine(DisplaySensor(visual.sensorRight), MBotConfig::lineSensorPosR, MBotConfig::lineSensorPosFront, MBotConfig::lineSensorPosR, MBotConfig::lineSensorPosFront, 5);

	double ledY = 0.5;
	double ledX = 0.4;
	drawLine(visual.ledLeft, +ledX * botScale, +ledY * botScale, +ledX * botScale, +ledY * botScale, 5);
	drawLine(visual.ledRight, -ledX * botScale, +ledY * botScale, -ledX * botScale, +ledY * botScale, 5);

	if (false) {
		drawLine(RGB(100, 150, 255), 0, 0.1, 0, 0.1 + visual.ultrasonicDistance, 2);
	}

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

			roomBitmap = std::unique_ptr<RoomBitmap>(new RoomBitmap(hbmRoom));

			SetTimer(hwnd, idTimer1, nTimerDelay, NULL);


			break;
		case WM_KEYDOWN:
			if (wParam == VK_SHIFT) accelerate = true;
			break;
		case WM_KEYUP:
			if (wParam == VK_SHIFT) accelerate = false;
			break;
		case WM_LBUTTONDOWN: {
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			placeMBot((xPos - 400)* PixelSize(), (yPos - 300) * PixelSize());
			buttonL = true;
			break;
		}
		case WM_RBUTTONDOWN: {
			buttonR = true;
			break;
		}
		case WM_LBUTTONUP:
			buttonL = false;
			break;
		case WM_RBUTTONUP:
			buttonR = false;
			break;
		case WM_MOUSEMOVE:
			if (buttonL || buttonR) {
				POINT botPos = ScreenPos(visual.pos);
				int xPos = GET_X_LPARAM(lParam);
				int yPos = GET_Y_LPARAM(lParam);
				if (abs(xPos - botPos.x) > 5 || abs (yPos - botPos.y) > 5) {
					double heading = atan2(xPos - botPos.x, yPos - botPos.y);
					rotateMBot(heading);
				}
			}
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
