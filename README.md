# Rest Pomodoro Timer

A lightweight, native C++ Pomodoro Timer for Windows.
It sits in your system tray, notifies you when to work and rest, and supports custom icons.

This application focuses on reminding users to **take regular breaks and stay away from the screen** to prevent eye strain and fatigue. (Because my sight is getting worse by overusing the computer)

## Features

- **System Tray Icon**: Runs in the background. Hover to see current status and remaining time.

<img width="224" height="106" alt="edited-photo (3)" src="https://github.com/user-attachments/assets/d282abd4-31ae-4bbc-99dd-b92c045918db" />

- **Notifications**: Native Windows balloon/toast notifications for work/break transitions.

<img width="466" height="145" alt="edited-photo (4)" src="https://github.com/user-attachments/assets/dd1ab075-fe85-494b-a27e-ed2962ad2c4d" />
<img width="461" height="143" alt="edited-photo (5)" src="https://github.com/user-attachments/assets/c13ca454-81fa-4bac-88dd-119127a9cec7" />

- **Customizable Timer**: Set your preferred work and rest durations.
- **Auto-Start on boot**: (Optional) Automatically start with Windows.
- **Custom Icon**: Support for `.ico` custom icons.
- **Pause/Resume**: Pause the timer when needed.

<img width="377" height="290" alt="edited-photo (6)" src="https://github.com/user-attachments/assets/68762cad-1e99-4134-9e21-93186571ec07" />
<img width="215" height="115" alt="edited-photo (7)" src="https://github.com/user-attachments/assets/7e3e02c4-0c30-47ae-bbdb-7fdffefc0771" />

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
    - Place a file named `icon.ico` in the same directory with the project.
    - Recompile the project and restart the application.

## Build Instructions

### Prerequisites

- MinGW (GCC) or Visual Studio (MSVC) C++ compiler.
- Windows SDK libraries (GDI+, Shell32, User32, Advapi32).

### Clone the repo

```bash
git clone https://github.com/FinalHope487/Rest-Pomodoro-Timer
```

### Building with MinGW (g++)

```bash
windres resource.rc -o resource.o
g++ globals.cpp timer_logic.cpp ui.cpp main.cpp resource.o -o pomodoro.exe -mwindows -Wall -static
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
