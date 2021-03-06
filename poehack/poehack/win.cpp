// window.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "stdio.h"
#include "ReceivePacket.h"

//extern BOOL InterceptWinsock();
//extern BOOL 	PoeInterceptWinsock(HWND);
int	_create_win();
DWORD WINAPI _WinMain(LPVOID lpParam);
void update_log_buf(char	*buf);
void _showinfo(char* lpszFormat,...);
HINSTANCE	_hinst;
HWND	_myhwnd=NULL;
char szClassName[] = "MainWClass"; 
BOOL	_window_up=FALSE;
//#define NUMLINES ((int) (sizeof sysmetrics / sizeof sysmetrics [0]))
#define NUMLINES 25
#define	MAX_LEN	256

struct
{
int iIndex ;
char	text[MAX_LEN];
}display_buf[NUMLINES];


LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
int	_create_win()
{
	DWORD	dwThreadId;
    HANDLE handleCThread=CreateThread(NULL,0,_WinMain,NULL,0,&dwThreadId); 
	sprintf(tempbuf,"created win: threadid:%x , handleid:%x",dwThreadId,handleCThread);
	CloseHandle(handleCThread); 

	return 0;
}



LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int  cxChar, cxCaps, cyChar, cyClient, iVscrollPos ;
    HDC         hdc ;    
    int         i, y ;    
    PAINTSTRUCT ps ;
//    TCHAR       szBuffer[10] ;    
    TEXTMETRIC  tm ;    
	static int LoadDLLTag=0;
	static int poehackTag=0;
    switch (message)    
     {

case WM_CREATE:
	strcpy(tempbuf,"wm_create");
    hdc = GetDC (hwnd) ;
    GetTextMetrics (hdc, &tm) ;
    cxChar = tm.tmAveCharWidth ;
    cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2 ;
    cyChar = tm.tmHeight + tm.tmExternalLeading ;

    ReleaseDC (hwnd, hdc) ;
    SetScrollRange (hwnd, SB_VERT, 0, NUMLINES - 1, FALSE) ;
    SetScrollPos   (hwnd, SB_VERT, iVscrollPos, TRUE) ;
	_window_up=TRUE;
        return 0 ; 

    case WM_SIZE:
        cyClient = HIWORD (lParam) ;
        return 0 ;

    case WM_VSCROLL:
        switch (LOWORD (wParam))
          {
    case SB_LINEUP:
          iVscrollPos -= 1 ;
        break ;
     
    case SB_LINEDOWN:
        iVscrollPos += 1 ;
        break ;

    case SB_PAGEUP:
        iVscrollPos -= cyClient / cyChar ;
        break ;
     
    case SB_PAGEDOWN:
        iVscrollPos += cyClient / cyChar ;
        break ;
     
    case SB_THUMBPOSITION:
        iVscrollPos = HIWORD (wParam) ;
        break ;
     
    default :
        break ;
          }

    iVscrollPos = max (0, min (iVscrollPos, NUMLINES - 1)) ;
    if (iVscrollPos != GetScrollPos (hwnd, SB_VERT))
          {
        SetScrollPos (hwnd, SB_VERT, iVscrollPos, TRUE) ;
        InvalidateRect (hwnd, NULL, TRUE) ;
          }
        return 0 ;
    case WM_PAINT:

        hdc = BeginPaint (hwnd, &ps) ;
        for (i = 0 ; i < NUMLINES ; i++)
        {
			int len=   strlen (display_buf[i].text) ;
			if(len==0) continue;
            y = cyChar * (i - iVscrollPos) ;
            TextOut (hdc, 0, y,
                display_buf[i].text,
                strlen (display_buf[i].text)) ;
     /*
            TextOut (hdc, 22 * cxCaps, y,
                display_buf[i].szDesc,
                lstrlen (sysmetrics[i].szDesc)) ;
     
            SetTextAlign (hdc, TA_RIGHT | TA_TOP) ;
            TextOut (hdc, 22 * cxCaps + 40 * cxChar, y, szBuffer,
                wsprintf (szBuffer, TEXT ("%5d"),
                        GetSystemMetrics (sysmetrics[i].iIndex))) ;
	
            SetTextAlign (hdc, TA_LEFT | TA_TOP) ;
		*/
          }
        EndPaint (hwnd, &ps) ;
        return 0 ;

    case WM_DESTROY:
  	_window_up=FALSE;
      PostQuitMessage (0) ;
        return 0 ;
     }
//	_showinfo("message id%d",message);
 
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}




LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message)  
    { 
        case WM_DESTROY: 
            PostQuitMessage (0); 
            break; 
        default: 
            return DefWindowProc (hwnd, message, wParam, lParam); 
    } 

    return 0; 
} 
DWORD WINAPI _WinMain(LPVOID lpParam) 
{ 
    HWND hwnd; 
    MSG messages; 
    WNDCLASSEX wincl; 
	strcpy(tempbuf,"in winmain");
/*
    wincl.hInstance = hInstance; 
    wincl.lpszClassName = szClassName; 
    wincl.lpfnWndProc = WindowProcedure; 
    wincl.style = CS_DBLCLKS; 
    wincl.cbSize = sizeof (WNDCLASSEX); 
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND; 
	MessageBox(NULL,"AA","AA",MB_OK); 
*/
		wincl.cbSize = sizeof(wincl);          // size of structure 
    wincl.style = CS_HREDRAW | 
        CS_VREDRAW;                    // redraw if size changes 
    wincl.lpfnWndProc = WndProc;     // points to window procedure 
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
    wincl.lpszClassName = "poe";  // name of window class 
	wincl.hIconSm = NULL; 

char	buf[100];

//sprintf(buf,"%d",_hinst);
//	MessageBox(NULL,buf,"11",MB_OK); 
	strcpy(tempbuf,"in winmain 1");
    if (!RegisterClassEx (&wincl)) 
	{
			long lError;
		   lError   =   ::GetLastError(); 
			sprintf(tempbuf,"%d",lError);
        return 0; 
	}
	strcpy(tempbuf,"in winmain 2");

    hwnd = CreateWindowEx ( 
          0, 
          szClassName,        /* Classname */ 
          "MainWClass",      /* Title Text */ 
          WS_OVERLAPPEDWINDOW, /* default window */ 
//          CW_USEDEFAULT,      /* Windows decides the position */ 
 //         CW_USEDEFAULT,      /* where the window ends up on the screen */ 
 0,
 0,
          1920,                /* The programs width */ 
          440,                /* and height in pixels */ 
          HWND_DESKTOP,        /* The window is a child-window to desktop */ 
          NULL,                /* No menu */ 
          _hinst,      /* Program Instance handler */ 
          NULL                /* No Window Creation data */ 
          ); 
	if(!hwnd)
	{
			long lError;
		   lError   =   ::GetLastError(); 
			sprintf(tempbuf,"%d, %d",hwnd,lError);
//			MessageBox(NULL,buf,"22",MB_OK); 
			return 0;
	}
	strcpy(tempbuf,"in winmain 1");

	_myhwnd=hwnd;
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

void update_log_buf(char	*buf)
{
	int	i;
	
	if(buf[0]==0) return;
	if(strlen(buf)>=MAX_LEN) buf[MAX_LEN-1]=0;
	BOOL	full=TRUE;
	for(i=0;i<NUMLINES;i++)
	{
		if(display_buf[i].text[0]==0)
		{
			strcpy(display_buf[i].text,buf);
			return;
		}
	}
	if(full)
	{
		for(i=0;i<NUMLINES-1;i++)
		{
			strcpy(display_buf[i].text,display_buf[i+1].text);
		}
		strcpy(display_buf[NUMLINES-1].text,buf);
	}
	return;
}

void _showinfo(LPCTSTR lpszFormat,...)
{
	/*
	return;
	if(!_window_up)
	{
		MessageBox(NULL,"no win","error",MB_OK);
		_create_win();
		return;
	}
	return;
	*/
	if(!lpszFormat) return;
	char _buf[4096],_sub[4096];
	int i,j=0;
	va_list args;
	va_start(args, lpszFormat);
	_vsnprintf(_buf,(sizeof(_buf)/sizeof(_buf[0])),lpszFormat,args);
	for(i=0;i<strlen(_buf);i++)
	{
		if(_buf[i]=='\n') 
		{
			_sub[j]=0;
			update_log_buf(_sub);
			j=0;
			continue;
		}
		else
			_sub[j]=_buf[i];
		j++;
//		update_log_buf(_buf);
	}
	_sub[j]=0;
	update_log_buf(_sub);
	va_end(args);
//	::PostMessage(_hwnd,WM_PAINT,0,0);
	InvalidateRect(_myhwnd,NULL,TRUE);
//    UpdateWindow(_hwnd); 

	
}

