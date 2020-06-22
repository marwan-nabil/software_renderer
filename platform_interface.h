#pragma once
#include <Windows.h>
#include <stdint.h>

#define Assert(cond) if(!(cond)){*((int *) 0) = 0;}

typedef float f32;
typedef double f64;
typedef int32_t i32;
typedef uint32_t u32;
typedef uint8_t u8;
typedef int64_t i64;
typedef uint64_t u64;

//
// platform services
//

struct button_state {
    u32 HalfTransitionCount;
    bool EndedDown;
};

struct input_sample {
    button_state MoveUp;
    button_state MoveDown;
    button_state MoveLeft;
    button_state MoveRight;
};

struct offscreen_buffer {
    BITMAPINFO Info;
    void *Memory;
    u32 Width;  // in pixels
    u32 Height; // in pixels
    u32 BytesPerPixel;
    u32 Pitch;  // bytes per row
};

extern offscreen_buffer GlobalBackBuffer;

//
// renderer services
//

struct renderer_state {
    u32 X, Y;
    u32 ScrollSpeed;
};

void RendererStateUpdate(input_sample *OldInput, input_sample *NewInput, f32 TimeDelta, renderer_state *State);
void RendererRenderFrame(offscreen_buffer *Buffer, renderer_state *State);
void InitializeRendererState(renderer_state *State);