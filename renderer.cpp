#include "platform_interface.h"

struct color {
    u8 Red, Green, Blue;
};

void RenderRectangle(offscreen_buffer Buffer, color Color, f64 X, f64 Y, f64 Width, f64 Height)
{
    i32 MinX = (i32) X;
    i32 MinY = (i32) Y;
    i32 MaxX = (i32) Width + MinX;
    i32 MaxY = (i32) Height + MinY;

    if(MinX < 0)
    {
        MinX = 0;
    }

    if(MaxX > Buffer.Width)
    {
        MaxX = Buffer.Width;
    }

    if(MinY < 0)
    {
        MinY = 0;
    }

    if(MaxY > Buffer.Height)
    {
        MaxY = Buffer.Height;
    }

    u8 *EndOfBuffer = (u8 *) Buffer.Memory +
        ((long) Buffer.Pitch * (long) Buffer.Height);

    u32 PixelColor = ((u32) (Color.Red) << 16) |
        ((u32) (Color.Green) << 8) |
        ((u32) (Color.Blue) << 0);

    u8 *Row = ((u8 *) Buffer.Memory +
               (long) MinY * (long) Buffer.Pitch +
               (long) MinX * (long) Buffer.BytesPerPixel);

    for(int Y = MinY;
        Y < MaxY;
        Y++)
    {
        u32 *Pixel = (u32 *) Row;

        for(int X = MinX;
            X < MaxX;
            X++)
        {
            *Pixel++ = PixelColor;
        }

        Row += Buffer.Pitch;
    }
}

void RendererStateUpdate(input_sample *OldInput, input_sample *NewInput, f32 TimeDelta)
{

}

void RendererRenderFrame(offscreen_buffer *Buffer)
{
    // draw a simple red rect
    RenderRectangle(*Buffer, {230, 0, 0}, 20, 20, 400, 400);
}