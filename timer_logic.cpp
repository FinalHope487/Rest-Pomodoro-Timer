#include "timer_logic.h"
#include <fstream>
#include <sstream>
#include <shellapi.h>
#include <gdiplus.h>

using namespace Gdiplus;

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
    if (nid.hIcon) {
        DestroyIcon(nid.hIcon);
        nid.hIcon = NULL;
    }
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
