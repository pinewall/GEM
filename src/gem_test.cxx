#include "IO_netCDF.h"
#include "field.h"
#include <assert.h>

int main(int argc, char ** argv)
{
    if (argc < 4)
    {
        printf ("Usage: ./gem remap1.nc remap2.nc testcase\n");
        return -1;
    }
    IO_netCDF * cdf = new IO_netCDF ("configure");
    cdf->Read_file (argv[1]);
    return 0;
}
