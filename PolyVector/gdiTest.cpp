#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <vector>
#include <memory>
#include <random>
#include <fstream>
#include "gdiTest.h"
#include "AllignedPolyVector.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

struct Graphic {
    virtual void draw(Graphics&, Pen*) = 0;
};
struct Line : public Graphic {
    Line(int x1, int y1, int x2, int y2) :
        x1(x1),
        y1(y1),
        x2(x2),
        y2(y2)
    {}

    void draw(Graphics& graphics, Pen* pen) {
        graphics.DrawLine(pen, x1, y1, x2, y2);
    }

    int x1, y1, x2, y2;
};
struct Elipse : public Graphic {
    Elipse(int x, int y, int w, int h) :
        x(x),
        y(y),
        w(w),
        h(h)
    {}

    void draw(Graphics& graphics, Pen* pen) {
        graphics.DrawEllipse(pen, x, y, w, h);
    }

    int x, y, w, h;
};

std::vector<std::unique_ptr<Graphic>> vec;
AllignedPolyVector<Graphic> polyVec(maxTypeSize<Elipse, Line>());

template<typename Vector> void paintPoly(Graphics& graphics, Pen& pen, Vector& polyVec, std::ofstream& file) {
    auto start = getTime();
    for (auto& g : polyVec) {
        g.draw(graphics, &pen);
    }
    auto end = getTime();
    file << end - start << '\n';
}
void paintVec(Graphics& graphics, Pen& pen, std::vector<std::unique_ptr<Graphic>>& vec, std::ofstream& file) {
    auto start = getTime();
    for (auto& g : vec) {
        g->draw(graphics, &pen);
    }
    auto end = getTime();
    file << end - start << '\n';
}

VOID OnPaint(HDC hdc) {
    static std::ofstream polyFile("poly.txt");
    static std::ofstream vecFile("vec.txt");
    static int mod = 0;
    Graphics graphics(hdc);
    Pen      pen(Color(255, 0, 0, 255));

    if (mod % 2) {
        polyFile << "1 ";
        paintPoly(graphics, pen, polyVec, polyFile);
        pen.SetColor(Color(255, 255, 0, 0));
        vecFile << "2 ";
        paintVec(graphics, pen, vec, vecFile);
    } else {
        vecFile << "1 ";
        paintVec(graphics, pen, vec, vecFile);
        pen.SetColor(Color(255, 255, 0, 0));
        polyFile << "2 ";
        paintPoly(graphics, pen, polyVec, polyFile);
    }
    mod += 1;
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow) {
    HWND                hWnd;
    MSG                 msg;
    WNDCLASS            wndClass;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    constexpr int N = 1e3;
    std::mt19937 g(1234);
    std::uniform_int_distribution<int> d(0, 500);
    for (int i = 0; i < N; ++i) {
        vec.emplace_back(std::make_unique<Line>(d(g), d(g), d(g), d(g)));
        vec.emplace_back(std::make_unique<Elipse>(d(g), d(g), d(g), d(g)));
        polyVec.emplace_back<Line>(d(g), d(g), d(g), d(g));
        polyVec.emplace_back<Elipse>(d(g), d(g), d(g), d(g));
    }

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = TEXT("GettingStarted");

    RegisterClass(&wndClass);

    hWnd = CreateWindow(
        TEXT("GettingStarted"),   // window class name
        TEXT("Getting Started"),  // window caption
        WS_OVERLAPPEDWINDOW,      // window style
        CW_USEDEFAULT,            // initial x position
        CW_USEDEFAULT,            // initial y position
        CW_USEDEFAULT,            // initial x size
        CW_USEDEFAULT,            // initial y size
        NULL,                     // parent window handle
        NULL,                     // window menu handle
        hInstance,                // program instance handle
        NULL);                    // creation parameters

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    HDC          hdc;
    PAINTSTRUCT  ps;

    switch (message) {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
} // WndProc