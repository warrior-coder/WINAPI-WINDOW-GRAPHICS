#include <windows.h>
#define ABS(a) ( (a) < 0 ? -(a) : (a) )
#define PI 3.14159265358979323846

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
    RGB pen_color = {};
    
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

    void clear(char COMMAND)
    {
        if (COMMAND == -1)
        {
            int i;

            for (short y = 0; y < height; y++)
            {
                for (short x = 0; x < width; x++)
                {
                    i = y * width + x;

                    buffer[i].R = (BYTE)(255 - (float)x / width * 255);
                    buffer[i].G = (BYTE)((float)x / width * 255);
                    buffer[i].B = (BYTE)((float)y / height * 255);
                }
            }
        }
    }

    void set_rect(short x1, short y1, short x2, short y2)
    {
        for (short y = y1; y <= y2; y++)
        {
            for (short x = x1; x <= x2; x++)
            {
                set_pixel(x, y);
            }
        }
    }

    void set_circle(short x0, short y0, short R)
    {
        for (short x = x0 - R; x < x0 + R; x++)
        {
            for (short y = y0 - R; y < y0 + R; y++)
            {
                if (((x-x0)*(x-x0) + (y-y0)*(y-y0) - R*R) < 0) set_pixel(x, y);
            }
        }
    }

    void set_pixel(short x, short y)
    {
        int i = y * width + x;

        if (i < height * width)
        {
            buffer[i].R = pen_color.R;
            buffer[i].G = pen_color.G;
            buffer[i].B = pen_color.B;
        }
    }

    void set_line(short x1, short y1, short x2, short y2)
    {
        short dx = ABS(x2 - x1);        // Absolute delta X
        short dy = ABS(y2 - y1);        // Absolute delta Y
        short sx = (x2 >= x1) ? 1 : -1; // Step X
        short sy = (y2 >= y1) ? 1 : -1; // Step Y

        if (dx > dy)
        {
            short d = (dy << 1) - dx, d1 = dy << 1, d2 = (dy - dx) << 1;

            for (short x = x1+sx, y = y1, i = 1; i < dx; i++, x += sx)
            {
                if (d > 0)
                {
                    d += d2;
                    y += sy;
                }
                else d += d1;

                set_pixel(x ,y);
            }
        }
        else
        {
            short d = (dx << 1) - dy, d1 = dx << 1, d2 = (dx - dy) << 1;

            for (short x = x1, y = y1+sy, i = 1; i < dy; i++, y += sy)
            {
                if (d > 0)
                {
                    d += d2;
                    x += sx;
                }
                else d += d1;

                set_pixel(x, y);
            }
        }

        set_pixel(x1, y1);
        set_pixel(x2, y2);
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
    g_hwnd = CreateWindowExW(0, L"MyWindow", L"My Window", WS_OVERLAPPED | WS_SYSMENU, 0, 0, frame.width, frame.height, nullptr, nullptr, hInstance, nullptr);

    // Show the window
    ShowWindow(g_hwnd, SW_SHOWNORMAL);
    //ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL); // SW_HIDE or SW_SHOWNORMAL - Hide or Show console

    // Window message
    MSG msg = {};

    // Frame draw
    frame.clear(-1);
    frame.pen_color = { 255,255,255 };
    frame.set_circle(frame.width/2, frame.height/2, 50);
    frame.set_rect(10, 10, 60, 60);
    frame.set_line(350, 10, 350, 200);
    frame.set_line(350, 200, 200, 10);
    frame.set_line(200, 10, 350, 10);

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