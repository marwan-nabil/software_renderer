#include "platform_interface.h"
#include "math.h"
#include "vector_math.h"
#include "shapes.h"


struct color {
    u8 Red, Green, Blue, Alpha;
};

rectangle ClipRectToBuffer(rectangle Rect, offscreen_buffer *Buffer)
{
    rectangle Result;
    u32 MinX = RoundF32ToU32(Rect.TopLeft.X);
    u32 MinY = RoundF32ToU32(Rect.TopLeft.Y);
    u32 MaxX = RoundF32ToU32(Rect.DownRight.X);
    u32 MaxY = RoundF32ToU32(Rect.DownRight.Y);

    if(MinX < 0)
    {
        MinX = 0;
    }

    if(MaxX > Buffer->Width)
    {
        MaxX = Buffer->Width;
    }

    if(MinY < 0)
    {
        MinY = 0;
    }

    if(MaxY > Buffer->Height)
    {
        MaxY = Buffer->Height;
    }

    Result.TopLeft = {(f32) MinX, (f32) MinY};
    Result.DownRight = {(f32) MaxX, (f32) MaxY};
    return Result;
}

u32 GetPixelValueFromColor(color Color)
{
    u32 Result = (
        ((u32) (Color.Alpha) << 24) |
        ((u32) (Color.Red) << 16) |
        ((u32) (Color.Green) << 8) |
        ((u32) (Color.Blue) << 0));
    return Result;
}

void PaintRainbow(offscreen_buffer Buffer, rectangle Rect, renderer_state *State)
{
    rectangle ClippedRect = ClipRectToBuffer(Rect, &Buffer);
    u32 MinX = RoundF32ToU32(ClippedRect.TopLeft.X);
    u32 MinY = RoundF32ToU32(ClippedRect.TopLeft.Y);
    u32 MaxX = RoundF32ToU32(ClippedRect.DownRight.X);
    u32 MaxY = RoundF32ToU32(ClippedRect.DownRight.Y);

    u8 *EndOfBuffer = (u8 *) Buffer.Memory +
        (Buffer.Pitch * Buffer.Height);

    u8 *Row = ((u8 *) Buffer.Memory +
               MinY * Buffer.Pitch +
               MinX * Buffer.BytesPerPixel);

    for(u32 Y = MinY;
        Y < MaxY;
        Y++)
    {
        u32 *Pixel = (u32 *) Row;

        for(u32 X = MinX;
            X < MaxX;
            X++)
        {
            *Pixel++ = GetPixelValueFromColor({(State->X + State->Y) % 255, (X + State->X) % 255, (Y + State->Y) % 255, 255});
        }

        Row += Buffer.Pitch;
    }
}

void RenderRectangle(offscreen_buffer Buffer, color Color, f64 X, f64 Y, f64 Width, f64 Height)
{
    u32 MinX = (u32) X;
    u32 MinY = (u32) Y;
    u32 MaxX = (u32) Width + MinX;
    u32 MaxY = (u32) Height + MinY;

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

    for(u32 Y = MinY;
        Y < MaxY;
        Y++)
    {
        u32 *Pixel = (u32 *) Row;

        for(u32 X = MinX;
            X < MaxX;
            X++)
        {
            *Pixel++ = PixelColor;
        }

        Row += Buffer.Pitch;
    }
}

void InitializeRendererState(renderer_state *State)
{
    State->X = 0;
    State->Y = 0;
    State->ScrollSpeed = 30;
}

void RendererStateUpdate(input_sample *OldInput, input_sample *NewInput, f32 TimeDelta, renderer_state *State)
{
    if(NewInput->MoveUp.EndedDown)
    {
        State->Y -= State->ScrollSpeed;
    }
    if(NewInput->MoveDown.EndedDown)
    {
        State->Y += State->ScrollSpeed;
    }
    if(NewInput->MoveLeft.EndedDown)
    {
        State->X -= State->ScrollSpeed;
    }
    if(NewInput->MoveRight.EndedDown)
    {
        State->X += State->ScrollSpeed;
    }
}

void RendererRenderFrame(offscreen_buffer *Buffer, renderer_state *State)
{
    // draw a simple rainbow rectangle
    rectangle Rect;
    Rect.TopLeft = {20, 20};
    Rect.DownRight = {500, 500};
    PaintRainbow(*Buffer, Rect, State);
}