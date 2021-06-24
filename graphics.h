#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

// Include using libs
#include <windows.h>
#include <stdio.h>

// Define macroses
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

class FRAME
{
private:
    // Frame buffer
    RGB4* buffer = nullptr;

    // Handle to the window
    HWND hwnd = nullptr;

    // Handle to the device context
    HDC hdc = nullptr;
    HDC tmpDc = nullptr;

    // Handle to the bitmap
    HBITMAP hbm = nullptr;

public:
    // Size of the frame
    short width;
    short height;

    // Pen color
    RGB pen_color = {};

    FRAME(short frameWidth, short frameHeight, HWND frameHwnd);
	~FRAME();

    void clear(RGB color = { 255, 255, 255 });
    void set_rect(short x1, short y1, short x2, short y2);
    void set_circle(short x0, short y0, short R);
    void set_pixel(short x, short y);
    void set_line(short x1, short y1, short x2, short y2);
    void set_triangle(int x1, int y1, int x2, int y2, int x3, int y3);
    void print();
    void save(const char* fname);
};

#endif
