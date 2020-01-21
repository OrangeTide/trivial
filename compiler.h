/* compiler.h - compiler macros
 * PUBLIC DOMAIN - Jon Mayo - October 15, 2007
 * initial: 2007-10-15
 */
#ifndef COMPILER_H
#define COMPILER_H

#ifdef __GNUC__
/* using GCC, enable special GCC options */
#define GCC_ONLY(x) x
#else
/* not using GCC */
#define GCC_ONLY(x)
#endif

/* mark a parameter as unused */
#define UNUSED GCC_ONLY(__attribute__((unused)))

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
/* enable C99 features */
#define C99_ONLY(x) x
#else
/* C89 mode */
#define C99_ONLY(x)
#endif

/* C99 style initialization designator */
#define ARRAY_DESIGNATOR(i) C99_ONLY([i]=)

/* Endian detection */
#if defined(__gnu_linux__)
#include <endian.h>
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <machine/endian.h>
#endif

#if defined(__APPLE__) && (_BIG_ENDIAN==1 || __BIG_ENDIAN==1)
#define HAS_BIG_ENDIAN
#elif defined(__APPLE__) && (_LITTLE_ENDIAN==1 || __LITTLE_ENDIAN==1)
#define HAS_LITTLE_ENDIAN
#elif (defined(__BYTE_ORDER) && __BYTE_ORDER ==  __BIG_ENDIAN) || (defined(_BYTE_ORDER) && _BYTE_ORDER ==  _BIG_ENDIAN)
#define HAS_BIG_ENDIAN
#elif (defined(__BYTE_ORDER) && __BYTE_ORDER ==  __LITTLE_ENDIAN) || (defined(_BYTE_ORDER) && _BYTE_ORDER ==  _LITTLE_ENDIAN)
#define HAS_LITTLE_ENDIAN
#elif defined(__hppa__) || defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || (defined(__MIPS__) && defined(__MISPEB__)) || defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || defined(__sparc__)
#define HAS_BIG_ENDIAN
#elif defined(_WIN32) || defined(_WIN64)
/* assume WIN32/WIN64 are PCs or little-endian WINCE devices. */
#define HAS_LITTLE_ENDIAN
#else
#error Could not determine endian / byte-order.
#endif

/* Thread Local Storage (TLS) storage class specifier */
#if defined(__VC32__)
/* Visual C++, Intel C/C++ (windows), Digital Mars C++, Borland C++ Builder */
#define thread __declspec(thread)
#else
/* GCC, Intel C/C++ (linux), Sun Studio C/C++, IBM XL C/C++ */
#define thread __thread
#endif

#endif
