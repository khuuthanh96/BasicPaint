// BasicPaint01.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "BasicPaint01.h"

#define MAX_LOADSTRING 100
#define SHAPE_LINE 0
#define SHAPE_RECTANGLE 1
#define SHAPE_ELLIPSE 2

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

//Toạ Độ 
POINT pStart; //điểm bắt đầu nhấp chuột
POINT pEnd; //điểm kết thúc khi thả chuột

int ShapeMode = SHAPE_LINE; //nhận hình vẽ mặc định là đường thẳng
bool isDrawing = FALSE; //cờ kiểm tra đang vẽ hay không



class CShape {
public:
	virtual void Draw(HWND hWnd, POINT pStart, POINT pEnd) = 0;
	virtual void ReDraw(HWND hWnd) = 0;
	virtual void SetData(POINT pLeftTop, POINT pRightBottom) = 0;
	virtual CShape* CreateNew() = 0;
protected:
	POINT pLeftTop;
	POINT pRightBottom;
};

class CLine : public CShape{
public:
	void Draw(HWND hWnd, POINT pStart, POINT pEnd)
	{
		pLeftTop = pStart;
		pRightBottom = pEnd;

		HDC hdc = GetDC(hWnd);
		SelectObject(hdc, CreateSolidBrush(NULL_BRUSH)); //trong suốt

		//2 hàm dùng để vẽ đường thẳng
		MoveToEx(hdc, pLeftTop.x, pLeftTop.y, NULL);
		LineTo(hdc, pRightBottom.x, pRightBottom.y);

		ReleaseDC(hWnd, hdc);
	}

	void ReDraw(HWND hWnd) { Draw(hWnd, pLeftTop, pRightBottom); }


	void SetData(POINT p_LeftTop, POINT p_RightBottom)
	{
		pLeftTop = p_LeftTop;
		pRightBottom = p_RightBottom;
	}

	CShape* CreateNew() { return new CLine; }
};

class CRectangle : public CShape {
public:
	void Draw(HWND hWnd, POINT pStart, POINT pEnd) {
		HDC hdc = GetDC(hWnd);
		SelectObject(hdc, GetStockObject(NULL_BRUSH)); //Trong suốt

		Rectangle(hdc, pLeftTop.x, pLeftTop.y, pRightBottom.x, pRightBottom.y);
		ReleaseDC(hWnd, hdc);
	}

	CShape* CreateNew() { return new CRectangle; }

	void SetData(POINT p_LeftTop, POINT p_RightBottom) 
	{
		pLeftTop = p_LeftTop;
		pRightBottom = p_RightBottom;
	}

	void ReDraw(HWND hWnd) { Draw(hWnd, pLeftTop, pRightBottom); }
};

class CEllipse :public CShape{
public:
	void Draw(HWND hWnd, POINT pStart, POINT pEnd){
		HDC hdc = GetDC(hWnd);
		SelectObject(hdc, GetStockObject(NULL_BRUSH)); //Trong suốt

		Ellipse(hdc, pLeftTop.x, pLeftTop.y, pRightBottom.x, pRightBottom.y);
		ReleaseDC(hWnd, hdc);
	}

	CShape* CreateNew(){ return new CEllipse; }

	void SetData(POINT p_LeftTop, POINT p_RightBottom)
	{
		pLeftTop = p_LeftTop;
		pRightBottom = p_RightBottom;
	}

	void ReDraw(HWND hWnd) { Draw(hWnd, pLeftTop, pRightBottom); }
};


vector <CShape*> Shapes; //Những hình đã vẽ

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

void ResizeToSquare(POINT pStart, POINT &pEnd);


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_BASICPAINT01, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BASICPAINT01));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BASICPAINT01));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_BASICPAINT01);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
	{
						CreateWindowEx(0, L"STATIC", L"SHAPE MODE: LINE", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 0, 0, 200, 20, hWnd, NULL, hInst, NULL);
					    break;
	}
	case WM_COMMAND:
	{
						 wmId = LOWORD(wParam);
						 wmEvent = HIWORD(wParam);

					     // Parse the menu selections:
						 switch (wmId)
						 {
						 case IDM_EXIT:
							 DestroyWindow(hWnd);
							 break;
						 case ID_PAINT_LINE:
						 {
							 ShapeMode = SHAPE_LINE;
							 isDrawing = FALSE;
							 CreateWindowEx(0, L"STATIC", L"SHAPE MODE: LINE", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 0, 0, 200, 20, hWnd, NULL, hInst, NULL);
							 break;
						 }
						 case ID_PAINT_RECTANGLE:
						 {
							 ShapeMode = SHAPE_RECTANGLE;
							 isDrawing = FALSE;
							 CreateWindowEx(0, L"STATIC", L"SHAPE MODE: RECTANGLE", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 0, 0, 200, 20, hWnd, NULL, hInst, NULL);
							 break;
						 }
						 case ID_PAINT_ELLIPSE:
						 {
							 ShapeMode = SHAPE_ELLIPSE;
							 isDrawing = FALSE;
							 CreateWindowEx(0, L"STATIC", L"SHAPE MODE: ELLIPSE", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 0, 0, 200, 20, hWnd, NULL, hInst, NULL);
							 break;
						 }
						 default:
							 return DefWindowProc(hWnd, message, wParam, lParam);
						 }
			 		     break;
	}
	case WM_PAINT:
	{
						 PAINTSTRUCT ps;
					     HDC hdc = BeginPaint(hWnd, &ps);

						 switch (ShapeMode)
						 {
						 case SHAPE_LINE:
							 if (isDrawing) {
								 MoveToEx(hdc, pStart.x, pStart.y, NULL);
								 LineTo(hdc, pEnd.x, pEnd.y);
							 }
							 break;
						 case SHAPE_RECTANGLE:
							 if (isDrawing){
								Rectangle(hdc, pStart.x, pStart.y, pEnd.x, pEnd.y);
							 }

							 break;
						 case SHAPE_ELLIPSE:
							 if (isDrawing){
								 Ellipse(hdc, pStart.x, pStart.y, pEnd.x, pEnd.y);
							 }
							 break;
						 default:
							 break;
						 }

						 for (int i = 0; i < Shapes.size(); i++)
						 {
							 Shapes[i]->ReDraw(hWnd);
						 }

						 EndPaint(hWnd, &ps);
						 break;
	}
	case WM_MOUSEMOVE:
	{
						 pEnd.x = GET_X_LPARAM(lParam);
						 pEnd.y = GET_Y_LPARAM(lParam);

						 if (isDrawing)
						 {
							 if (wParam & MK_SHIFT)
							 {
								 ResizeToSquare(pStart, pEnd);
							 }
							 InvalidateRect(hWnd, NULL, TRUE);
						 }
						 break;
	}
	case WM_LBUTTONDOWN:
	{
						 isDrawing = TRUE;
						 pStart.x = GET_X_LPARAM(lParam);
						 pStart.y = GET_Y_LPARAM(lParam);
						 break;
	}
	case WM_LBUTTONUP:
	{
						 isDrawing = FALSE;
						 pEnd.x = GET_X_LPARAM(lParam);
						 pEnd.y = GET_Y_LPARAM(lParam);
						 switch (ShapeMode)
						 {
						 case SHAPE_LINE:
						 {
								   CLine* line = new CLine;
								   line->SetData(pStart, pEnd);
								   Shapes.push_back(line);
								   break;
						 }
						 case SHAPE_RECTANGLE:
						 {
								   CRectangle* rect = new CRectangle;

								   if (wParam & MK_SHIFT)
								   {
									   ResizeToSquare(pStart, pEnd);
								   }
								   
								   rect->SetData(pStart, pEnd);
								   Shapes.push_back(rect);
								   
								   break;
						 }
						 case SHAPE_ELLIPSE:
						 {
								   CEllipse* ellip = new CEllipse;
								   if (wParam & MK_SHIFT)
								   {
									   ResizeToSquare(pStart, pEnd);
								   }

								   ellip->SetData(pStart, pEnd);
								   Shapes.push_back(ellip);
								   break;
						 }
						 default:
							 break;
						 }
						 break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void ResizeToSquare(POINT pStart, POINT &pEnd)
{
	if (abs(pStart.x - pEnd.x) > abs(pStart.y - pEnd.y))
	{
		if (pEnd.x > pStart.x)
			pEnd.x = pStart.x + abs(pStart.y - pEnd.y);
		else
			pEnd.x = pStart.x - abs(pStart.y - pEnd.y);
	}
	else
	{
		if (pEnd.y > pStart.y)
			pEnd.y = pStart.y + abs(pStart.x - pEnd.x);
		else
			pEnd.y = pStart.y - abs(pStart.x - pEnd.x);
	}
}