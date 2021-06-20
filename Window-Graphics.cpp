#include <windows.h>
#include <cmath>

typedef struct
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
}RGB;

typedef struct
{
    unsigned char B;
    unsigned char G;
    unsigned char R;
    unsigned char A;
}RGB4;

typedef struct
{
    short x;
    short y;
}DOT_2D;

// Handle to the window (g_ - global)
HWND g_hwnd = nullptr;

// Handle to the device context
HDC g_hdc = nullptr;
HDC g_tmpDc = nullptr;

// Handle to the bitmap
HBITMAP g_hbm = nullptr;

// Window processing function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

class FRAME
{
private:
    RGB4* buffer;

public:
    short width;
    short height;
    
    FRAME(short frameWidth, short frameHeight)
    {
        width = frameWidth;
        height = frameHeight;
        int size = frameHeight * frameWidth;
        buffer = new RGB4[size];
    }

    ~FRAME()
    {
        delete[] buffer;
    }

    void clear(RGB color = { 255, 255, 255 })
    {
        int i;
        for (short y = 0; y < height; y++)
        {
            for (short x = 0; x < width; x++)
            {
                i = y * width + x;
                buffer[i].R = color.R;
                buffer[i].G = color.G;
                buffer[i].B = color.B;
            }
        }
    }

    void set_pixel(DOT_2D dot, RGB color = { 0, 0, 0 })
    {
        int i = dot.y * width + dot.x;

        if (i < height * width)
        {
            buffer[i].R = color.R;
            buffer[i].G = color.G;
            buffer[i].B = color.B;
        }
    }

    void set_line(DOT_2D dot1, DOT_2D dot2, RGB color = { 0, 0, 0 })
    {
        float x, y;
        DOT_2D dot;

        if (abs(dot2.x - dot1.x) > abs(dot2.y - dot1.y))
        {
            // Render though X
            if (dot1.x > dot2.x)
            {
                DOT_2D temp = dot1;
                dot1 = dot2;
                dot2 = temp;
            }
            for (x = dot1.x; x < dot2.x; x++)
            {
                y = (x - dot1.x) / (dot2.x - dot1.x) * (dot2.y - dot1.y) + dot1.y;
                dot.x = floor(x);
                dot.y = floor(y);
                set_pixel(dot, color);
            }
        }
        else
        {
            // Render though Y
            if (dot1.y > dot2.y)
            {
                DOT_2D temp = dot1;
                dot1 = dot2;
                dot2 = temp;
            }
            for (y = dot1.y; y < dot2.y; y++)
            {
                x = (y - dot1.y) / (dot2.y - dot1.y) * (dot2.x - dot1.x) + dot1.x;
                dot.x = floor(x);
                dot.y = floor(y);
                set_pixel(dot, color);
            }
        }
    }

    void print()
    {
        // Create Bitmap function creates a bitmap from array of pixels data and returns handle to the bitmap
        g_hbm = CreateBitmap(
            width,            // Bitmap width
            height,           // Bitmap height
            1,                // Number of color planes
            8 * sizeof(RGB4), // Number of bits required to identify the color 1 pixel
            buffer            // Pointer to array of pixels data
        );

        // Handle to the device context
        g_hdc = GetDC(g_hwnd);
        g_tmpDc = CreateCompatibleDC(g_hdc);

        //  Selects Object into the specified device context (DC)
        SelectObject(
            g_tmpDc, // Handle to the DC
            g_hbm    // Handle to the object to be selected
        );

        // Bit Built transfers a rectangle of pixels from one DC into another
        BitBlt(
            g_hdc,    // Destination DC
            0,      // X begin of destination rect
            0,      // Y begin of destination rect
            width,  // Width of the source and destination rect
            height, // Height of the source and destination rect
            g_tmpDc,  // Sourse DC
            0,      // X begin of source rect
            0,      // Y begin of source rect
            SRCCOPY // Source copy mode
        );

        // Clear memory
        DeleteObject(g_hbm);
        DeleteDC(g_tmpDc);
        ReleaseDC(g_hwnd, g_hdc);
    }
};


int main()
{
    FRAME frame(400, 400);
    
    // Handle to the application instance
    HINSTANCE hInstance = GetModuleHandleW(nullptr);
    
    // Register the window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyWindow";
    RegisterClassW(&wc);

    // Create the window
    g_hwnd = CreateWindowExW(0, L"MyWindow", L"My Window", WS_OVERLAPPED | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, frame.width, frame.height, nullptr, nullptr, hInstance, nullptr);
    
    // Show the window
    ShowWindow(g_hwnd, SW_SHOWNORMAL);
    //ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL); // SW_HIDE or SW_SHOWNORMAL - Hide or Show console

    // Window message
    MSG msg = {};

    // Frame draw
    frame.clear();
    frame.set_pixel({ 50, 50 });
    frame.set_line({ 100, 50 }, { 200, 200 });
    frame.set_line({ 250, 100 }, { 50, 300 });
    frame.print();

    // Main loop
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