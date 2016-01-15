import win32gui, win32con

class WinManager:

  def close_window(self):
    hwnd = win32gui.GetForegroundWindow()
    win32gui.SendMessage(hwnd, win32con.WM_SYSCOMMAND, win32con.SC_CLOSE, 0)