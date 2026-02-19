# Rest Pomodoro Timer

A lightweight, native C++ Pomodoro Timer for Windows.
It sits in your system tray, notifies you when to work and rest, and supports custom icons.

This application focuses on reminding users to **take regular breaks and stay away from the screen** to prevent eye strain and fatigue. (Because my sight is getting worse by overusing the computer)

## Features

- **System Tray Icon**: Runs in the background. Hover to see current status and remaining time.
- **Notifications**: Native Windows balloon/toast notifications for work/break transitions.
- **Customizable Timer**: Set your preferred work and rest durations.
- **Auto-Start on boot**: (Optional) Automatically start with Windows.
- **Custom Icon**: Support for `.png` or `.ico` custom icons.
- **Pause/Resume**: Pause the timer when needed.

## How to Use

1. **Launch**: Run `pomodoro.exe`.
2. **Open Settings**: Left-click the tray icon (or Right-click -> Settings).
3. **Configure**:
    - Set **Work Duration** (minutes).
    - Set **Rest Duration** (minutes).
    - Toggle **Auto-start on boot**.
4. **Control**:
    - **Pause/Resume**: Use the button in Settings or the Right-click menu.
    - **Exit**: Right-click the tray icon -> Exit.
5. **Custom Icon**:
    - Place a file named `icon.png` (recommended) or `icon.ico` in the same directory as the executable.
    - Restart the application.

## Build Instructions

### Prerequisites

- MinGW (GCC) or Visual Studio (MSVC) C++ compiler.
- Windows SDK libraries (GDI+, Shell32, User32, Advapi32).

### Building with MinGW (g++)

```bash
g++ -o pomodoro.exe main.cpp -lgdi32 -luser32 -lshell32 -ladvapi32 -lgdiplus -mwindows
```

### Building with Visual Studio (MSVC)

```cmd
cl.exe /DUNICODE /D_UNICODE main.cpp user32.lib shell32.lib gdi32.lib advapi32.lib gdiplus.lib /link /SUBSYSTEM:WINDOWS /OUT:pomodoro.exe
```

**Note**: Ensure you turn off the `pomodoro.exe` before building.

## Resources

Icon: [creazilla.com/zh-tw/media/emoji/44512/yuan-quan](https://creazilla.com/zh-tw/media/emoji/44512/yuan-quan)

## License

This project is licensed under the MIT License.
