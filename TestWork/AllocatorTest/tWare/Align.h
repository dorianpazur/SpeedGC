#ifndef TWARE_ALIGN_H
#define TWARE_ALIGN_H

#if defined(__GNUC__) || defined(__clang__)
#define ALIGN(x) __attribute__((__aligned__(x)))
#elif defined(_MSC_VER)
#define ALIGN(x) __declspec(align(x))
#else
#define ALIGN(x)
#endif

#endif
