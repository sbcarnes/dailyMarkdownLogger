#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "fileops.h"


const char g_szClassName[] = "myWindowClass";

typedef struct
{
    const char *promptText;
    const char *markdownHeading;
    int editControlId;
    HWND editHandle;
} LogField;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static RECT rcClient;
    
    static SYSTEMTIME localTime;
    static char curDateBuffer[128];
    
    static LogField fields[] =
    {
        {
            "What did I work on?",
            "What did I work on?",
            IDC_EDIT_WORKED_ON,
            NULL
        },
        {
            "What did I learn?",
            "What did I learn?",
            IDC_EDIT_LEARNED,
            NULL
        },
        {
            "What is the next concrete step?",
            "What is the next concrete step?",
            IDC_EDIT_NEXT_STEP,
            NULL
        }
    };
    
    static const size_t fieldCount = sizeof(fields) / sizeof(fields[0]);

    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *pCreate = (CREATESTRUCT *) lParam;
            HINSTANCE hInstance = pCreate->hInstance;
            
            HDC hdc = GetDC(hwnd);
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
            
            
            for (size_t i = 0; i < fieldCount; ++i)
            {
                int promptY = FIRST_FIELD_Y + (int)i * FIELD_VERTICAL_STEP;
                int editY = promptY + PROMPT_HEIGHT + PROMPT_EDIT_GAP;
                
                CreateWindow(
                    "STATIC",
                    fields[i].promptText,
                    WS_CHILD | WS_VISIBLE,
                    10, promptY, 250, 20,
                    hwnd,
                    NULL,
                    hInstance,
                    NULL
                );
                
                fields[i].editHandle = CreateWindow(
                    "EDIT",
                    NULL,
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
                    10, editY, 400, 75,
                    hwnd,
                    (HMENU)fields[i].editControlId,
                    hInstance,
                    NULL
                );
            }
            
            if (fields[0].editHandle == NULL)
            {
                MessageBox(
                    hwnd,
                    "Could not create log entry field,",
                    "Control Creation Error",
                    MB_OK | MB_ICONERROR
                    );
            }
            
            HWND saveButton = CreateWindow(
                "BUTTON",
                "Save",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                450, 100, 100, 30,
                hwnd,
                (HMENU)IDC_BUTTON_SAVE,
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
            
            // Adjust size as needed
            char editBuffer[EDIT_BUFFER_SIZE];
            char errorBuffer[ERROR_BUFFER_SIZE];
            
            if (notification == BN_CLICKED) {
                if(control_id == IDC_BUTTON_SAVE){
                    //MessageBeep(MB_OK);
                    GetLocalTime(&localTime);
                    GetWindowText(fields[0].editHandle, editBuffer, sizeof(editBuffer));
                    if (!saveLogToFile(&localTime, editBuffer, errorBuffer, sizeof(errorBuffer))){
                        MessageBox(hwnd, errorBuffer, "Error", MB_OK | MB_ICONERROR);
                        
                        fprintf(stderr, "%s\n", errorBuffer);
                    }
                }
                
                //InvalidateRect(hwnd, NULL, TRUE);
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
