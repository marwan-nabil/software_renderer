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

struct input_sample {
    int a;
};

struct offscreen_buffer {
    BITMAPINFO Info;
    void *Memory;
    i32 Width;  // in pixels
    i32 Height; // in pixels
    i32 BytesPerPixel;
    i32 Pitch;  // bytes per row
};

extern offscreen_buffer GlobalBackBuffer;

//
// renderer services
//
void RendererStateUpdate(input_sample *OldInput, input_sample *NewInput, f32 TimeDelta);
void RendererRenderFrame(offscreen_buffer *Buffer);