#ifndef FIELD_H
#define FIELD_H

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
#endif
