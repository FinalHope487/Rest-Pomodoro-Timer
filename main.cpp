#include "globals.h"
#include "timer_logic.h"
#include "ui.h"
#include <gdiplus.h>

using namespace Gdiplus;

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
