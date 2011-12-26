#ifndef TYPES_H
#define TYPES_H

#define USE_32BITS
#ifdef USE_64BITS
typedef unsigned long UINT;
#endif
#ifdef USE_32BITS
typedef unsigned int UINT;
#endif

#endif
