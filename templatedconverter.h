/*
author: Giovanni Rasera
git: GiovanniRaseraF
license: open to use, change and redistribute
*/

#ifndef TEMPLATEDCONVERTER_H
#define TEMPLATEDCONVERTER_H
#include <unistd.h>
#include <utility>

// if no constexpr is allowed on your compiler use this part
#ifdef NO_SUPPORT_CONSTEXPR
template<uint8_t s, uint8_t e, bool active, unsigned long  acc, uint8_t counter, bool start>
struct maskbits{};

template<uint8_t s, uint8_t e, bool active, unsigned long acc, uint8_t c>
struct maskbits<s, e, active, acc, c, false>
    : public maskbits<
        s,
        e,
        (c >= s && c < e),
        (((unsigned long)(c >= s && c < e)) << c) | acc,
        c+1,
        false
    >{};

template<uint8_t s, uint8_t e, bool active, unsigned long acc>
struct maskbits<s, e, active, acc, 64, false>{
    static const unsigned long value = acc;
};

template<uint8_t s, uint8_t e>
struct mask_base{
    static const unsigned long value = maskbits<64-e, 64-s, 0, 0, 0, false>::value;
};

template<uint64_t s, uint64_t e>
uint64_t mask(){
    return mask_base<s, e>::value;
}

template<uint64_t sbyte, uint64_t ebyte>
uint64_t maskbyte(){
    return mask_base<sbyte*8, ebyte*8>::value;
}
#else
template<uint64_t s, uint64_t e, uint64_t c>
uint64_t mask(){
    if constexpr(c >= s && c < e)
        return (0x8000000000000000 >> c) | mask<s, e, c+1>();
    else if constexpr(c == 64)
        return 0;
    else
        return 0 | mask<s, e, c+1>();
}

template<uint64_t s, uint64_t e>
uint64_t mask(){
    if constexpr(s < 0 || e > 64 || s > e)
        return 0;
    else
        return mask<s, e, 0>();
}

template<uint64_t sbyte, uint64_t ebyte>
uint64_t maskbyte(){
    return mask<sbyte * 8, ebyte * 8>();
}
#endif

// Masking
#define __(type_var, nameVar, def, s, e, offset, scale) \
type_var nameVar = def; \
uint64_t nameVar##_mask = mask<s, e>(); \
uint64_t nameVar##_start = s; \
uint64_t nameVar##_end = e; \ 
uint8_t nameVar##_offset = offset; \
float nameVar##_scale = (scale == 0 ? 1 : scale); \
\

#define GET(nameVar, data) \
nameVar = (decltype(nameVar))( (data & nameVar##_mask) >> (64 - nameVar##_end)) * nameVar##_scale; \
\

// to use this add CXXFLAGS += -frtti to ~/.arduino15/packages/esp32/hardware/esp32/2.0.9/platform.txt
#define TOJSON(nameVar) \
'"' << #nameVar << '"' << ':' << std::to_string((nameVar)) << ',' << std::endl \
\

#define LASTTOJSON(nameVar) \
'"' << #nameVar << '"' << ':' << std::to_string((nameVar)) \
\

#endif // TEMPLATEDCONVERTER_H


