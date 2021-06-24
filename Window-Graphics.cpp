#include "graphics.h"

// Window processing function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

int main()
{
    // Handle to the application instance
    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    // Register the window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyWindow";
    RegisterClassW(&wc);

    // Create the window
    const short windowWidth = 400;
    const short windowHeight = 400;
    HWND hwnd = CreateWindowExW(0, L"MyWindow", L"My Window", WS_OVERLAPPED | WS_SYSMENU, 0, 0, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);
    
    // Show the window
    ShowWindow(hwnd, SW_SHOWNORMAL);
    //ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL); // SW_HIDE or SW_SHOWNORMAL - Hide or Show console

    // Window message
    MSG msg = {};

    /* Frame draw */
    FRAME frame(windowWidth, windowHeight, hwnd);
    frame.clear();
    frame.print();

    // Main loop
    int i = 0;
    while (GetKeyState(VK_ESCAPE) >= 0)
    {
        // Processing window messages
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            DispatchMessageW(&msg);
            if (msg.message == WM_QUIT) break;
        }  
    }
    
    return 0;
}