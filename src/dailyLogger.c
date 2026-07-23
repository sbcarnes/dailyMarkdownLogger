#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "fileops.h"


const char g_szClassName[] = "myWindowClass";

static HWND dateLabelHandle = NULL;
static HWND saveButtonHandle = NULL;
static HWND statusLabelHandle = NULL;
static HFONT segoeFont = NULL;

typedef struct
{
    const char *promptText;
    const char *markdownHeading;
    int editControlId;
    HWND promptHandle;
    HWND editHandle;
} LogField;

static LogField fields[] =
{
    {
        "What did I work on?",
        "What did I work on?",
        IDC_EDIT_WORKED_ON,
        NULL,
        NULL
    },
    {
        "What did I learn?",
        "What did I learn?",
        IDC_EDIT_LEARNED,
        NULL,
        NULL
    },
    {
        "What is the next concrete step?",
        "What is the next concrete step?",
        IDC_EDIT_NEXT_STEP,
        NULL,
        NULL
    }
};

static const size_t fieldCount = sizeof(fields) / sizeof(fields[0]);

static void layoutControls(int clientWidth, int clientHeight);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static RECT rcClient;
    
    static SYSTEMTIME localTime;
    static char curDateBuffer[128];
    
    
    
    
    
    
    char answerBuffers[LOG_FIELD_COUNT][EDIT_BUFFER_SIZE];
    LogSection sections[LOG_FIELD_COUNT];

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
            
            segoeFont = CreateFont(
                20,
                0,
                0, 0,
                FW_NORMAL,
                FALSE, FALSE, FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE,
                TEXT("Segoe UI")
            );
            
            int windowWidth = rcClient.right - rcClient.left;
            int windowHeight = rcClient.bottom - rcClient.top;
            
            int dateLabelWidthBase = (windowWidth * DATE_WIDTH_PCT) / 100;
            int dateLabelHeightBase = (windowHeight * DATE_HEIGHT_PCT) / 100;
            
            dateLabelHandle = CreateWindow(
                "STATIC",
                curDateBuffer,
                WS_CHILD | WS_VISIBLE,
                WINDOW_MARGIN, WINDOW_MARGIN, dateLabelWidthBase, dateLabelHeightBase,
                hwnd,
                NULL,
                hInstance,
                NULL
            );
            
            SendMessage(dateLabelHandle, WM_SETFONT, (WPARAM)segoeFont, TRUE);
            
            
            for (size_t i = 0; i < fieldCount; ++i)
            {
                int promptY = FIRST_FIELD_Y + (int)i * FIELD_VERTICAL_STEP;
                int editY = promptY + PROMPT_HEIGHT + PROMPT_EDIT_GAP;
                
                fields[i].promptHandle = CreateWindow(
                    "STATIC",
                    fields[i].promptText,
                    WS_CHILD | WS_VISIBLE,
                    10, promptY, 250, 20,
                    hwnd,
                    NULL,
                    hInstance,
                    NULL
                );
                
                SendMessage(fields[i].promptHandle, WM_SETFONT, (WPARAM)segoeFont, TRUE);
                
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
                
                SendMessage(fields[i].editHandle, WM_SETFONT, (WPARAM)segoeFont, TRUE);
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
            int buttonXBase = windowWidth - WINDOW_MARGIN - BUTTON_WIDTH;
            int buttonYBase = windowHeight - WINDOW_MARGIN - BUTTON_HEIGHT;
            saveButtonHandle = CreateWindow(
                "BUTTON",
                "Save",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                buttonXBase, buttonYBase, BUTTON_WIDTH, BUTTON_HEIGHT,
                hwnd,
                (HMENU)IDC_BUTTON_SAVE,
                hInstance,
                NULL
            );
            SendMessage(saveButtonHandle, WM_SETFONT, (WPARAM)segoeFont, TRUE);
            
            statusLabelHandle = CreateWindow(
                "STATIC",
                "Ready",
                WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
                WINDOW_MARGIN, buttonYBase, 150, STATUS_HEIGHT,
                hwnd,
                (HMENU)IDC_STATUS_LABEL,
                hInstance,
                NULL
            );
            SendMessage(statusLabelHandle, WM_SETFONT, (WPARAM)segoeFont, TRUE);
            
            memset(curDateBuffer, '\0', sizeof(curDateBuffer)/sizeof(curDateBuffer[0]));
            ReleaseDC(hwnd, hdc);
        }
        break;
        
        case WM_COMMAND: {
            int control_id = LOWORD(wParam);
            int notification = HIWORD(wParam);
            
            // Adjust size as needed
            char errorBuffer[ERROR_BUFFER_SIZE];
            
            if (notification == BN_CLICKED) {
                if(control_id == IDC_BUTTON_SAVE){
                    //MessageBeep(MB_OK);
                    GetLocalTime(&localTime);
                    for (size_t i = 0; i < fieldCount; ++i)
                    {
                        GetWindowText(
                            fields[i].editHandle,
                            answerBuffers[i],
                            sizeof(answerBuffers[i])
                        );
                        
                        sections[i].heading = fields[i].markdownHeading;
                        sections[i].text = answerBuffers[i];
                    }
                    SetWindowText(statusLabelHandle, "Saved successfully.");
                    if (!saveLogToFile(&localTime, sections, fieldCount, errorBuffer, sizeof(errorBuffer))){
                        MessageBox(hwnd, errorBuffer, "Error", MB_OK | MB_ICONERROR);
                        SetWindowText(statusLabelHandle, "Save failed.");
                        fprintf(stderr, "%s\n", errorBuffer);
                    }
                }
                
                //InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (notification == EN_CHANGE)
            {
                if (control_id == IDC_EDIT_WORKED_ON ||
                    control_id == IDC_EDIT_LEARNED ||
                    control_id == IDC_EDIT_NEXT_STEP)
                {
                    SetWindowText(statusLabelHandle, "Unsaved changes");
                }
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
        case WM_SIZE:
        {
            if (wParam == SIZE_MINIMIZED)
            {
                break;
            }
            
            int clientWidth = LOWORD(lParam);
            int clientHeight = HIWORD(lParam);
            
            layoutControls(clientWidth, clientHeight);
        }
        break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;

        case WM_DESTROY:
            if (segoeFont != NULL)
            {
                DeleteObject(segoeFont);
                segoeFont = NULL;
            }
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

static void layoutControls(int clientWidth, int clientHeight)
{
    int contentWidth = clientWidth - (2 * WINDOW_MARGIN);
    int contentHeight = clientHeight - (2 * WINDOW_MARGIN);
    
    int dateLabelWidthNew = (clientWidth * DATE_WIDTH_PCT) / 100;
    int dateLabelHeightNew = (clientHeight * DATE_HEIGHT_PCT) / 100;
    
    int buttonX = clientWidth - WINDOW_MARGIN - BUTTON_WIDTH;
    int buttonY = clientHeight - WINDOW_MARGIN - BUTTON_HEIGHT;
    
    int statusWidth = buttonX - WINDOW_MARGIN - STATUS_BUTTON_GAP;
    
    int currentY = WINDOW_MARGIN + dateLabelHeightNew + DATE_TO_FIELD_GAP;
    
    MoveWindow(dateLabelHandle, WINDOW_MARGIN, WINDOW_MARGIN, dateLabelWidthNew, DATE_DISPLAY_HEIGHT, TRUE);
    
    for (size_t i = 0; i < fieldCount; ++i)
    {
        MoveWindow(fields[i].promptHandle, WINDOW_MARGIN, currentY, contentWidth, PROMPT_HEIGHT, TRUE);
        currentY += PROMPT_HEIGHT + PROMPT_EDIT_GAP;
        MoveWindow(fields[i].editHandle, WINDOW_MARGIN, currentY, contentWidth, EDIT_HEIGHT, TRUE);
        currentY += EDIT_HEIGHT + PROMPT_EDIT_GAP;
    }
    
    MoveWindow(saveButtonHandle, buttonX, buttonY, BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
    MoveWindow(statusLabelHandle, WINDOW_MARGIN, buttonY, statusWidth, BUTTON_HEIGHT, TRUE);
}
