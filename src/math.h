#ifndef MATH_H
#define MATH_H

typedef struct
{
    float x;
    float y;
} en_v2f;

inline float en_abs_f(float value)
{
    float result = value;

    result *= (value < 0) ? -1.0f : 1.0f;

    return result;
}

inline void en_swap_f(float *a, float *b)
{
    float temp = *a;
    *a = *b;
    *b = temp;
}

inline void en_swap_v2f(en_v2f *a, en_v2f *b)
{
    en_v2f temp = *a;
    *a = *b;
    *b = temp;
}

#endif