#include <windows.h>

struct RGB
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
};
struct DOT_2D
{
    short x;
    short y;
};
struct FRAME
{
    int width;
    int height;
    RGBQUAD* buffer;

    FRAME(int frameWidth, int frameHeight)
    {
        width = frameWidth;
        height = frameHeight;
        buffer = new RGBQUAD[frameHeight * frameWidth];
    }

    ~FRAME()
    {
        delete[] buffer;
    }

    void clear(RGB color = { 0, 0, 0 })
    {
        int i;
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                i = y * width + x;
                buffer[i].rgbRed = color.R;
                buffer[i].rgbGreen = color.G;
                buffer[i].rgbBlue = color.B;
            }
        }
    }

    void set_pixel(DOT_2D dot, RGB color)
    {
        int i = dot.y * width + dot.x;

        if (i < height * width)
        {
            buffer[i].rgbRed = color.R;
            buffer[i].rgbGreen = color.G;
            buffer[i].rgbBlue = color.B;
        }
    }

    void set_line(DOT_2D dot1, DOT_2D dot2, RGB color)
    {

    }

    void print(HDC& dc, HDC& tmpDc, HBITMAP& bm)
    {
        // Create Bitmap function creates a bitmap from array of pixels data
        bm = CreateBitmap(
            width,               // Bitmap width
            height,              // Bitmap height
            1,                   // Number of color planes
            8 * sizeof(RGBQUAD), // Number of bits required to identify the color 1 pixel
            buffer               // Pointer to array of pixels data
        );

        //  Selects Object into the specified device context (DC)
        SelectObject(
            tmpDc, // Handle to the DC
            bm     // Handle to the object to be selected
        );

        // Bit Built transfers a rectangle of pixels from one DC into another
        BitBlt(
            dc,     // Destination DC
            0,      // X begin of destination rect
            0,      // Y begin of destination rect
            width,  // Width of the source and destination rect
            height, // Height of the source and destination rect
            tmpDc,  // Sourse DC
            0,      // X begin of source rect
            0,      // Y begin of source rect
            SRCCOPY // Source copy mode
        );
    }
};

// Window processing function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

FRAME frame(400, 300);

int main()
{
    // Register the window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = L"MyWindow";
    RegisterClassW(&wc);

    // Create the window
    HWND hwnd = CreateWindowExW(0, L"MyWindow", L"My Window", WS_OVERLAPPEDWINDOW, 0, 0, frame.width, frame.height, NULL, NULL, NULL, NULL);
    
    // Create document context
    HDC dc = GetDC(hwnd);
    HDC tmpDc = CreateCompatibleDC(dc);
    HBITMAP bm = NULL;

    // Show the window
    ShowWindow(hwnd, SW_SHOWNORMAL);
    //ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL); // SW_HIDE or SW_SHOWNORMAL - Hide or Show console

    // Window message
    MSG msg = {};

    // Frame draw
    frame.clear({ 0, 255, 0 });
    frame.set_pixel({ 50, 50 }, { 0, 0, 0 });
    frame.print(dc, tmpDc, bm);

    // Main loop
    while (GetKeyState(VK_ESCAPE) >= 0)
    {
        // Processing window messages
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            DispatchMessageW(&msg);
        }
    }

    DeleteDC(tmpDc);
    DeleteObject(bm);
    ReleaseDC(hwnd, dc);

    return 0;
}