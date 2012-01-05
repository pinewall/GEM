#ifndef TYPES_H
#define TYPES_H

#define USE_32BITS
#ifdef USE_64BITS
    typedef unsigned long CDF_INT;
    typedef long INT;
#endif
#ifdef USE_32BITS
    typedef int INT;
    typedef unsigned int CDF_INT;
#endif

//#define USE_DEGREES
#define USE_RADIANS

#endif
