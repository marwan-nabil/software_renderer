#pragma once
#include "platform_interface.h"

// 2d vector type
union v2 {
    struct {
        f32 X, Y;
    };
    f64 E[2];
};

// utility constructor
inline v2
V2(f32 X, f32 Y)
{
    v2 Result;
    Result.X = X;
    Result.Y = Y;
    return Result;
}

//
// vector addition operations
//
inline v2
operator+(v2 A, v2 B)
{
    v2 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    return Result;
}

inline v2 &
operator+=(v2 &A, v2 B)
{
    A.X = A.X + B.X;
    A.Y = A.Y + B.Y;
    return A;
}

inline v2
operator-(v2 A, v2 B)
{
    v2 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    return Result;
}

inline v2 &
operator-=(v2 &A, v2 B)
{
    A.X = A.X - B.X;
    A.Y = A.Y - B.Y;
    return A;
}

// unary minus
inline v2
operator-(v2 A)
{
    v2 Result;
    Result.X = -A.X;
    Result.Y = -A.Y;
    return Result;
}

// scalar multiplication (left side)
inline v2
operator*(f32 A, v2 B)
{
    v2 Result;
    Result.X = A * B.X;
    Result.Y = A * B.Y;
    return Result;
}

// scalar multiplication (right side)
inline v2
operator*(v2 A, f32 B)
{
    v2 Result;
    Result.X = B * A.X;
    Result.Y = B * A.Y;
    return Result;
}

inline v2 &
operator*=(v2 &A, f32 B)
{
    A.X = B * A.X;
    A.Y = B * A.Y;
    return A;
}
