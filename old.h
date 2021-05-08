#pragma once
#include "sqrt.h"

bool debug = false;
 
uint64_t fast_sqrt_kar(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint32_t bits) {
    if (debug) {
        std::cout << std::setfill('0') << std::setw(8) << std::hex << a << " ";
        std::cout << std::setfill('0') << std::setw(8) << std::hex << b << " ";
        std::cout << std::setfill('0') << std::setw(8) << std::hex << c << " ";
        std::cout << std::setfill('0') << std::setw(8) << std::hex << d << std::endl;
    }
    assert(a < (1ull << 32));
    assert(b < (1ull << 32));
    assert(c < (1ull << 32));
    assert(d < (1ull << 32));
    assert(a >= (1ull << 30));
    uint64_t high = (a << bits) + b;
    uint64_t hsqrt = std::min(fast_sqrt_int(high), (1ull << bits) - 1);
    uint64_t hrmnd = high - hsqrt * hsqrt;
    //assert(hsqrt * hsqrt <= high && (hsqrt + 1) * (hsqrt + 1) > high);
 
    uint64_t mid = (hrmnd << bits) + c;
    uint64_t hsqrt2 = (2 * hsqrt);
    uint64_t mdiv = mid / hsqrt2;
    uint64_t mrmnd = mid - mdiv * hsqrt2;
 
    uint64_t appr = (hsqrt << bits) + mdiv;
 
    int64_t rest = (mrmnd << bits) + d;
    int64_t mdiv2 = mdiv * mdiv;
    int64_t t = rest - mdiv2;
    if (t < 0) {
        appr--;
        rest = rest + hsqrt2 - 1;
    }
    return appr;
}

uint32_t first_bit(uint32_t value) {
    uint32_t ind;
    for (ind = 31;; ind--)
        if (value & (1u << ind))
            break;
    return ind;
}
 
template<>
uint64_t fast_sqrt_kar<3>(const bmp::limb_type* arr) {
    if (arr[2] == 0)
        return fast_sqrt_kar<2>(arr);
 
    uint32_t bits = first_bit(arr[2]) + 65;
    uint32_t bucket = (bits + 3) / 4; // (a + b - 1) / b
    std::bitset<3 * 32> number;
    for (int32_t i = 2; i >= 0; i--) {
        number <<= 32;
        number |= arr[i];
    }
    
    uint64_t a, b, c, d;
    uint32_t mask = (1 << bucket) - 1;
    
    std::bitset<3 * 32> tmp = number;
    tmp &= mask;
    d = tmp.to_ullong();
    number >>= bucket;
 
    tmp = number;
    tmp &= mask;
    c = tmp.to_ullong();
    number >>= bucket;
 
    tmp = number;
    tmp &= mask;
    b = tmp.to_ullong();
    number >>= bucket;
 
    tmp = number;
    tmp &= mask;
    a = tmp.to_ullong();
    return fast_sqrt_kar(a, b, c, d, bucket);
}
 
template<>
uint64_t fast_sqrt_kar<4>(const bmp::limb_type* arr) {
    if (debug) {
        std::cout << std::setfill('0') << std::setw(8) << std::hex << arr[3] << " ";
        std::cout << std::setfill('0') << std::setw(8) << std::hex << arr[2] << " ";
        std::cout << std::setfill('0') << std::setw(8) << std::hex << arr[1] << " ";
        std::cout << std::setfill('0') << std::setw(8) << std::hex << arr[0] << std::endl;
    }
    if (arr[3] == 0)
        return fast_sqrt_kar<3>(arr);
 
    uint32_t bits = first_bit(arr[3]) + 97;
    uint32_t bucket = (bits + 3) / 4; // (a + b - 1) / b
    if (bucket == 32) {
        return fast_sqrt_kar(arr[3], arr[2], arr[1], arr[0], bucket);
    }
 
    std::bitset<4 * 32> number;
    for (int32_t i = 3; i >= 0; i--) {
        number <<= 32;
        number |= arr[i];
    }
 
    uint64_t a, b, c, d;
    uint32_t mask = (1 << bucket) - 1;
 
    std::bitset<4 * 32> tmp = number;
    tmp &= mask;
    d = tmp.to_ullong();
    number >>= bucket;
 
    tmp = number;
    tmp &= mask;
    c = tmp.to_ullong();
    number >>= bucket;
 
    tmp = number;
    tmp &= mask;
    b = tmp.to_ullong();
    number >>= bucket;
 
    tmp = number;
    tmp &= mask;
    a = tmp.to_ullong();
    return fast_sqrt_kar(a, b, c, d, bucket);
}
 
 