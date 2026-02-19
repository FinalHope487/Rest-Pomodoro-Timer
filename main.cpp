#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <gdiplus.h>

using namespace Gdiplus;

// Link with Shell32.lib and Gdiplus.lib
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "advapi32.lib") 
#pragma comment(lib, "gdiplus.lib") 

// Constants
#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_APP_ICON 1001
#define ID_TRAY_EXIT 1002
#define ID_TRAY_SETTINGS 1003
#define ID_TRAY_PAUSE_RESUME 1004

#define ID_TIMER_UPDATE 2003

// Settings Window Controls
#define ID_EDIT_WORK 3001
#define ID_EDIT_REST 3002
#define ID_CHECK_AUTOSTART 3003
#define ID_BTN_SAVE 3004
#define ID_BTN_CANCEL 3005
#define ID_BTN_PAUSE 3006

// Global Variables
HINSTANCE hInst;
HWND hMainWnd;
HWND hSettingsWnd = NULL;
NOTIFYICONDATA nid;

// Settings
bool isWorking = true;
bool isPaused = false;
int workDuration = 25; // Minutes
int restDuration = 5;  // Minutes
int remainingSeconds = 0;
bool autoStart = false;

// Function Prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SettingsWndProc(HWND, UINT, WPARAM, LPARAM);
void ShowContextMenu(HWND hwnd, POINT pt);
void InitNotifyIcon(HWND hwnd);
void RemoveNotifyIcon();
void ShowNotification(const std::wstring& title, const std::wstring& message);
void LoadSettings();
void SaveSettings();
void ShowSettingsDialog(HWND hwnd);
void SetAutoStart(bool enable);
void StartPhase(bool working);
void TogglePause();
void SetAutoStart(bool enable);
void StartPhase(bool working);
void TogglePause();
void UpdateTooltip();
HICON LoadIconFromPNG(const std::wstring& filePath);

// Global for GDI+
ULONG_PTR gdiplusToken;

// Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    
    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    LoadSettings();

    // Register Main Window Class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"PomodoroTimerClass";
    RegisterClassEx(&wc);

    // Register Settings Window Class
    WNDCLASSEX wcSettings = {0};
    wcSettings.cbSize = sizeof(wcSettings);
    wcSettings.lpfnWndProc = SettingsWndProc;
    wcSettings.hInstance = hInstance;
    wcSettings.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcSettings.lpszClassName = L"PomodoroSettingsClass";
    wcSettings.hIcon = LoadIconFromPNG(L"icon.png");
    if (!wcSettings.hIcon) wcSettings.hIcon = (HICON)LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
    if (!wcSettings.hIcon) wcSettings.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcSettings.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(&wcSettings);

    // Create Main Window (Hidden)
    hMainWnd = CreateWindowEx(0, L"PomodoroTimerClass", L"Pomodoro Timer", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    if (!hMainWnd) return 0;

    // Initialize Tray Icon
    InitNotifyIcon(hMainWnd);

    // Start Timer
    StartPhase(true);

    // Main Message Loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    RemoveNotifyIcon();
    
    // Shutdown GDI+
    GdiplusShutdown(gdiplusToken);
    
    return (int)msg.wParam;
}

// ---------------------------------------------------------
// Logic Functions
// ---------------------------------------------------------

void InitNotifyIcon(HWND hwnd) {
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = ID_TRAY_APP_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIconFromPNG(L"icon.png");
    if (!nid.hIcon) nid.hIcon = (HICON)LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
    if (!nid.hIcon) nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"Pomodoro Timer");
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void RemoveNotifyIcon() {
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void ShowNotification(const std::wstring& title, const std::wstring& message) {
    wcscpy_s(nid.szInfoTitle, title.c_str());
    wcscpy_s(nid.szInfo, message.c_str());
    nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON; // Use application icon
    nid.uFlags |= NIF_INFO; 
    Shell_NotifyIcon(NIM_MODIFY, &nid);
    nid.uFlags &= ~NIF_INFO; // Clear flag to prevent persistence
}

void StartPhase(bool working) {
    isWorking = working;
    remainingSeconds = (isWorking ? workDuration : restDuration) * 60;
    
    KillTimer(hMainWnd, ID_TIMER_UPDATE);
    if (!isPaused) {
        SetTimer(hMainWnd, ID_TIMER_UPDATE, 1000, NULL);
    }
    UpdateTooltip();
}

void TogglePause() {
    isPaused = !isPaused;
    if (isPaused) {
        KillTimer(hMainWnd, ID_TIMER_UPDATE);
        wcscpy_s(nid.szTip, L"Pomodoro: Paused");
        nid.uFlags = NIF_TIP;
        Shell_NotifyIcon(NIM_MODIFY, &nid);
    } else {
        SetTimer(hMainWnd, ID_TIMER_UPDATE, 1000, NULL);
        UpdateTooltip();
    }
}

void UpdateTooltip() {
    int min = remainingSeconds / 60;
    int sec = remainingSeconds % 60;
    std::wstring state = isWorking ? L"Working" : L"Resting";
    std::wstring text = L"Pomodoro: " + state + L" (" + std::to_wstring(min) + L":" + (sec < 10 ? L"0" : L"") + std::to_wstring(sec) + L")";
    
    wcscpy_s(nid.szTip, text.c_str());
    nid.uFlags = NIF_TIP;
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void LoadSettings() {
    std::wifstream infile("settings.ini");
    if (infile.is_open()) {
        infile >> workDuration >> restDuration >> autoStart;
        infile.close();
    }
}

void SaveSettings() {
    std::wofstream outfile("settings.ini");
    if (outfile.is_open()) {
        outfile << workDuration << L" " << restDuration << L" " << autoStart;
        outfile.close();
    }
    SetAutoStart(autoStart);
}

void SetAutoStart(bool enable) {
    HKEY hKey;
    const wchar_t* czSubkey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    if (RegOpenKeyEx(HKEY_CURRENT_USER, czSubkey, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        if (enable) {
            wchar_t path[MAX_PATH];
            GetModuleFileName(NULL, path, MAX_PATH);
            RegSetValueEx(hKey, L"PomodoroTimer", 0, REG_SZ, (BYTE*)path, (wcslen(path) + 1) * sizeof(wchar_t));
        } else {
            RegDeleteValue(hKey, L"PomodoroTimer");
        }
        RegCloseKey(hKey);
    }
}



HICON LoadIconFromPNG(const std::wstring& filePath) {
    Bitmap* bitmap = Bitmap::FromFile(filePath.c_str());
    if (bitmap && bitmap->GetLastStatus() == Ok) {
        HICON hIcon = NULL;
        bitmap->GetHICON(&hIcon);
        delete bitmap;
        return hIcon;
    }
    if (bitmap) delete bitmap;
    return NULL;
}

// ---------------------------------------------------------
// Window Procedures
// ---------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                ShowContextMenu(hwnd, pt);
            } else if (lParam == WM_LBUTTONUP) {
                ShowSettingsDialog(hwnd);
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_TRAY_EXIT:
                    DestroyWindow(hwnd);
                    break;
                case ID_TRAY_SETTINGS:
                    ShowSettingsDialog(hwnd);
                    break;
                case ID_TRAY_PAUSE_RESUME:
                    TogglePause();
                    break;
            }
            break;

        case WM_TIMER:
            if (wParam == ID_TIMER_UPDATE) {
                if (remainingSeconds > 0) {
                    remainingSeconds--;
                    UpdateTooltip();
                } else {
                    // Phase complete
                    KillTimer(hwnd, ID_TIMER_UPDATE);
                    
                    // Prevent re-entry or multiple triggers
                    static bool isProcessing = false;
                    if (isProcessing) break;
                    isProcessing = true;

                    if (isWorking) {
                         ShowNotification(L"Time to take a break!", L" ");
                         StartPhase(false); // Start rest
                    } else {
                         ShowNotification(L"Break time over!", L"Ready to focus again?");
                         StartPhase(true); // Start work
                    }
                    
                    isProcessing = false;
                }
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void ShowContextMenu(HWND hwnd, POINT pt) {
    HMENU hMenu = CreatePopupMenu();
    if (hMenu) {
        if (isPaused) {
             InsertMenu(hMenu, -1, MF_BYPOSITION, ID_TRAY_PAUSE_RESUME, L"Resume");
        } else {
             InsertMenu(hMenu, -1, MF_BYPOSITION, ID_TRAY_PAUSE_RESUME, L"Pause");
        }
        InsertMenu(hMenu, -1, MF_BYPOSITION, ID_TRAY_SETTINGS, L"Settings");
        InsertMenu(hMenu, -1, MF_BYPOSITION, ID_TRAY_EXIT, L"Exit");

        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(hMenu);
    }
}

// ---------------------------------------------------------
// Settings Window
// ---------------------------------------------------------

void ShowSettingsDialog(HWND hwnd) {
    if (IsWindow(hSettingsWnd)) {
        SetForegroundWindow(hSettingsWnd);
        return;
    }

    hSettingsWnd = CreateWindowEx(
        0, L"PomodoroSettingsClass", L"Settings",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 230,
        hwnd, NULL, hInst, NULL
    );
}

LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hEditWork, hEditRest, hCheckAuto, hBtnSave, hBtnCancel, hBtnPause;
    static HFONT hFont = NULL;

    switch (msg) {
        case WM_CREATE: 
        {
            // Create Font (Consolas, 14pt)
            hFont = CreateFont(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
                               OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
                               DEFAULT_PITCH | FF_SWISS, L"Consolas");

            // Labels 
            // Params: (ClassName, Text, Style, X, Y, Width, Height, Parent, Menu, Instance, Param)
            HWND hLblWork = CreateWindow(L"STATIC", L"Work Duration (min):", WS_VISIBLE | WS_CHILD, 40, 20, 150, 20, hwnd, NULL, hInst, NULL);
            HWND hLblRest = CreateWindow(L"STATIC", L"Rest Duration (min):", WS_VISIBLE | WS_CHILD, 40, 60, 150, 20, hwnd, NULL, hInst, NULL);
            
            SendMessage(hLblWork, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hLblRest, WM_SETFONT, (WPARAM)hFont, TRUE);

            // Edit Controls
            hEditWork = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 200, 20, 50, 20, hwnd, (HMENU)ID_EDIT_WORK, hInst, NULL);
            hEditRest = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 200, 60, 50, 20, hwnd, (HMENU)ID_EDIT_REST, hInst, NULL);
            
            SendMessage(hEditWork, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hEditRest, WM_SETFONT, (WPARAM)hFont, TRUE);

            // Set current values
            SetWindowText(hEditWork, std::to_wstring(workDuration).c_str());
            SetWindowText(hEditRest, std::to_wstring(restDuration).c_str());

            // Checkbox
            hCheckAuto = CreateWindow(L"BUTTON", L"Auto-start on boot", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 40, 100, 200, 20, hwnd, (HMENU)ID_CHECK_AUTOSTART, hInst, NULL);
            SendMessage(hCheckAuto, WM_SETFONT, (WPARAM)hFont, TRUE);
            if (autoStart) {
                SendMessage(hCheckAuto, BM_SETCHECK, BST_CHECKED, 0);
            }

            // Buttons
            hBtnSave = CreateWindow(L"BUTTON", L"Save", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 30, 140, 70, 30, hwnd, (HMENU)ID_BTN_SAVE, hInst, NULL);
            hBtnCancel = CreateWindow(L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD, 110, 140, 70, 30, hwnd, (HMENU)ID_BTN_CANCEL, hInst, NULL);
            hBtnPause = CreateWindow(L"BUTTON", isPaused ? L"Resume" : L"Pause", WS_VISIBLE | WS_CHILD, 190, 140, 70, 30, hwnd, (HMENU)ID_BTN_PAUSE, hInst, NULL);
            
            SendMessage(hBtnSave, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hBtnCancel, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hBtnPause, WM_SETFONT, (WPARAM)hFont, TRUE);
            break;
        }

        case WM_COMMAND: 
        {
            if (LOWORD(wParam) == ID_BTN_SAVE) {
                wchar_t buf[16];
                GetWindowText(hEditWork, buf, 16);
                int newWork = _wtoi(buf);
                GetWindowText(hEditRest, buf, 16);
                int newRest = _wtoi(buf);

                if (newWork > 0) workDuration = newWork;
                if (newRest > 0) restDuration = newRest;
                
                autoStart = (SendMessage(hCheckAuto, BM_GETCHECK, 0, 0) == BST_CHECKED);

                SaveSettings();
                
                // Reset timer with new settings
                StartPhase(true);
                
                MessageBox(hwnd, L"Settings Saved!", L"Info", MB_OK);
                DestroyWindow(hwnd);
            } else if (LOWORD(wParam) == ID_BTN_CANCEL) {
                DestroyWindow(hwnd);
            } else if (LOWORD(wParam) == ID_BTN_PAUSE) {
                TogglePause();
                SetWindowText(hBtnPause, isPaused ? L"Resume" : L"Pause");
            }
            break;
        }

        case WM_DESTROY:
            if (hFont) {
                DeleteObject(hFont);
                hFont = NULL;
            }
            hSettingsWnd = NULL;
            // Do NOT quit app, just close settings window
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
