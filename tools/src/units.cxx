#include <stdio.h>
#include "IO_netCDF.h"
#include "types.h"

int main(int argc, char ** argv)
{
    IO_netCDF * cdf = new IO_netCDF (argv[2]);
    cdf->Read_file (argv[1]);
    cdf->Modify_var_prep ("grid_center_lat", "units", "degrees");
    cdf->Modify_var_prep ("grid_center_lon", "units", "degrees");
    cdf->Modify_var_prep ("grid_corner_lat", "units", "degrees");
    cdf->Modify_var_prep ("grid_corner_lon", "units", "degrees");
    cdf->Write_file (argv[3]);
    return 0;
}
