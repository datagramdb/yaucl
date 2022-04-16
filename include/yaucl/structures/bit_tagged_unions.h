
#ifndef KNOBAB_BIT_TAGGED_UNIONS_H
#define KNOBAB_BIT_TAGGED_UNIONS_H

#ifdef  _WIN64
#define WINDOWS
#include <Windows.h>
#if REG_DWORD == REG_DWORD_LITTLE_ENDIAN
#define IS_LITTLE_ENDIAN
#else
#define IS_BIG_ENDIAN
#endif
#endif

#ifdef unix
#include <features.h>
#include <endian.h>
#if	__BYTE_ORDER == __LITTLE_ENDIAN
#define IS_LITTLE_ENDIAN
#elif __BYTE_ORDER == __BIG_ENDIAN
#define IS_BIG_ENDIAN00
#endif
#endif

#define EXPAND(x) x

#define FIELD_1(a)      a;
#define FIELD_2(a,b)    a;b
#define FIELD_3(a,...)  a;EXPAND(FIELD_2(__VA_ARGS__));
#define FIELD_4(a,...)  a;EXPAND(FIELD_3(__VA_ARGS__));
#define FIELD_5(a,...)  a;EXPAND(FIELD_4(__VA_ARGS__));
#define FIELD_6(a,...)  a;EXPAND(FIELD_5(__VA_ARGS__));
#define FIELD_7(a,...)  a;EXPAND(FIELD_6(__VA_ARGS__));
#define FIELD_8(a,...)  a;EXPAND(FIELD_7(__VA_ARGS__));
#define FIELD_9(a,...)  a;EXPAND(FIELD_8(__VA_ARGS__));
#define FIELD_10(a,...) a;EXPAND(FIELD_9(__VA_ARGS__));
#define FIELD1(N,...)   EXPAND(FIELD_ ## N(__VA_ARGS__))
#define FIELD(N, ...)   FIELD1(N, __VA_ARGS__)

#define REVERSE_1(a)        a;
#define REVERSE_2(a,b)      b;a;
#define REVERSE_3(a,...) EXPAND(REVERSE_2(__VA_ARGS__));a;
#define REVERSE_4(a,...) EXPAND(REVERSE_3(__VA_ARGS__));a;
#define REVERSE_5(a,...) EXPAND(REVERSE_4(__VA_ARGS__));a;
#define REVERSE_6(a,...) EXPAND(REVERSE_5(__VA_ARGS__));a;
#define REVERSE_7(a,...) EXPAND(REVERSE_6(__VA_ARGS__));a;
#define REVERSE_8(a,...) EXPAND(REVERSE_7(__VA_ARGS__));a;
#define REVERSE_9(a,...) EXPAND(REVERSE_8(__VA_ARGS__));a;
#define REVERSE_10(a,...) EXPAND(REVERSE_9(__VA_ARGS__));a;
#define REVERSE1(N,...) EXPAND(REVERSE_ ## N(__VA_ARGS__))
#define REVERSE(N, ...) REVERSE1(N, __VA_ARGS__)

#ifdef IS_LITTLE_ENDIAN

#define TAGGED_UNION(type, name, N, ...)  union _ ## name { type data; struct S {\
REVERSE(N, __VA_ARGS__)                                                                 \
 } __attribute__((packed)) parts;                                     \
};                                                                       \

#else
#ifdef IS_BIG_ENDIAN

#define TAGGED_UNION(type, name, N, ...)  union name { type data; struct S {\
FIELD(N, __VA_ARGS__)                                                                \
 } __attribute__((packed)) parts;                                     \
};                                                                     \

#else
    #error "Unknown configuration - NEITHER LITTLE ENDIAN NOR BIG ENDIAN"
#endif
#endif

#define TAGGED_UNION_ENCAPSULATOR_BEGIN(type, name, default_value, N) struct name {\
    static_assert(sizeof(_ ## name) == sizeof(type) && sizeof(type) == sizeof(_ ## name ::S), "Error: the bit in the bitfield are not correctly packed");\
    _ ## name id;\
    name() : name(default_value) {}\
    name(const type& x) { id.data = x; }    \
    name(const name &) = default;\
    name(name && ) = default;\
    name& operator=(const name & x) = default;\
    name& operator=(name && x) = default;\
    bool operator<(const name &rhs) const { return id.data < rhs.id.data;  }\
    bool operator>(const name &rhs) const { return rhs < *this;     }\
    bool operator<=(const name &rhs) const { return !(rhs < *this);  }\
    bool operator>=(const name &rhs) const { return !(*this < rhs);  }\
    bool operator==(const name &rhs) const { return id.data == rhs.id.data; }\
    bool operator!=(const name &rhs) const { return !(rhs == *this); }\
    name\

#define TAGGED_UNION_ENCAPSULATOR_END };

#define TAGGED_UNION_WITH_ENCAPSULATION_BEGIN(type,name, default_Value, N, ...) 		TAGGED_UNION(type,name, N, __VA_ARGS__); TAGGED_UNION_ENCAPSULATOR_BEGIN(type,name, default_Value, N)



#endif //KNOBAB_BIT_TAGGED_UNIONS_H