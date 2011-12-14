#include "field.h"
#include <math.h>

double function_coslat_coslon (double lat, double lon)
{
    return cos(lat) * cos(lon);
}

double partial_lat_function_coslat_coslon (double lat, double lon)
{
    return - sin(lat) * cos(lon);
}

double partial_lon_function_coslat_coslon (double lat, double lon)
{
    return -sin(lon);
}
