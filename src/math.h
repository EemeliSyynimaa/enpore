#ifndef MATH_H
#define MATH_H

typedef struct
{
    float x;
    float y;
} en_v2f;

typedef struct
{
    int x;
    int y;
} en_v2i;

inline int en_abs_i(int value)
{
    int result = value;

    result *= (value < 0) ? -1 : 1;

    return result;
}

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

inline void en_swap_i(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

inline void en_swap_v2f(en_v2f *a, en_v2f *b)
{
    en_v2f temp = *a;
    *a = *b;
    *b = temp;
}

inline void en_swap_v2i(en_v2i *a, en_v2i *b)
{
    en_v2i temp = *a;
    *a = *b;
    *b = temp;
}

#endif