#include "IO_netCDF.h"
#include <stdio.h>

int main (int argc, char ** argv)
{
    if (argc < 4)
    {
        printf ("Usage: ./cdfcopyer input.nc select.xml output.nc\n");
        return -1;
    }
    IO_netCDF * copyer = new IO_netCDF (argv[2]);
    //copyer->Print_dims ();
    //copyer->Print_vars ();
    //copyer->Print_global_preps ();
    copyer->Read_file (argv[1]);
    copyer->Special_case ();
    copyer->Signiture ("pinewall", "2012-01-04", "T42");
    copyer->Write_file (argv[3]);
    return 0;
}
