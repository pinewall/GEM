#include "IO_netCDF.h"
#include "gradient.h"
#include "field.h"
#include "assert.h"

int main(int argc, char ** argv)
{
    IO_netCDF * cdf = new IO_netCDF ("configure");
    //cdf->Print_dims();
    //cdf->Print_vars();
    //IO_netCDF::Check_NC_Error (NC_NOERR);
    cdf->Read_file ("test.nc");
    //cdf->Write_file ("output.nc");

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

    double * center_lat_value = (double *) center_lat->data;
    double * center_lon_value = (double *) center_lon->data;
    int * src_address_value = (int *) src_address->data;
    int * dst_address_value = (int *) dst_address->data;

    assert (center_lat_value != (double *) 0);
    assert (center_lon_value != (double *) 0);
    assert (src_address_value != (int *) 0);
    assert (dst_address_value != (int *) 0);

    //printf ("src_address[0] = %d\n", src_address_value[0]);
    //printf ("src_grid_center_lat[0] = %f\n", ((double *)center_lat->data)[0]);

    Dim size = cdf->Get_dim_by_ID ((center_lat->dim_list)[0]);
    //Dim corners = cdf->Get_dim_by_ID ((center_lat->dim_list)[1]);
    
    printf("size: %d\n", size->data);
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

    int lon_dim = ((int *) grid_dims->data)[0];
    int lat_dim = ((int *) grid_dims->data)[1];
    printf ("lat_dim = %d\n", lat_dim);
    printf ("lon_dim = %d\n", lon_dim);
    grad_latlon * grad = new grad_latlon (lat_dim, lon_dim, center_lat_value, center_lon_value);
    grad->Calculate_grad_latlon_matrix();

    // test gradient method
    grad->Test_grad_latlon (&function_coslat_coslon, &partial_lat_function_coslat_coslon, &partial_lon_function_coslat_coslon);
    //grad->Get_grad_lat_matrix ()->print ();
    grad->Get_grad_lon_matrix ()->print ();
    SparseMatrix * transposed = grad->Get_grad_lon_matrix ()->Matrix_transpose();
    transposed->print ();
    //return 0;
    int num_links = cdf->Get_dim_by_name ("num_links")->data;
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
    SparseMatrix * m1 = new SparseMatrix(8192, 7680, num_links, dst_address_value, src_address_value, w1);
    SparseMatrix * m2lat = new SparseMatrix(8192, 7680, num_links, dst_address_value, src_address_value, w2lat);
    SparseMatrix * m2lon = new SparseMatrix(8192, 7680, num_links, dst_address_value, src_address_value, w2lon);
    delete w1;
    delete w2lat;
    delete w2lon;
    return 0;
    SparseMatrix * final = grad->Calculate_final_matrix (m1, m2lat, m2lon);
    return 0;
}
