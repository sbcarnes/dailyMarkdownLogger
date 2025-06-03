#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define IDC_EDIT_LOG 1001

const char g_szClassName[] = "myWindowClass";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HDC hdcCompat;
    static HBITMAP hbmp;

    static RECT rcClient;

    static POINT pt;
    
    static SYSTEMTIME localTime;
    static char curDateBuffer[128];

    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *pCreate = (CREATESTRUCT *) lParam;
            HINSTANCE hInstance = pCreate->hInstance;
            
            HDC hdc = GetDC(hwnd);
            hdcCompat = CreateCompatibleDC(hdc);
            GetClientRect(hwnd, &rcClient);

            GetLocalTime(&localTime);
            snprintf(curDateBuffer, sizeof(curDateBuffer), "%04d-%02d-%02d", localTime.wYear, localTime.wMonth, localTime.wDay);
            
            HWND dateDisplay = CreateWindow(
                "STATIC",
                curDateBuffer,
                WS_CHILD | WS_VISIBLE,
                10, 10, 180, 20,
                hwnd,
                NULL,
                hInstance,
                NULL
            );
            
            HWND promptDisplay = CreateWindow(
                "STATIC",
                "What did you learn today?",
                WS_CHILD | WS_VISIBLE,
                10, 45, 180, 20,
                hwnd,
                NULL,
                hInstance,
                NULL
            );
            
            HWND logContent = CreateWindow(
                "EDIT",
                NULL,
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
                10, 80, 400, 200,
                hwnd,
                (HMENU)IDC_EDIT_LOG,
                hInstance,
                NULL
            );
            
            HWND saveButton = CreateWindow(
                "BUTTON",
                "Save",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                450, 100, 100, 30,
                hwnd,
                (HMENU)1,
                hInstance,
                NULL
            );
            
            memset(curDateBuffer, '\0', sizeof(curDateBuffer)/sizeof(curDateBuffer[0]));
            ReleaseDC(hwnd, hdc);
        }
        break;
        
        case WM_COMMAND: {
            int control_id = LOWORD(wParam);
            int notification = HIWORD(wParam);
            HWND hwndControl = (HWND)lParam;
            HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_LOG);
            
            FILE* logFile;
            // Adjust size as needed
            char editBuffer[4096];
            char errorBuffer[1024];
            
            if (notification == BN_CLICKED) {
                if(control_id ==1){
                    //MessageBeep(MB_OK);
                    snprintf(curDateBuffer, sizeof(curDateBuffer), "../logs/%04d-%02d-%02d.md", localTime.wYear, localTime.wMonth, localTime.wDay);
                    if((logFile = fopen(curDateBuffer, "w")) == NULL)
                    {
                        MessageBox(hwnd, "The log file was not opened", "Could not write to file", MB_ICONEXCLAMATION | MB_OK);
                    }
                    else
                    {
                        GetWindowText(hEdit, editBuffer, sizeof(editBuffer));
                        if((fprintf(logFile, editBuffer) < 0))
                        {
                            snprintf(errorBuffer, sizeof(errorBuffer), "The log could not be written \n\n errNo: %d\n errMsg: %s", errno, strerror(errno));
                            MessageBox(hwnd, errorBuffer, "Could not write to file", MB_ICONEXCLAMATION | MB_OK);
                        }
                    }
                    
                    // Close file
                    fclose(logFile);
                    //MessageBox(hwnd, curDateBuffer, "File Name", MB_OK | MB_ICONINFORMATION);
                    
                }
                
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &rcClient);

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

            // Clear background in memory DC
            FillRect(memDC, &rcClient, (HBRUSH)(COLOR_WINDOW + 1));

            SetBkMode(memDC, TRANSPARENT);

            // Final blit
            BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, memDC, 0, 0, SRCCOPY);

            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);

            EndPaint(hwnd, &ps);
        }
        break;

        case WM_ERASEBKGND:
            return 1; // Prevent flicker

        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;

        case WM_DESTROY:
            DeleteDC(hdcCompat);
            
            PostQuitMessage(0);
        break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_COMPOSITED | WS_EX_CLIENTEDGE,
        g_szClassName,
        "Daily Markdown Logger",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 500,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
