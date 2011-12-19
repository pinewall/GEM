#include "field.h"
#include <math.h>

// function y=cos(lat)*cos(lon)
double function_coslat_coslon (double lat, double lon)
{
    return 2 + cos(lat) * cos(lon);
}

double partial_lat_function_coslat_coslon (double lat, double lon)
{
    return - sin(lat) * cos(lon);
}

double partial_lon_function_coslat_coslon (double lat, double lon)
{
    return -sin(lon);
}

// function y=1
double function_unit (double lat, double lon)
{
    return 1.0;
}

double partial_lat_function_unit (double lat, double lon)
{
    return 0.0;
}

double partial_lon_function_unit (double lat, double lon)
{
    return 0.0;
}

// function y = cos(sqrt((lon - pi)^2 + lat^2))
#ifndef SCRIP
double function_cosbell (double lat, double lon)
{
    return 2 + cos (sqrt ((lon - FIELD_PI) * (lon - FIELD_PI) + lat * lat));
}

double partial_lat_function_cosbell (double lat, double lon)
{
    double sqr = sqrt ((lon - FIELD_PI) * (lon - FIELD_PI) + lat * lat);
    if (sqr < 1e-10)
        return 0.0;
    return sin (sqr) * lat / sqr;
}

double partial_lon_function_cosbell (double lat, double lon)
{
    double sqr = sqrt ((lon - FIELD_PI) * (lon - FIELD_PI) + lat * lat);
    if (sqr < 1e-10)
        return 0.0;
    return sin (sqr) * (lon - FIELD_PI) / sqr / cos(lat);
}
#endif
#ifdef SCRIP
// test functions defined in SCRIP (scrip_test.f)
double function_cosbell (double lat, double lon)
{
    double result;
    double length = 0.1 * FIELD_PI * 2;
    result = cos (lat) * cos (lon);
    double tmp = acos (-result) / length;
    if (tmp <= 1.0)
        result = 2.0 + cos (tmp * FIELD_PI);
    else
        result = 1.0;
    return result;
}

double partial_lat_function_cosbell (double lat, double lon)
{
    double result;
    double length = 0.1 * FIELD_PI * 2;
    result = cos (lat) * cos (lon);
    double tmp = acos (-result) / length;
    if (tmp <= 1.0)
        result = FIELD_PI / length * sin (FIELD_PI * tmp) * sin (lat) * cos (lon) / sqrt (1 - result * result);
    else
        result = 0.0;
    return result;
}

double partial_lon_function_cosbell (double lat, double lon)
{
    double result;
    double length = 0.1 * FIELD_PI * 2;
    result = cos (lat) * cos (lon);
    double tmp = acos (-result) / length;
    if (tmp <= 1.0)
        result = FIELD_PI / length * sin (FIELD_PI * tmp) * sin (lon) / sqrt (1 - result * result);
    else
        result = 0.0;
    return result;
}

#endif

// spherical harmonic l=2, m=2
double function_spherical_harmonic_2_2 (double lat, double lon)
{
    double result = 2.0 + cos (lat) * cos (lat) * cos (2.0 * lon);
    return result;
}

double partial_lat_function_spherical_harmonic_2_2 (double lat, double lon)
{
    double result = - sin (2.0 * lat) * cos (2.0 * lon);
    return result;
}

double partial_lon_function_spherical_harmonic_2_2 (double lat, double lon)
{
    double result = - 2.0 * cos (lat) * sin (2.0 * lon);
    return result;
}

// spherical harmonic l=16, m=32
double function_spherical_harmonic_16_32 (double lat, double lon)
{
    double result = 2.0 + pow (sin (2.0 * lat), 16) * cos (16.0 * lon);
    return result;
}

double partial_lat_function_spherical_harmonic_16_32 (double lat, double lon)
{
    double result = 32.0 * pow (sin (2.0 * lat), 15) * cos (2.0 * lat) * cos (16.0 * lon);
    return result;
}

double partial_lon_function_spherical_harmonic_16_32 (double lat, double lon)
{
    double result = - 32.0 * pow (sin (2.0 * lat), 15) * sin (lat) * sin (16.0 * lon);
    return result;
}
