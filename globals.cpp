#include "globals.h"

// Global Variables Definitions
HINSTANCE hInst = NULL;
HWND hMainWnd = NULL;
HWND hSettingsWnd = NULL;
NOTIFYICONDATA nid = {0};

// Settings
bool isWorking = true;
bool isPaused = false;
int workDuration = 25; // Minutes
int restDuration = 5;  // Minutes
int remainingSeconds = 0;
bool autoStart = false;
