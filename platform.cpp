#include <Windows.h>
#include <stdint.h>
#include "game.h"

// -----
// types
// -----


// -------
// globals
// -------
struct
{
    int WindowWidth;
    int WindowHeight;
    HWND Window;
} GlobalState;

struct
{
    struct {
	    BITMAPINFO Info;
	    void *Memory;
	    int Width;  // in pixels
	    int Height; // in pixels
	    int BytesPerPixel;
	    int Pitch;  // bytes per row
	} BackBuffer;
} GlobalResources;


// ---------
// functions
// ---------
static void
UpdateWindowFromBuffer()
{
    offscreen_buffer *Buffer = &GlobalResources.BackBuffer;
    HDC DevContext = GetDC(GlobalState.Window);
    int OffsetX = 10;
    int OffsetY = 10;

    PatBlt(GlobalState.DevContext, 0, 0, GlobalState.WindowWidth, OffsetY, BLACKNESS);
    PatBlt(GlobalState.DevContext, 0, 0, OffsetX, GlobalState.WindowHeight, BLACKNESS);
    PatBlt(GlobalState.DevContext, OffsetX + Buffer->Width, 0, GlobalState.WindowWidth, GlobalState.WindowHeight, BLACKNESS);
    PatBlt(GlobalState.DevContext, 0, OffsetY + Buffer->Height, OffsetX + Buffer->Width, GlobalState.WindowHeight, BLACKNESS);

    StretchDIBits(GlobalState.DevContext, // destination device
                  OffsetX, OffsetY, Buffer->Width, Buffer->Height, // destination dimensions (no stretching)
                  0, 0, Buffer->Width, Buffer->Height, // source buffer stuff
                  Buffer->Memory,
                  &Buffer->Info, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(GlobalState.Window, DevContext);
}


static void
ResizeBackBuffer()
{
    offscreen_buffer *Buffer = &GlobalResources.BackBuffer;
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    // resize global bitmap dimensions
    Buffer->Width = GlobalState.WindowWidth;
    Buffer->Height = GlobalState.WindowHeight;

    // filling out the BITMAPINFO struct
    // the header is the most important
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;	// top-down scheme
    Buffer->Info.bmiHeader.biPlanes = 1;		// no separate RGB channels
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB; // no compression
    Buffer->Info.bmiHeader.biSizeImage = 0;
    Buffer->Info.bmiHeader.biXPelsPerMeter = 0;
    Buffer->Info.bmiHeader.biYPelsPerMeter = 0;
    Buffer->Info.bmiHeader.biClrUsed = 0;
    Buffer->Info.bmiHeader.biClrImportant = 0;

    Buffer->BytesPerPixel = 4;	// RGB and Alpha
    // allocate bitmap memory
    Buffer->Memory = VirtualAlloc(0, Buffer->BytesPerPixel * Buffer->Width * Buffer->Height,
                                  MEM_RESERVE | MEM_COMMIT,
                                  PAGE_READWRITE);
    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;
}


// window callback procedure, handles windows messages
LRESULT CALLBACK
MainWindowProcedure(HWND   Window,
                    UINT   Message,
                    WPARAM wParam,
                    LPARAM lParam)
{
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_ACTIVATEAPP:
        {
            if(wParam)
            {
                SetLayeredWindowAttributes(Window, RGB(0, 0, 0), 255, LWA_ALPHA);
            }
            else
            {
                SetLayeredWindowAttributes(Window, RGB(0, 0, 0), 64, LWA_ALPHA);
            }
        } break;
        
        case WM_PAINT:
        {

        } break;

        case WM_CLOSE:
        {
            PostQuitMessage(0);
        } break;

        default:
        {
            Result = DefWindowProcA(Window, Message, wParam, lParam);
        } break;
    }
    return Result;
}


// entry point
int CALLBACK
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR     lpCmdLine,
        int       nCmdShow)
{
    // ---------------
    // create a window
    // ---------------
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = MainWindowProcedure;
    WindowClass.hInstance = hInstance;
    WindowClass.lpszClassName = "MainWindowClass";

    HRESULT LastResult = RegisterClassA(&WindowClass);
    if(FAILED(LastResult))
    {
        OutputDebugStringA("RegisterClassA() FAILED, Exiting.\n");
        return 0;
    }

    HWND Window = CreateWindowExA(0, "MainWindowClass", "Backbuffer Experiments",
                                  WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST,
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  0, 0, hInstance, 0);
    if(!Window)
    {
        OutputDebugStringA("CreateWindowExA() FAILED, Exiting.\n");
        return 0;
    }
    SetWindowPos(Window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    //-----------------------
    // create a bitmap buffer
    //-----------------------
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    GlobalState.WindowHeight = ClientRect.bottom - ClientRect.top;
    GlobalState.WindowWidth = ClientRect.right - ClientRect.left;
    GlobalState.Window = Window;
    ResizeBackBuffer();


    // --------------
    // main game loop
    // --------------
    game *Game = GameInit();
    bool Running = true;
    bool Pause = false;
    while(Running)
    {
        // -------------------
        // handle all messages
        // -------------------
        MSG Message;
        while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            switch(Message.message)
            {
                case WM_QUIT:
                {
                    return (int) Message.wParam;
                } break;
                
                default:
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                } break;
            }          
        }

        if(!Pause)
        {
            // ------------
            // update state
            // ------------
            Game->UpdateGameState();
        }


        // ------------
        // render frame
        // ------------
        UpdateWindowFromBuffer();
    }

    return(0);
}
