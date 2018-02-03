#ifndef MATH_INL
#define MATH_INL

s32 abs_i(s32 value)
{
    s32 result = value;

    result *= (value < 0) ? -1 : 1;

    return result;
}

f32 abs_f(f32 value)
{
    f32 result = value;

    result *= (value < 0) ? -1.0f : 1.0f;

    return result;
}

s32 max_i(s32 a, s32 b)
{
    s32 result;

    result = a < b ? b : a;

    return result;
}

s32 min_i(s32 a, s32 b)
{
    s32 result;

    result = a < b ? a : b;

    return result;
}

void swap_f(f32 *a, f32 *b)
{
    f32 temp = *a;
    *a = *b;
    *b = temp;
}

void swap_i(s32 *a, s32 *b)
{
    s32 temp = *a;
    *a = *b;
    *b = temp;
}

void swap_v2f(v2f *a, v2f *b)
{
    v2f temp = *a;
    *a = *b;
    *b = temp;
}

void swap_v2i(v2i *a, v2i *b)
{
    v2i temp = *a;
    *a = *b;
    *b = temp;
}

s32 round(f32 value)
{
    s32 result;
    
    result = (s32)(value + (value < 0 ? -0.5f : 0.5f));

    return result;
}

v2i sum_v2i(v2i a, v2i b)
{
    v2i result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}

v2i sub_v2i(v2i a, v2i b)
{
    v2i result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}

b32 equals_v2i(v2i a, v2i b)
{
    b32 result = (a.x == b.x && a.y == b.y);

    return result;
}

#endif