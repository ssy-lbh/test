#include <stdio.h>
#include <math.h>

// 系统库
#include <windows.h>

// 图形库
#include <GL/gl.h>
#include <GL/glu.h>

#include <GL/wgl.h>

const int MAP_WIDTH = 10;
const int MAP_HEIGHT = 10;

const float MAP_UNIT = 2.0f / MAP_WIDTH;

char map[MAP_HEIGHT][MAP_WIDTH + 1] =
    {"##########",
     "#..#..O#.#",
     "#.....#..#",
     "#...#....#",
     "#.#.#X.#.#",
     "#...#..#.#",
     "#.X.#..#.#",
     "#...#C.#.#",
     "#..O#..#.#",
     "##########"};

typedef struct {
    int x;
    int y;
} int2;

int2 pos = {};

// 游戏初始化
void InitGame(){
    for (int x = 0; x < MAP_WIDTH; x++){
        for (int y = 0; y < MAP_HEIGHT; y++){
            switch (map[y][x]){
            case 'C':
                pos.x = x;
                pos.y = y;
                break;
            }
        }
    }
}

// 返回1移动成功
// 返回0移动失败
int Move(int dx, int dy){
    int2 next = {
        .x = pos.x + dx,
        .y = pos.y + dy
    };

    switch (map[next.y][next.x]){
    case '#':
        return 0;
    case '.':
        map[pos.y][pos.x] = '.';
        map[next.y][next.x] = 'C';
        break;
    case 'X': {
        int2 next2 = {
            .x = next.x + dx,
            .y = next.y + dy
        };
        if (map[next2.y][next2.x] == 'O'){
            map[pos.y][pos.x] = '.';
            map[next.y][next.x] = 'C';
            map[next2.y][next2.x] = 'Y';
            break;
        }
        if (map[next2.y][next2.x] != '.')
            return 0;
        map[pos.y][pos.x] = '.';
        map[next.y][next.x] = 'C';
        map[next2.y][next2.x] = 'X';
        break;
    }
    default:
        return 0;
    }

    pos = next;

    return 1;
}

// 窗口消息处理函数
LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch (uMsg){
    case WM_CREATE:
        InitGame();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        switch (wParam){
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        case 'W':
            Move(0, -1);
            break;
        case 'A':
            Move(-1, 0);
            break;
        case 'S':
            Move(0, 1);
            break;
        case 'D':
            Move(1, 0);
            break;
        }
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

inline void Vertex2(float x, float y){
    glVertex2f(-1.0f + x * MAP_UNIT, 1.0f - y * MAP_UNIT);
}

inline void Vertex3(float x, float y, float z){
    glVertex3f(-1.0f + x * MAP_UNIT, 1.0f - y * MAP_UNIT, z);
}

void To3D(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.1, 10.0);
    gluLookAt(0.0, -0.5, 3.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Render(){
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    To3D();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    Vertex2(0, 0);
    Vertex2(MAP_WIDTH, 0);
    Vertex2(MAP_WIDTH, MAP_HEIGHT);
    Vertex2(0, MAP_HEIGHT);
    glEnd();

    glClearDepth(1.0);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (int x = 0; x < MAP_WIDTH; x++){
        for (int y = 0; y < MAP_HEIGHT; y++){
            switch (map[y][x]){
            case '#':
                glColor3f(0.5f, 0.5f, 0.0f);
                glBegin(GL_QUADS);
                Vertex2(x, y);
                Vertex2(x + 1, y);
                Vertex2(x + 1, y + 1);
                Vertex2(x, y + 1);
                glEnd();
                break;
            case 'C':
                glColor3f(1.0f, 0.0f, 0.0f);
                glBegin(GL_TRIANGLES);
                Vertex2(x + 0.5f, y + 0.2f);
                Vertex2(x + 0.2f, y + 0.8f);
                Vertex2(x + 0.8f, y + 0.8f);
                glEnd();
                break;
            case 'X':
                glColor3f(1.0f, 1.0f, 0.0f);
                glBegin(GL_QUADS);
                Vertex2(x + 0.2f, y + 0.2f);
                Vertex2(x + 0.8f, y + 0.2f);
                Vertex2(x + 0.8f, y + 0.8f);
                Vertex2(x + 0.2f, y + 0.8f);
                glEnd();
                break;
            case 'O': {
                glColor3f(0.0f, 0.0f, 1.0f);
                glBegin(GL_TRIANGLE_FAN);
                Vertex2(x + 0.5f, y + 0.5f);
                const float R = 0.4f;
                for (float angle = 0.0f; angle < 360.0f; angle += 1.0f){
                    float vsin, vcos;
                    sincosf(angle, &vsin, &vcos);
                    Vertex2(x + 0.5f + vcos * R, y + 0.5f + vsin * R);
                }
                Vertex2(x + 0.5f + R, y + 0.5f);
                glEnd();
                break;
            }
            case 'Y': {
                glColor3f(1.0f, 1.0f, 0.0f);

                glBegin(GL_TRIANGLE_FAN);
                Vertex2(x + 0.5f, y + 0.5f);
                const float R = 0.4f;
                for (float angle = 0.0f; angle < 360.0f; angle += 1.0f){
                    float vsin, vcos;
                    sincosf(angle, &vsin, &vcos);
                    Vertex2(x + 0.5f + vcos * R, y + 0.5f + vsin * R);
                }
                Vertex2(x + 0.5f + R, y + 0.5f);
                glEnd();

                glBegin(GL_TRIANGLE_FAN);
                Vertex3(x + 0.5f, y + 0.5f, 0.3f);
                for (float angle = 0.0f; angle < 360.0f; angle += 1.0f){
                    float vsin, vcos;
                    sincosf(angle, &vsin, &vcos);
                    Vertex2(x + 0.5f + vcos * R, y + 0.5f + vsin * R);
                }
                Vertex2(x + 0.5f + R, y + 0.5f);
                glEnd();

                break;
            }
            }
        }
    }
}

const char* WINDOW_CLASS = "PushBoxClass";
const char* WINDOW_TITLE = "PushBox";

ATOM SetupClass(HINSTANCE hInst){
    WNDCLASSEX wcex = {};

    wcex.cbSize = sizeof(wcex);
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInst;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszClassName = WINDOW_CLASS;
    wcex.hIcon = NULL;
    wcex.hIconSm = NULL;

    return RegisterClassEx(&wcex);
}

HWND SetupWindow(HINSTANCE hInst){
    SetupClass(hInst);

    return CreateWindow(
        WINDOW_CLASS,
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800, 800,
        NULL,
        NULL,
        hInst,
        NULL
    );
}

void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC){
    PIXELFORMATDESCRIPTOR pfd = {};
    *hDC = GetDC(hWnd);

    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int iFormat = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, iFormat, &pfd);

    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC){
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
    MSG Msg;

    hWnd = SetupWindow(hInstance);

    EnableOpenGL(hWnd, &hDC, &hRC);

    UpdateWindow(hWnd);
    ShowWindow(hWnd, SW_SHOW);
    // 消息循环
    while (1){
        if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)){
            if (Msg.message == WM_QUIT){
                break;
            }
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        } else {
            Render();
            SwapBuffers(hDC);
        }
    }

    DisableOpenGL(hWnd, hDC, hRC);
    DestroyWindow(hWnd);

    return Msg.wParam;
}