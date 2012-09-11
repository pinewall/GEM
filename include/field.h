#ifndef FIELD_H
#define FIELD_H

#define SCRIP
#include "types.h"

const _ValueType FIELD_PI = 3.14159265359;

_ValueType function_coslat_coslon (_ValueType lat, _ValueType lon);
_ValueType partial_lat_function_coslat_coslon (_ValueType lat, _ValueType lon);
_ValueType partial_lon_function_coslat_coslon (_ValueType lat, _ValueType lon);

_ValueType function_unit (_ValueType lat, _ValueType lon);
_ValueType partial_lat_function_unit (_ValueType lat, _ValueType lon);
_ValueType partial_lon_function_unit (_ValueType lat, _ValueType lon);

_ValueType function_cosbell (_ValueType lat, _ValueType lon);
_ValueType partial_lat_function_cosbell (_ValueType lat, _ValueType lon);
_ValueType partial_lon_function_cosbell (_ValueType lat, _ValueType lon);

// spherical harmonic l=2, m=2
_ValueType function_spherical_harmonic_2_2 (_ValueType lat, _ValueType lon);
_ValueType partial_lat_function_spherical_harmonic_2_2 (_ValueType lat, _ValueType lon);
_ValueType partial_lon_function_spherical_harmonic_2_2 (_ValueType lat, _ValueType lon);

// area-averaged spherical harmonic l=2, m=2
_ValueType function_aa_spherical_harmonic_2_2 (_ValueType lat, _ValueType lon, _ValueType dlat, _ValueType dlon);
_ValueType partial_lat_aa_function_spherical_harmonic_2_2 (_ValueType lat, _ValueType lon, _ValueType dlat, _ValueType dlon);
_ValueType partial_lon_aa_function_spherical_harmonic_2_2 (_ValueType lat, _ValueType lon, _ValueType dlat, _ValueType dlon);

// spherical harmonic l=16, m=32
_ValueType function_spherical_harmonic_16_32 (_ValueType lat, _ValueType lon);
_ValueType partial_lat_function_spherical_harmonic_16_32 (_ValueType lat, _ValueType lon);
_ValueType partial_lon_function_spherical_harmonic_16_32 (_ValueType lat, _ValueType lon);
#endif
