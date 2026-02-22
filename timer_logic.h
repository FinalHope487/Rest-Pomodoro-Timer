#pragma once
#include "globals.h"
#include <string>

void InitNotifyIcon(HWND hwnd);
void RemoveNotifyIcon();
void ShowNotification(const std::wstring& title, const std::wstring& message);
void LoadSettings();
void SaveSettings();
void SetAutoStart(bool enable);
void StartPhase(bool working);
void TogglePause();
void UpdateTooltip();
HICON LoadIconFromPNG(const std::wstring& filePath);
