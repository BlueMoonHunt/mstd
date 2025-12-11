#include <mstd.h>
#include <intrin.h>

#define PCG_MULT_LOW  0x4385DFE97E1F97B1ULL
#define PCG_MULT_HIGH 0x2360ED051FC65DA4ULL

inline void random_update_pcg_state(RandomNumberGenerator* rng) {
    u128 A = (u128){ .high = rng->state.high, .low = rng->state.low };
    u128 P = {0};

    u64 P_00_carry;
    u64 P_10_low, P_10_carry;
    u64 P_01_low, P_01_carry;

    u8 carry_temp = 0;

    P.low = _umul128(A.low, PCG_MULT_LOW, &P_00_carry);
    P_10_low = _umul128(A.high, PCG_MULT_LOW, &P_10_carry);
    P_01_low = _umul128(A.low, PCG_MULT_HIGH, &P_01_carry);
    carry_temp = _addcarry_u64(0, P_00_carry, P_10_low, &P.high);
    _addcarry_u64(carry_temp, P.high, P_01_low, &P.high);
    u8 carry_inc = 0;
    carry_inc = _addcarry_u64(0, P.low, rng->inc.low, &rng->state.low);
    _addcarry_u64(carry_inc, P.high, rng->inc.high, &rng->state.high);
}

u64 random_in_range(u64 low, u64 high, RandomNumberGenerator* rng) {
    u64 range = high - low;
    if (range == UINT64_MAX) {
        return random_pcg(rng);
    }
    range += 1;

    u64 threshold = (0-(range)) % range;
    u64 x, high_product, low_product;
    do {
        x = random_pcg(rng);
        low_product = _umul128(x, range, &high_product);
    } while (low_product < threshold);
    return high_product + low;
}