#include <Windows.h>
#include "platform_interface.h"


static bool GlobalRunning;
static i32 MonitorRefreshHz = 30;
static f32 GameUpdateHz = ((f32) MonitorRefreshHz);
offscreen_buffer GlobalBackBuffer;
static LARGE_INTEGER PerfCounterFrequency;
static LARGE_INTEGER LastCounter;


void
ResizeBackBuffer(offscreen_buffer *Buffer,
                 i32 NewWidth, i32 NewHeight)
{
    // deallocate previous bitmap memory on size change
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    // resize global bitmap dimensions
    Buffer->Width = NewWidth;
    Buffer->Height = NewHeight;

    Buffer->BytesPerPixel = 4;	// RGBA
    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;

    // filling out the BITMAPINFO struct, the header is the most important
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -(i32) Buffer->Height; // top-down scheme
    Buffer->Info.bmiHeader.biPlanes = 1; // 1 color plane, no separate RGB channels
    Buffer->Info.bmiHeader.biBitCount = (WORD) (Buffer->BytesPerPixel * 8); // bits per pixel
    Buffer->Info.bmiHeader.biCompression = BI_RGB; // no compression
    Buffer->Info.bmiHeader.biSizeImage = 0;
    Buffer->Info.bmiHeader.biXPelsPerMeter = 0;
    Buffer->Info.bmiHeader.biYPelsPerMeter = 0;
    Buffer->Info.bmiHeader.biClrUsed = 0;
    Buffer->Info.bmiHeader.biClrImportant = 0;

    // allocate bitmap memory
    Buffer->Memory = VirtualAlloc(0, Buffer->BytesPerPixel * Buffer->Width * Buffer->Height,
                                  MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void
UpdateWindowFromBuffer(HDC DevContext, i32 WindowWidth, i32 WindowHeight,
                       offscreen_buffer *Buffer)
{
    if((WindowWidth >= Buffer->Width * 1.2) &&
       (WindowHeight >= Buffer->Height * 1.2)) // window bigger than buffer dimensions
    {
        StretchDIBits(DevContext, // destination device
                      0, 0, WindowWidth, WindowHeight, // destination dimensions (stretch to window dimensions)
                      0, 0, Buffer->Width, Buffer->Height, // source buffer stuff
                      Buffer->Memory, // source buffer memory
                      &Buffer->Info,
                      DIB_RGB_COLORS, // usage
                      SRCCOPY); // raster operation
    }
    else // window same size or smaller than backbuffer
    {
        i32 OffsetX = 10;
        i32 OffsetY = 10;

        // paint buffer offsets inside the window with black (4 regions)
        PatBlt(DevContext, 0, 0, WindowWidth, OffsetY, BLACKNESS);
        PatBlt(DevContext, 0, 0, OffsetX, WindowHeight, BLACKNESS);
        PatBlt(DevContext, OffsetX + Buffer->Width, 0, WindowWidth, WindowHeight, BLACKNESS);
        PatBlt(DevContext, 0, OffsetY + Buffer->Height, OffsetX + Buffer->Width, WindowHeight, BLACKNESS);

        StretchDIBits(
            DevContext, // destination device
            OffsetX, OffsetY, Buffer->Width, Buffer->Height, // destination dimensions (no stretching)
            0, 0, Buffer->Width, Buffer->Height, // source buffer stuff
            Buffer->Memory, // source buffer memory
            &Buffer->Info,
            DIB_RGB_COLORS,	// usage
            SRCCOPY); // raster operation
    }
}

void
ProcessKeyboardButton(button_state *NewState, bool IsDown)
{
    if(NewState->EndedDown != IsDown)
    {
        NewState->EndedDown = IsDown;
        ++NewState->HalfTransitionCount;
    }
}

void
HandleKeyboardMessage(MSG Message, input_sample *Input)
{
    u32 VKCode = (u32) Message.wParam;
    bool WasDown = ((Message.lParam & (1 << 30)) != 0);
    bool IsDown = ((Message.lParam & (1 << 31)) == 0);
    // ignore repeating messages of the same key if there's no transitions in between them
    if(WasDown != IsDown)
    {
        if(VKCode == VK_UP)
        {
            ProcessKeyboardButton(&Input->MoveUp, IsDown);
        }
        else if(VKCode == VK_DOWN)
        {
            ProcessKeyboardButton(&Input->MoveDown, IsDown);
        }
        else if(VKCode == VK_LEFT)
        {
            ProcessKeyboardButton(&Input->MoveLeft, IsDown);
        }
        else if(VKCode == VK_RIGHT)
        {
            ProcessKeyboardButton(&Input->MoveRight, IsDown);
        }
    }
}

void
HandlePaintMessage(HWND Window)
{
    PAINTSTRUCT Paint;
    HDC DevContext = BeginPaint(Window, &Paint);
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    UpdateWindowFromBuffer(DevContext,
                           ClientRect.bottom - ClientRect.top,
                           ClientRect.right - ClientRect.left,
                           &GlobalBackBuffer);
    EndPaint(Window, &Paint);
}

inline f32
GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    f32 Result = ((f32) (End.QuadPart - Start.QuadPart) / (f32) PerfCounterFrequency.QuadPart);
    return(Result);
}

inline LARGE_INTEGER
GetWallClock()
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return(Result);
}

void SyncFrame(f32 TargetFramePeriod)
{
    f32 TimeSpentWorking = GetSecondsElapsed(LastCounter, GetWallClock());
    // synchronizing with the target frame rate
    f32 SecondsElapsedForFrame = TimeSpentWorking;
    if(SecondsElapsedForFrame < TargetFramePeriod)
    {
        DWORD msSleep = (DWORD) (1000.0f * (TargetFramePeriod - SecondsElapsedForFrame));
        if(msSleep > 0)
        {
            Sleep(msSleep);
        }

        SecondsElapsedForFrame = GetSecondsElapsed(LastCounter, GetWallClock());

        if(SecondsElapsedForFrame < TargetFramePeriod)
        {
            // TODO: log missing a frame here

        }

        // spinlock for the rest of the interval
        while(SecondsElapsedForFrame < TargetFramePeriod)
        {
            SecondsElapsedForFrame = GetSecondsElapsed(LastCounter, GetWallClock());
        }
    }
    else
    {
        // we've missed a frame
    }

    // reset stats each frame
    LastCounter = GetWallClock();
}

LRESULT CALLBACK
WindowProc(HWND   Window,
           UINT   Message,
           WPARAM wParam,
           LPARAM lParam)
{
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_SIZE:
        {
        } break;

        case WM_DESTROY:
        {
            GlobalRunning = false;
        } break;

        case WM_CLOSE:
        {
            GlobalRunning = false;
        } break;

        case WM_PAINT:
        {
            HandlePaintMessage(Window);
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            Assert(!"keyboard input came in through a non-dispatch message")
        } break;

        default:
        {
            Result = DefWindowProcA(Window, Message, wParam, lParam);
        } break;
    }
    return Result;
}

void
MessagePump(input_sample *Input)
{
    MSG Message;
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch(Message.message)
        {
            case WM_QUIT:
            {
                GlobalRunning = false;
            } break;
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYUP:
            case WM_KEYDOWN:
            {
                HandleKeyboardMessage(Message, Input);
            } break;
            default:
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            } break;
        }
    }
}


i32 CALLBACK
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR     lpCmdLine,
        i32       nCmdShow)
{
    // initialize stuff
    GlobalRunning = true;
    ResizeBackBuffer(&GlobalBackBuffer, 960, 540);
    f32 TargetFramePeriod = 1.0f / GameUpdateHz;
    QueryPerformanceFrequency(&PerfCounterFrequency);
    LastCounter = GetWallClock();
    input_sample Inputs[2] = {};
    input_sample *OldInput = &Inputs[0];
    input_sample *NewInput = &Inputs[1];
    renderer_state RendererState = {};
    InitializeRendererState(&RendererState);

    // make and register a window class
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = hInstance;
    WindowClass.lpszClassName = "SoftwareRendererWindowClass";
    WindowClass.hCursor = LoadCursor(0, IDC_CROSS);
    if(!RegisterClassA(&WindowClass))
    {
        Assert(0);
    }

    // create a window
    HWND Window = CreateWindowExA(0, "SoftwareRendererWindowClass", "Software Renderer",
                                  WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  0, 0, hInstance, 0);
    if(!Window)
    {
        Assert(0);
    }

    // enter game loop
    while(GlobalRunning)
    {
        // handle all window messages
        MessagePump(NewInput);

        RendererStateUpdate(OldInput, NewInput, TargetFramePeriod, &RendererState); // update internal renderer state
        RendererRenderFrame(&GlobalBackBuffer, &RendererState); // render scene on backbuffer

        // swap input objects
        /*input_sample *Temp = OldInput;
        OldInput = NewInput;
        NewInput = Temp;*/

        SyncFrame(TargetFramePeriod); // sleep the rest of the frame to sync 

        // frame flip
        HDC DevContext = GetDC(Window);
        RECT ClientRect;
        GetClientRect(Window, &ClientRect);
        UpdateWindowFromBuffer(DevContext,
                               ClientRect.bottom - ClientRect.top,
                               ClientRect.right - ClientRect.left,
                               &GlobalBackBuffer);
    }
}

