#pragma once
#include "globals.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ShowContextMenu(HWND hwnd, POINT pt);
void ShowSettingsDialog(HWND hwnd);
