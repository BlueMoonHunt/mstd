#if !defined(BASE_CORE_H)
#define BASE_CORE_H

#define clamp_top(val, high) (((val) < (high)) ? (val) : (high))
#define clamp_bottom(val, low) (((val) > (low)) ? (val) : (low))
#define clamp(val, low, high) (clamp_bottom(low, clamp_top(val, high)))

#define num_is_pow2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
#define num_is_pow2_or_zero(x) (((x) & ((x) - 1)) == 0)

#define align_up_pow2(x, p) (((x) + ((p) - 1)) & ~((p) - 1))
#define align_down_pow2(x, p) ((x) & ~((p) - 1))

#define BIT(x) (1ULL << (x))
#define bit(x) BIT(x)
#define KB(n) (((U64)(n)) << 10)
#define MB(n) (((U64)(n)) << 20)
#define GB(n) (((U64)(n)) << 30)
#define TB(n) (((U64)(n)) << 40)

#define mask(n) (((uint64_t)1 << (n)) - 1)

#define bit_unset(val, index) ((val) &= ~bit(index))
#define bit_set(val, index) ((val) |= bit(index))
#define bit_toggle(val, index) ((val) ^= bit(index))
#define bit_is_set(val, index) (((val) >> (index)) & 1ULL)

#define swap(T, a, b)  \
    do {               \
        T temp_ab = a; \
        a = b;         \
        b = temp_ab;   \
    } while (0)

#define NPOS (U64)(-1)
#define STRNPOS NPOS

#define for_each_index(i, n) for (i = 0; i < n; i++)

#endif /* BASE_CORE_H */