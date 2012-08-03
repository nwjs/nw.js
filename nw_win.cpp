// Copyright (c) 2010 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "nw/nw.h"

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <direct.h>
#include <sstream>
#include <string>

#include "base/values.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_runnable.h"
#include "nw/client_handler.h"
#include "nw/client_switches.h"
#include "nw/resource.h"
#include "nw/string_util.h"

#define MAX_LOADSTRING 100
#define MAX_URL_LENGTH  255
#define BUTTON_WIDTH 72
#define URLBAR_HEIGHT  24

// Global Variables:
HINSTANCE hInst;   // current instance
TCHAR szTitle[MAX_LOADSTRING];  // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];  // the main window class name

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// The global ClientHandler reference.
extern CefRefPtr<ClientHandler> g_handler;

#if defined(OS_WIN)
// Add Common Controls to the application manifest because it's required to
// support the default tooltip implementation.
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")  // NOLINT(whitespace/line_length)
#endif

// Program entry point function.
int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  CefMainArgs main_args(hInstance);
  CefRefPtr<ClientApp> app(new ClientApp);

  // Execute the secondary process, if any.
  int exit_code = CefExecuteProcess(main_args, app.get());
  if (exit_code >= 0)
    return exit_code;

  // Parse command line arguments. The passed in values are ignored on Windows.
  AppInitCommandLine(0, NULL);

  CefSettings settings;

  // Populate the settings based on command line arguments.
  AppGetSettings(settings, app);

  // Initialize CEF.
  CefInitialize(main_args, settings, app.get());

  HACCEL hAccelTable;

  // Initialize global strings
  LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadString(hInstance, IDC_CEFCLIENT, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // Perform application initialization
  if (!InitInstance (hInstance, nCmdShow))
    return FALSE;

  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CEFCLIENT));

  int result = 0;

  if (!settings.multi_threaded_message_loop) {
    // Run the CEF message loop. This function will block until the application
    // recieves a WM_QUIT message.
    CefRunMessageLoop();
  } else {
    MSG msg;

    // Run the application message loop.
    while (GetMessage(&msg, NULL, 0, 0)) {
      if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }

    result = static_cast<int>(msg.wParam);
  }

  // Shut down CEF.
  CefShutdown();

  return result;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this
//    function so that the application will get 'well formed' small icons
//    associated with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = WndProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = hInstance;
  wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CEFCLIENT));
  wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName  = MAKEINTRESOURCE(IDC_CEFCLIENT);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  HWND hWnd;

  hInst = hInstance;  // Store instance handle in our global variable

  // Get window settings
  base::DictionaryValue *window_manifest = NULL;
  AppGetManifest()->GetDictionary(nw::kmWindow, &window_manifest);

  int width = 800;
  int height = 600;
  int x = CW_USEDEFAULT;
  int y = 0;
  if (window_manifest) {
    // window.width & window.height
    window_manifest->GetInteger(nw::kmWidth, &width);
    window_manifest->GetInteger(nw::kmHeight, &height);

    // window.position
    std::string position;
    if (window_manifest->GetString(nw::kmPosition, &position)) {
      if (position == "center") {
        x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
        y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
      } else if (position == "mouse") {
        POINT point;
        GetCursorPos(&point);
        x = point.x - width / 2;
        y = point.y - height / 2;
      }
    }
  }

  hWnd = CreateWindow(szWindowClass, szTitle,
                      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, x, y,
                      width, height, NULL, NULL, hInstance, NULL);

  if (!hWnd)
    return FALSE;

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
  static HWND backWnd = NULL, forwardWnd = NULL, reloadWnd = NULL,
      stopWnd = NULL, editWnd = NULL;
  static WNDPROC editWndOldProc = NULL;

  // Static members used for the find dialog.
  static FINDREPLACE fr;
  static WCHAR szFindWhat[80] = {0};
  static WCHAR szLastFindWhat[80] = {0};
  static bool findNext = false;
  static bool lastMatchCase = false;

  // How many buttons on toolbar
  static int nb = 0;

  int wmId, wmEvent;
  PAINTSTRUCT ps;
  HDC hdc;

  if (hWnd == editWnd) {
    // Callback for the edit window
    switch (message) {
    case WM_CHAR:
      if (wParam == VK_RETURN && g_handler.get()) {
        // When the user hits the enter key load the URL
        CefRefPtr<CefBrowser> browser = g_handler->GetBrowser();
        wchar_t strPtr[MAX_URL_LENGTH+1] = {0};
        *((LPWORD)strPtr) = MAX_URL_LENGTH;
        LRESULT strLen = SendMessage(hWnd, EM_GETLINE, 0, (LPARAM)strPtr);
        if (strLen > 0) {
          strPtr[strLen] = 0;
          browser->GetMainFrame()->LoadURL(strPtr);
        }

        return 0;
      }
    }

    return (LRESULT)CallWindowProc(editWndOldProc, hWnd, message, wParam,
                                   lParam);
  } else {
    // Callback for the main window
    switch (message) {
    case WM_CREATE: {
      // Create the single static handler class instance
      g_handler = new ClientHandler();
      g_handler->SetMainHwnd(hWnd);

      // Get window settings
      base::DictionaryValue *window_manifest = NULL;
      AppGetManifest()->GetDictionary(nw::kmWindow, &window_manifest);

      // Create the child windows used for navigation
      RECT rect;
      int x = 0;

      GetClientRect(hWnd, &rect);

      bool is_toolbar_open = true;
      if (window_manifest)
        window_manifest->GetBoolean(nw::kmToolbar, &is_toolbar_open);

      if (is_toolbar_open) {
        backWnd = CreateWindow(L"BUTTON", L"Back",
                                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON
                                | WS_DISABLED, x, 0, BUTTON_WIDTH, URLBAR_HEIGHT,
                                hWnd, (HMENU) IDC_NAV_BACK, hInst, 0);
        x += BUTTON_WIDTH;
        nb++;

        forwardWnd = CreateWindow(L"BUTTON", L"Forward",
                                  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON
                                  | WS_DISABLED, x, 0, BUTTON_WIDTH,
                                  URLBAR_HEIGHT, hWnd, (HMENU) IDC_NAV_FORWARD,
                                  hInst, 0);
        x += BUTTON_WIDTH;
        nb++;

        reloadWnd = CreateWindow(L"BUTTON", L"Reload",
                                  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON
                                  | WS_DISABLED, x, 0, BUTTON_WIDTH,
                                  URLBAR_HEIGHT, hWnd, (HMENU) IDC_NAV_RELOAD,
                                  hInst, 0);
        x += BUTTON_WIDTH;
        nb++;

        stopWnd = CreateWindow(L"BUTTON", L"Stop",
                                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON
                                | WS_DISABLED, x, 0, BUTTON_WIDTH, URLBAR_HEIGHT,
                                hWnd, (HMENU) IDC_NAV_STOP, hInst, 0);
        x += BUTTON_WIDTH;
        nb++;

        editWnd = CreateWindow(L"EDIT", 0,
                                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT |
                                ES_AUTOVSCROLL | ES_AUTOHSCROLL| WS_DISABLED,
                                x, 0, rect.right - BUTTON_WIDTH * nb,
                                URLBAR_HEIGHT, hWnd, 0, hInst, 0);
      }

      // Assign the edit window's WNDPROC to this function so that we can
      // capture the enter key
      editWndOldProc =
          reinterpret_cast<WNDPROC>(GetWindowLongPtr(editWnd, GWLP_WNDPROC));
      SetWindowLongPtr(editWnd, GWLP_WNDPROC,
          reinterpret_cast<LONG_PTR>(WndProc));
      g_handler->SetEditHwnd(editWnd);
      g_handler->SetButtonHwnds(backWnd, forwardWnd, reloadWnd, stopWnd);

      if (is_toolbar_open)
        rect.top += URLBAR_HEIGHT;

      CefWindowInfo info;
      CefBrowserSettings settings;

      // Populate the settings based on command line arguments.
      AppGetBrowserSettings(settings);

      // Initialize window info to the defaults for a child window
      info.SetAsChild(hWnd, rect);

      // Creat the new child browser window
      CefBrowserHost::CreateBrowser(info, g_handler.get(),
          g_handler->GetStartupURL(), settings);

      return 0;
    }

    case WM_COMMAND: {
      CefRefPtr<CefBrowser> browser;
      if (g_handler.get())
        browser = g_handler->GetBrowser();

      wmId    = LOWORD(wParam);
      wmEvent = HIWORD(wParam);
      // Parse the menu selections:
      switch (wmId) {
      case ID_WARN_CONSOLEMESSAGE:
        if (g_handler.get()) {
          std::wstringstream ss;
          ss << L"Console messages will be written to "
              << std::wstring(CefString(g_handler->GetLogFile()));
          MessageBox(hWnd, ss.str().c_str(), L"Console Messages",
              MB_OK | MB_ICONINFORMATION);
        }
        return 0;
      case ID_WARN_DOWNLOADCOMPLETE:
      case ID_WARN_DOWNLOADERROR:
        if (g_handler.get()) {
          std::wstringstream ss;
          ss << L"File \"" <<
              std::wstring(CefString(g_handler->GetLastDownloadFile())) <<
              L"\" ";

          if (wmId == ID_WARN_DOWNLOADCOMPLETE)
            ss << L"downloaded successfully.";
          else
            ss << L"failed to download.";

          MessageBox(hWnd, ss.str().c_str(), L"File Download",
              MB_OK | MB_ICONINFORMATION);
        }
        return 0;
      case IDC_NAV_BACK:   // Back button
        if (browser.get())
          browser->GoBack();
        return 0;
      case IDC_NAV_FORWARD:  // Forward button
        if (browser.get())
          browser->GoForward();
        return 0;
      case IDC_NAV_RELOAD:  // Reload button
        if (browser.get())
          browser->Reload();
        return 0;
      case IDC_NAV_STOP:  // Stop button
        if (browser.get())
          browser->StopLoad();
        return 0;
      }
      break;
    }

    case WM_PAINT:
      hdc = BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);
      return 0;

    case WM_SETFOCUS:
      if (g_handler.get() && g_handler->GetBrowser()) {
        // Pass focus to the browser window
        CefWindowHandle hwnd =
            g_handler->GetBrowser()->GetHost()->GetWindowHandle();
        if (hwnd)
          PostMessage(hwnd, WM_SETFOCUS, wParam, NULL);
      }
      return 0;

    case WM_SIZE:
      // Minimizing resizes the window to 0x0 which causes our layout to go all
      // screwy, so we just ignore it.
      if (wParam != SIZE_MINIMIZED && g_handler.get() &&
          g_handler->GetBrowser()) {
        CefWindowHandle hwnd =
            g_handler->GetBrowser()->GetHost()->GetWindowHandle();
        if (hwnd) {
          // Resize the browser window and address bar to match the new frame
          // window size
          RECT rect;
          GetClientRect(hWnd, &rect);
          rect.top += URLBAR_HEIGHT;


          HDWP hdwp = BeginDeferWindowPos(1);
          if (editWnd) {
            int urloffset = rect.left + BUTTON_WIDTH * nb;
            hdwp = DeferWindowPos(hdwp, editWnd, NULL, urloffset,
              0, rect.right - urloffset, URLBAR_HEIGHT, SWP_NOZORDER);
          }
          hdwp = DeferWindowPos(hdwp, hwnd, NULL,
            rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
            SWP_NOZORDER);
          EndDeferWindowPos(hdwp);
        }
      }
      break;

    case WM_GETMINMAXINFO:
      if (g_handler.get() && g_handler->GetBrowser()) {
        CefWindowHandle hwnd =
            g_handler->GetBrowser()->GetHost()->GetWindowHandle();

        if (hwnd) {
          MINMAXINFO* minMaxInfo = (MINMAXINFO*)message->lParam;

          // Get window settings
          base::DictionaryValue *window_manifest = NULL;
          if (!AppGetManifest()->GetDictionary(nw::kmWindow, &window_manifest))
            break;

          bool changed = false;
          int tmp;
          if (window_manifest->GetInteger(nw::kmMinWidth, &tmp)) {
            changed = true;
            minMaxInfo->ptMinTrackSize.x = tmp;
          }
          if (window_manifest->GetInteger(nw::kmMinHeight, &tmp)) {
            changed = true;
            minMaxInfo->ptMinTrackSize.y = tmp;
          }
          if (window_manifest->GetInteger(nw::kmMaxWidth, &tmp)) {
            changed = true;
            minMaxInfo->ptMaxTrackSize.x = tmp;
          }
          if (window_manifest->GetInteger(nw::kmMaxHeight, &tmp)) {
            changed = true;
            minMaxInfo->ptMaxTrackSize.y = tmp;
          }

          if (!changed)
            break;

          return 0;
        }
      }
      break;

    case WM_ERASEBKGND:
      if (g_handler.get() && g_handler->GetBrowser()) {
        CefWindowHandle hwnd =
            g_handler->GetBrowser()->GetHost()->GetWindowHandle();
        if (hwnd) {
          // Dont erase the background if the browser window has been loaded
          // (this avoids flashing)
          return 0;
        }
      }
      break;

    case WM_CLOSE:
      if (g_handler.get()) {
        CefRefPtr<CefBrowser> browser = g_handler->GetBrowser();
        if (browser.get()) {
          // Let the browser window know we are about to destroy it.
          browser->GetHost()->ParentWindowWillClose();
        }
      }
      break;

    case WM_DESTROY:
      // The frame window has exited
      PostQuitMessage(0);
      return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
  }
}
