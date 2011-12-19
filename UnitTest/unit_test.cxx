#include "IO_netCDF.h"
#include "gradient.h"
#include "field.h"
#include "assert.h"

int main(int argc, char ** argv)
{
    if (argc < 3)
    {
        printf ("Usage: ./unit_test remap_ncfile_path test_function_name\n\tunit\n\tcoslat_coslon\n\tcosbell\n\tY2_2\n\tY16_32\n");
        return -1;
    }
    // print header information for NCL
    int pos1 = 0, pos2 = 0;
    int len = strlen (argv[1]);
    char src_grid_name[64];
    char dst_grid_name[64];
    strcpy (src_grid_name, argv[1]);
    for (int i = 0; i < len; i ++)
    {
        if (argv[1][i] == '_')
            pos1 = i;
        else if (argv[1][i] == '.')
            pos2 = i;
    }
    src_grid_name[pos1] = '\0';
    strcpy (dst_grid_name, argv[1] + pos1 + 1);
    dst_grid_name[pos2 - pos1 - 1] = '\0';
    printf ("%s\n", src_grid_name);
    printf ("%s\n", dst_grid_name);

    printf ("%s\n", argv[2]);

    IO_netCDF * cdf = new IO_netCDF ("configure");
    //cdf->Print_dims();
    //cdf->Print_vars();
    //IO_netCDF::Check_NC_Error (NC_NOERR);
    //cdf->Read_file ("test.nc");
    cdf->Read_file (argv[1]);
    //cdf->Write_file ("output.nc");

    int src_grid_size = cdf->Get_dim_by_name ("src_grid_size")->data;
    int dst_grid_size = cdf->Get_dim_by_name ("dst_grid_size")->data;
    int num_links = cdf->Get_dim_by_name ("num_links")->data;

    Var center_lat = cdf->Get_var_by_name ("src_grid_center_lat");
    Var center_lon = cdf->Get_var_by_name ("src_grid_center_lon");
    Var grid_dims = cdf->Get_var_by_name ("src_grid_dims");
    Var src_address = cdf->Get_var_by_name ("src_address");
    Var dst_address = cdf->Get_var_by_name ("dst_address");
    Var remap_matrix = cdf->Get_var_by_name ("remap_matrix");
    assert (center_lat != (Var) 0);
    assert (center_lon != (Var) 0);
    assert (src_address != (Var) 0);
    assert (dst_address != (Var) 0);
    int lon_dim = ((int *) grid_dims->data)[0];
    int lat_dim = ((int *) grid_dims->data)[1];
    //printf ("lat_dim = %d\n", lat_dim);
    //printf ("lon_dim = %d\n", lon_dim);

    double * center_lat_value = (double *) center_lat->data;
    double * center_lon_value = (double *) center_lon->data;
    double * dst_lat_deg = (double *) cdf->Get_var_by_name ("dst_grid_center_lat")->data;
    double * dst_lon_deg = (double *) cdf->Get_var_by_name ("dst_grid_center_lon")->data;
    double * dst_lat = new double [dst_grid_size];
    double * dst_lon = new double [dst_grid_size];
    double deg2rad = 3.14159265359 / 180;
    for (int i = 0; i < dst_grid_size; i ++)
    {
        dst_lat[i] = dst_lat_deg[i] * deg2rad;
        dst_lon[i] = dst_lon_deg[i] * deg2rad;
    }
    int * src_address_value = (int *) src_address->data;
    int * dst_address_value = (int *) dst_address->data;
    
    assert (center_lat_value != (double *) 0);
    assert (center_lon_value != (double *) 0);
    assert (src_address_value != (int *) 0);
    assert (dst_address_value != (int *) 0);

    int * src_address_cvalue = new int [num_links];
    int * dst_address_cvalue = new int [num_links];
    for (int i = 0; i < num_links; i ++)
    {
        src_address_cvalue[i] = src_address_value[i] - 1;
        dst_address_cvalue[i] = dst_address_value[i] - 1;
    }

    //printf ("src_address[0] = %d\n", src_address_value[0]);
    //printf ("src_grid_center_lat[0] = %f\n", ((double *)center_lat->data)[0]);

    Dim size = cdf->Get_dim_by_ID ((center_lat->dim_list)[0]);
    //Dim corners = cdf->Get_dim_by_ID ((center_lat->dim_list)[1]);
    
    //printf("size: %d\n", size->data);
    //printf("corners: %d\n", corners->data);

    //return 0;
    /*
    for (int i = 0; i < size->data; i ++)
    {
//        for (int j = 0; j < corners->data; j ++)
        printf ("%3.6f  ", center_lat_value[i]);
        if (i % 4 == 3)
            printf ("\n");
    }
    printf ("\n");
    */

    grad_latlon * grad = new grad_latlon (lat_dim, lon_dim, center_lat_value, center_lon_value);
    grad->Calculate_grad_latlon_matrix();

    // test gradient method
    //grad->Test_grad_latlon (&function_coslat_coslon, &partial_lat_function_coslat_coslon, &partial_lon_function_coslat_coslon);

    //grad->Get_grad_lat_matrix ()->print ();
    //grad->Get_grad_lon_matrix ()->print ();
    //SparseMatrix * transposed = grad->Get_grad_lon_matrix ()->Matrix_transpose();
    //transposed->print ();
    //return 0;
    double * w = (double *) (cdf->Get_var_by_name ("remap_matrix")->data);
    double * w1 = new double [num_links];
    double * w2lat = new double [num_links];
    double * w2lon = new double [num_links];
    int wid = 0;
    for (int i = 0; i < num_links; i ++)
    {
        w1[i] = w[wid++];
        w2lat[i] = w[wid++];
        w2lon[i] = w[wid++];
    }
#if 0
    for (int i = 0; i < num_links; i ++)
    {
        printf ("%6d\t%6d\t\t\t%3.6f\n", dst_address_value[i], src_address_value[i], w1[i]);
    }
#endif
    SparseMatrix * m1 = new SparseMatrix(dst_grid_size, src_grid_size, num_links, dst_address_cvalue, src_address_cvalue, w1);
    SparseMatrix * m2lat = new SparseMatrix(dst_grid_size, src_grid_size, num_links, dst_address_cvalue, src_address_cvalue, w2lat);
    SparseMatrix * m2lon = new SparseMatrix(dst_grid_size, src_grid_size, num_links, dst_address_cvalue, src_address_cvalue, w2lon);
    delete w1;
    delete w2lat;
    delete w2lon;
    delete src_address_cvalue;
    delete dst_address_cvalue;
    if (strcmp (argv[2], "unit") == 0)
        grad->Test_final_results (m1, m2lat, m2lon, &function_unit, &partial_lat_function_unit, &partial_lon_function_unit, dst_lat, dst_lon);
    else if (strcmp (argv[2], "coslat_coslon") == 0)
        grad->Test_final_results (m1, m2lat, m2lon, &function_coslat_coslon, &partial_lat_function_coslat_coslon, &partial_lon_function_coslat_coslon, dst_lat, dst_lon);
    else if (strcmp (argv[2], "cosbell") == 0)
        grad->Test_final_results (m1, m2lat, m2lon, &function_cosbell, &partial_lat_function_cosbell, &partial_lon_function_cosbell, dst_lat, dst_lon);
    else if (strcmp (argv[2], "Y2_2") == 0)
        grad->Test_final_results (m1, m2lat, m2lon, &function_spherical_harmonic_2_2, &partial_lat_function_spherical_harmonic_2_2, &partial_lon_function_spherical_harmonic_2_2, dst_lat, dst_lon);
    else if (strcmp (argv[2], "Y16_32") == 0)
        grad->Test_final_results (m1, m2lat, m2lon, &function_spherical_harmonic_16_32, &partial_lat_function_spherical_harmonic_16_32, &partial_lon_function_spherical_harmonic_16_32, dst_lat, dst_lon);
    else
        printf ("Un-supported test function!\nUse the following ones:\n\tunit\n\tcoslat_coslon\n\tcosbell\n\tY2_2\n\tY16_32\n");
    delete dst_lat;
    delete dst_lon;
    return 0;
    SparseMatrix * final = grad->Calculate_final_matrix (m1, m2lat, m2lon);
    return 0;
}
