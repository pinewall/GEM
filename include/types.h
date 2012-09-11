#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdlib.h>

#include <iostream>

//#define USE_64BITS
//#ifdef USE_64BITS
//    typedef unsigned long CDF_INT;
//    typedef long INT;
//#endif
//#ifdef USE_32BITS
//    typedef int INT;
//    typedef unsigned int CDF_INT;
//#endif
typedef size_t CDF_INT;
typedef long INT;

//#define USE_DEGREES
#define USE_RADIANS

typedef int _IDX; 
//typedef double _ValueType;
typedef long double _ValueType;
typedef _IDX rowPtr;
typedef _IDX rowList;
typedef _IDX colList;
typedef _ValueType valList;

const _ValueType pi = 3.14159265359;
const _ValueType deg2rad = pi / 180.0;
const _ValueType rad2deg = 180.0 / pi;

#endif
