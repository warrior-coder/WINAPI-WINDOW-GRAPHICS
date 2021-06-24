#include <windows.h>
#include <stdio.h>

#define ABS(a) ( (a) < 0 ? -(a) : (a) )
#define MIN(a, b) ( (a) < (b) ? (a) : (b) )
#define MAX(a, b) ( (a) > (b) ? (a) : (b) )
#define PI 3.14159265358979323846

// Define using types
typedef unsigned char BYTE;

typedef struct
{
    BYTE R;
    BYTE G;
    BYTE B;
}RGB;

typedef struct
{
    BYTE B;
    BYTE G;
    BYTE R;
    BYTE A;
}RGB4;

typedef struct
{
    short x;
    short y;
}DOT_2D;

typedef struct
{
    short x;
    short y;
    short z;
}DOT_3D;

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

    void set_triangle(int x1, int y1, int x2, int y2, int x3, int y3)
    {
        int left = MIN(MIN(x1, x2), x3);   // Rect left border
        int right = MAX(MAX(x1, x2), x3);  // Rect right border
        int top = MIN(MIN(y1, y2), y3);    // Rect top border
        int bottom = MAX(MAX(y1, y2), y3); // Rect bottom border

        int side12, side23, side31;

        for (int x = left; x <= right; x++)
        {
            for (int y = top; y <= bottom; y++)
            {
                side12 = (x - x1) * (y2 - y1) - (x2 - x1) * (y - y1);
                side23 = (x - x2) * (y3 - y2) - (x3 - x2) * (y - y2);
                side31 = (x - x3) * (y1 - y3) - (x1 - x3) * (y - y3);

                if (side12 <= 0 && side23 <= 0 && side31 <= 0 || side12 > 0 && side23 > 0 && side31 > 0) set_pixel((short)x, (short)y);
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
            g_hdc,   // Destination DC
            0,       // X begin of destination rect
            0,       // Y begin of destination rect
            width,   // Width of the source and destination rect
            height,  // Height of the source and destination rect
            g_tmpDc, // Sourse DC
            0,       // X begin of source rect
            0,       // Y begin of source rect
            SRCCOPY  // Source copy mode
        );

        // Clear memory
        DeleteObject(g_hbm);
        DeleteDC(g_tmpDc);
        ReleaseDC(g_hwnd, g_hdc);
    }

    void save(const char* fname)
    {
        const short f_header_size = 14;
        BYTE f_header[f_header_size];

        const short f_info_size = 40;
        BYTE f_info[f_info_size];

        int p_size = (4 - (width * 3) % 4) % 4;

        int fsize = f_header_size + f_info_size + (width * height * 4) + (p_size * height);

        // File signature
        f_header[0] = 'B';
        f_header[1] = 'M';

        // File size
        f_header[2] = fsize;
        f_header[3] = fsize >> 8;
        f_header[4] = fsize >> 16;
        f_header[5] = fsize >> 24;

        // Reserved
        f_header[6] = 0;
        f_header[7] = 0;
        f_header[8] = 0;
        f_header[9] = 0;

        // Pixel data offset
        f_header[10] = f_header_size + f_info_size;
        f_header[11] = 0;
        f_header[12] = 0;
        f_header[13] = 0;

        // Header size
        f_info[0] = f_info_size;
        f_info[1] = 0;
        f_info[2] = 0;
        f_info[3] = 0;

        // Image width
        f_info[4] = width;
        f_info[5] = width >> 8;
        f_info[6] = width >> 16;
        f_info[7] = width >> 24;

        // Image height
        f_info[8] = height;
        f_info[9] = height >> 8;
        f_info[10] = height >> 16;
        f_info[11] = height >> 24;

        // Planes
        f_info[12] = 1;
        f_info[13] = 0;

        // Bits per pixel (RGB - 24 bits)
        f_info[14] = 24;
        f_info[15] = 0;

        // Compression (No compression)
        f_info[16] = 0;
        f_info[17] = 0;
        f_info[18] = 0;
        f_info[19] = 0;

        // Image size (No compression)
        f_info[20] = 0;
        f_info[21] = 0;
        f_info[22] = 0;
        f_info[23] = 0;

        // X pixels per meter
        f_info[24] = 0;
        f_info[25] = 0;
        f_info[26] = 0;
        f_info[27] = 0;

        // Y pixels per meter
        f_info[28] = 0;
        f_info[29] = 0;
        f_info[30] = 0;
        f_info[31] = 0;

        // Total colors (Not used)
        f_info[32] = 0;
        f_info[33] = 0;
        f_info[34] = 0;
        f_info[35] = 0;

        // Important colors (0 - all)
        f_info[36] = 0;
        f_info[37] = 0;
        f_info[38] = 0;
        f_info[39] = 0;

        // Write file
        FILE* fp;

        fopen_s(&fp, fname, "wb");
        if (fp)
        {
            // Write header
            fwrite(f_header, sizeof(BYTE), f_header_size, fp);
            
            // Write info
            fwrite(f_info, sizeof(BYTE), f_info_size, fp);

            // Write buffer
            BYTE padding = 0;

            for (int y = height-1; y > -1; y--)
            {
                for (int x = 0; x < width; x++)
                {
                    fwrite(&buffer[y*width+x].B, sizeof(BYTE), 1, fp);
                    fwrite(&buffer[y*width+x].G, sizeof(BYTE), 1, fp);
                    fwrite(&buffer[y*width+x].R, sizeof(BYTE), 1, fp);
                }
                fwrite(&padding, sizeof(BYTE), p_size, fp);
            }

            fclose(fp);
        }
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
    frame.clear();

    frame.pen_color = { 255,0,0 };
    frame.set_triangle(200, 50, 300, 250, 100, 300);

    frame.pen_color = { 0,255,0 };
    frame.set_triangle(50, 50, 200, 100, 150, 250);

    frame.pen_color = { 0,0, 255 };
    frame.set_triangle(350, 350, 350, 250, 250, 350);

    frame.print();

    frame.save("out.bmp");
    
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