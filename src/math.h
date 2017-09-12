#ifndef MATH_H
#define MATH_H

typedef struct
{
    f32 x;
    f32 y;
} v2f;

typedef struct
{
    s32 x;
    s32 y;
} v2i;

inline s32 abs_i(s32 value)
{
    s32 result = value;

    result *= (value < 0) ? -1 : 1;

    return result;
}

inline f32 abs_f(f32 value)
{
    f32 result = value;

    result *= (value < 0) ? -1.0f : 1.0f;

    return result;
}

inline void swap_f(f32 *a, f32 *b)
{
    f32 temp = *a;
    *a = *b;
    *b = temp;
}

inline void swap_i(s32 *a, s32 *b)
{
    s32 temp = *a;
    *a = *b;
    *b = temp;
}

inline void swap_v2f(v2f *a, v2f *b)
{
    v2f temp = *a;
    *a = *b;
    *b = temp;
}

inline void swap_v2i(v2i *a, v2i *b)
{
    v2i temp = *a;
    *a = *b;
    *b = temp;
}

inline s32 round(f32 value)
{
    s32 result;
    
    result = (s32)(value + (value < 0 ? -0.5f : 0.5f));

    return result;
}

inline v2i sum_v2i(v2i a, v2i b)
{
    v2i result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}

inline v2i sub_v2i(v2i a, v2i b)
{
    v2i result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}

#endif