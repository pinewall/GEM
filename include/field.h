#ifndef FIELD_H
#define FIELD_H

#define SCRIP
const double FIELD_PI = 3.14159265359;

double function_coslat_coslon (double lat, double lon);
double partial_lat_function_coslat_coslon (double lat, double lon);
double partial_lon_function_coslat_coslon (double lat, double lon);

double function_unit (double lat, double lon);
double partial_lat_function_unit (double lat, double lon);
double partial_lon_function_unit (double lat, double lon);

double function_cosbell (double lat, double lon);
double partial_lat_function_cosbell (double lat, double lon);
double partial_lon_function_cosbell (double lat, double lon);

// spherical harmonic l=2, m=2
double function_spherical_harmonic_2_2 (double lat, double lon);
double partial_lat_function_spherical_harmonic_2_2 (double lat, double lon);
double partial_lon_function_spherical_harmonic_2_2 (double lat, double lon);

// spherical harmonic l=16, m=32
double function_spherical_harmonic_16_32 (double lat, double lon);
double partial_lat_function_spherical_harmonic_16_32 (double lat, double lon);
double partial_lon_function_spherical_harmonic_16_32 (double lat, double lon);
#endif
