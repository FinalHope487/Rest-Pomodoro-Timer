#include "globals.h"
#include "timer_logic.h"
#include "ui.h"
#include "resource.h"
// Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    
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
    wcSettings.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APP_ICON));
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

    // Shutdown
    
    return (int)msg.wParam;
}
