#ifndef MATH_H
#define MATH_H

#include "platform.h"

s32 abs_i(s32 value);
f32 abs_f(f32 value);

s32 max_i(s32 a, s32 b);
s32 min_i(s32 a, s32 b);

void swap_f(f32 *a, f32 *b);
void swap_i(s32 *a, s32 *b);

void swap_v2f(v2f *a, v2f *b);
void swap_v2i(v2i *a, v2i *b);

s32 round(f32 value);

v2i sum_v2i(v2i a, v2i b);
v2i sub_v2i(v2i a, v2i b);

b32 equals_v2i(v2i a, v2i b);

#include "math.inl"

#endif