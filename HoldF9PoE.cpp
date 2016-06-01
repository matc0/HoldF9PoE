#include <Windows.h>
#include <string>

HWINEVENTHOOK hEvent;

INPUT ip;

VOID CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook,
                                   DWORD dwEvent,
                                   HWND hwnd,
                                   LONG idObject,
                                   LONG idChild,
                                   DWORD dwEventThread,
                                   DWORD dwmsEventTime) {
  int bufsize = GetWindowTextLength(hwnd);
  std::basic_string<TCHAR> t(bufsize, 0);
  GetWindowText(hwnd, &t[0], bufsize + 1);

  std::string title;

  #ifndef UNICODE
    title = t;
  #else
    std::wstring wStr = t;
    title = std::string(wStr.begin(), wStr.end());
  #endif

  if (title == "Path of Exile") {
    // Press the "f9" key
    ip.ki.wVk = 0x78; // virtual-key code for the "f9" key
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));
  }
  else {
    // Release the "f9" key
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
  }
}

void unhookEvent() {
  UnhookWinEvent(hEvent);
}

int main() {
  hEvent = SetWinEventHook(EVENT_SYSTEM_FOREGROUND,
                            EVENT_SYSTEM_FOREGROUND,
                            NULL,
                            WinEventProcCallback,
                            0,
                            0,
                            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

  atexit(unhookEvent);

  ip.type = INPUT_KEYBOARD;
  ip.ki.wScan = 0;
  ip.ki.time = 0;
  ip.ki.dwExtraInfo = 0;

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}
