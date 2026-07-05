#if !defined(MX_MATH_H)
#define MX_MATH_H

#include <math.h>

/* Quantization */
#define DECLARE_QUANT(TYPE)                                                                                            \
    force_inline internal TYPE quantize_f32_to_##TYPE(f32 val, f32 min, f32 max);                                      \
    force_inline internal f32 dequantize_##TYPE##_to_f32(TYPE val, f32 min, f32 max);

#define DEFINE_QUANT(TYPE)                                                                                             \
    force_inline internal TYPE quantize_f32_to_##TYPE(f32 val, f32 min, f32 max) {                                     \
        if (max <= min)                                                                                                \
            return (TYPE)0;                                                                                            \
        f32 scaled = ((val - min) / (max - min)) * ((f32)TYPE##_max - (f32)TYPE##_min) + (f32)TYPE##_min;              \
        return (TYPE)clamp(roundf(scaled), (f32)TYPE##_min, (f32)TYPE##_max);                                          \
    }                                                                                                                  \
    force_inline internal f32 dequantize_##TYPE##_to_f32(TYPE val, f32 min, f32 max) {                                 \
        if (max <= min)                                                                                                \
            return min;                                                                                                \
        return ((f32)val - (f32)TYPE##_min) / ((f32)TYPE##_max - (f32)TYPE##_min) * (max - min) + min;                 \
    }

DECLARE_QUANT(I8)
DECLARE_QUANT(I16)
DECLARE_QUANT(I32)
DECLARE_QUANT(U8)
DECLARE_QUANT(U16)
DECLARE_QUANT(U32)

#endif /* MX_MATH_H */