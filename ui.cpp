#include "ui.h"
#include "timer_logic.h"
#include <string>

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
