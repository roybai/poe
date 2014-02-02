#include "stdafx.h"

HWND _myHwnd;
int mywindow();
LRESULT CALLBACK WndProc1 (HWND, UINT, WPARAM, LPARAM) ;
char szClassName[] = "MainWClasses"; 
HINSTANCE _hinst;
DWORD WINAPI create_win(LPVOID);
LRESULT CALLBACK WndProc1 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int  cxChar, cxCaps, cyChar, cyClient, iVscrollPos ;
    HDC         hdc ;    
    int         i, y ;    
    PAINTSTRUCT ps ;
    TEXTMETRIC  tm ;    
	//return DefWindowProc (hwnd, message, wParam, lParam) ;
	switch (message)    
    {
		case WM_LBUTTONDOWN:
			break;
		case WM_RBUTTONDOWN: 
			break;
		case WM_MBUTTONDOWN:		
			break;
		case WM_CREATE:
			//hdc = GetDC (hwnd) ;
		    //ReleaseDC (hwnd, hdc) ;
			break;
		case WM_SIZE:
			break;
	    case WM_PAINT:
			
	        hdc = BeginPaint (hwnd, &ps) ;
            TextOut (hdc, 0, y,
                "test",4);
	        EndPaint (hwnd, &ps) ;
			
		    //ReleaseDC (hwnd, hdc) ;
			break;
		case WM_DESTROY:
			PostQuitMessage (0) ;
			break;
     }
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}


DWORD WINAPI create_win(LPVOID dummy) 
{ 
    HWND hwnd; 
    MSG messages; 
    WNDCLASSEX wincl; 
	
	wincl.cbSize = sizeof(wincl);          // size of structure 
    wincl.style = CS_HREDRAW | 
        CS_VREDRAW;                    // redraw if size changes 
    wincl.lpfnWndProc = WndProc1;     // points to window procedure 
    wincl.cbClsExtra = 0;                // no extra class memory 
    wincl.cbWndExtra = 0;                // no extra window memory 
    wincl.hInstance = _hinst;         // handle to instance 
    wincl.hIcon = LoadIcon(NULL, 
        IDI_APPLICATION);              // predefined app. icon 
    wincl.hCursor = LoadCursor(NULL, 
        IDC_ARROW);                    // predefined arrow 
    wincl.hbrBackground     = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
  //  wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;             // white background brush 
    wincl.lpszMenuName =  "MainMenu";    // name of menu resource 
    wincl.lpszClassName =szClassName;  // name of window class 
	wincl.hIconSm = NULL; 

char	buf[100];

//sprintf_s(buf,"%d",_hinst);
//	MessageBox(NULL,buf,"11",MB_OK); 
    if (!RegisterClassEx (&wincl)) 
        return 0; 

    hwnd = CreateWindowEx ( 
          0, 
          szClassName,        /* Classname */ 
          "mywindow title",      /* Title Text */ 
          WS_OVERLAPPEDWINDOW, /* default window */ 
//          CW_USEDEFAULT,      /* Windows decides the position */ 
 //         CW_USEDEFAULT,      /* where the window ends up on the screen */ 
 0,
 0,
          640,                /* The programs width */ 
          400,                /* and height in pixels */ 
          NULL,        /* The window is a child-window to desktop */ 
          NULL,                /* No menu */ 
          _hinst,      /* Program Instance handler */ 
          NULL                /* No Window Creation data */ 
          ); 
	if(!hwnd)
	{
			long lError;
		   lError   =   ::GetLastError(); 
			sprintf_s(buf,"%d, %d",hwnd,lError);
			MessageBox(NULL,buf,"22",MB_OK); 
			return 0;
	}

	_myHwnd=hwnd;
    /* Make the window visible on the screen */ 
    ShowWindow (hwnd, SW_SHOWNORMAL); 
    UpdateWindow(hwnd); 

    /* Run the message loop. It will run until GetMessage() returns 0 */ 
    while (GetMessage (&messages, NULL, 0, 0)) 
    { 
        /* Translate virtual-key messages into character messages */ 
        TranslateMessage(&messages); 
        /* Send message to WindowProcedure */ 
        DispatchMessage(&messages); 
    } 

    /* The program return-value is 0 - The value that PostQuitMessage() gave */ 
    return messages.wParam; 
} 

int	mywindow()
{
	DWORD	dwThreadId;
	HANDLE handleCThread=CreateThread(NULL,0,create_win,NULL,0,&dwThreadId); 

	CloseHandle(handleCThread); 
	return 0;
}

