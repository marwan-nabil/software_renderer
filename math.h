#pragma once
#include <math.h>
#include "platform_interface.h"

inline i32
RoundF32ToI32(f32 Value)
{
    i32 Result = (i32) roundf(Value);
    return Result;
}

inline u32
RoundF32ToU32(f32 Value)
{
    u32 Result = (u32) roundf(Value);
    return Result;
}

inline i32
FloorF32ToI32(f32 Value)
{
    i32 Result = (i32) floorf(Value);
    return Result;
}

// safely truncates an unsigned 64bit value into a 32bit one
// safely here means will not cause unexpected info loss in case
// of big values, but will segfault instead
inline u32
SafeTruncateU64ToU32(u64 Value)
{
    Assert(Value <= 0xFFFFFFFF);
    u32 Result = (u32) Value;
    return(Result);
}

inline f32
Sin(f32 Angle)
{
    return sinf(Angle);
}

inline f32
Cos(f32 Angle)
{
    return cosf(Angle);
}

inline f64
ATan2(f64 Y, f64 X)
{
    return atan2(Y, X);
}

typedef struct
{
    bool Found;
    u32 Index;
} bitscan_result;

inline bitscan_result
FindLeastSignificantSetBit(u32 Value)
{
    bitscan_result Result;
    Result.Found = _BitScanForward((unsigned long *) &Result.Index, Value);
    return Result;
}

inline f32
Square(f32 Val)
{
    f32 Result = Val * Val;
    return Result;
}
