#include "graphics.h"

// Window processing function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main()
{
    // Register the window class
    WNDCLASS wc = {};
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    const wchar_t CLASS_NAME[] = L"WindowClass";

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window
    const short windowWidth = 400;
    const short windowHeight = 400;
    HWND hwnd = CreateWindow(CLASS_NAME, L"Window Graphics", WS_OVERLAPPEDWINDOW, 0, 0, windowWidth+16, windowHeight+39, nullptr, nullptr, hInstance, nullptr);
    
    ShowWindow(hwnd, SW_SHOWNORMAL);
    //ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL); // SW_HIDE or SW_SHOWNORMAL - Hide or Show console

    // Window message
    MSG msg = {};

    /** FRAME DRAW **/
    FRAME frame(windowWidth, windowHeight, hwnd);
    frame.clear({ 0,255,255 });
    frame.pen_color = { 255,255,0 };
    frame.set_circle(200, 200, 200);
    frame.print();

    /** MAIN LOOP **/
    while (GetKeyState(VK_ESCAPE) >= 0)
    {
        // Processing window messages
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) break;
        }

        // Code ...
    }
    
    return 0;
}