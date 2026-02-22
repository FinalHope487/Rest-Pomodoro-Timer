#pragma once

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <string>

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

// Global Variables Extern Declarations
extern HINSTANCE hInst;
extern HWND hMainWnd;
extern HWND hSettingsWnd;
extern NOTIFYICONDATA nid;

// Settings
extern bool isWorking;
extern bool isPaused;
extern int workDuration; // Minutes
extern int restDuration;  // Minutes
extern int remainingSeconds;
extern bool autoStart;
