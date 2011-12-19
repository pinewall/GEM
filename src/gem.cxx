#include "IO_netCDF.h"
#include "gradient.h"
#include "field.h"
#include "assert.h"

int main(int argc, char ** argv)
{
    if (argc < 3)
    {
        printf ("Usage: ./gem input_nc_filename output_nc_filename\n");
        return -1;
    }
    IO_netCDF * cdf = new IO_netCDF ("configure");
    cdf->Read_file (argv[1]);

    int src_grid_size = cdf->Get_dim_by_name ("src_grid_size")->data;
    int dst_grid_size = cdf->Get_dim_by_name ("dst_grid_size")->data;
    Dim dim_num_links = cdf->Get_dim_by_name ("num_links");
    int num_links = dim_num_links->data;

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

    Dim size = cdf->Get_dim_by_ID ((center_lat->dim_list)[0]);

    grad_latlon * grad = new grad_latlon (lat_dim, lon_dim, center_lat_value, center_lon_value);
    grad->Calculate_grad_latlon_matrix();

    double * w = (double *) (remap_matrix->data);
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
    SparseMatrix * m1 = new SparseMatrix(dst_grid_size, src_grid_size, num_links, dst_address_cvalue, src_address_cvalue, w1);
    SparseMatrix * m2lat = new SparseMatrix(dst_grid_size, src_grid_size, num_links, dst_address_cvalue, src_address_cvalue, w2lat);
    SparseMatrix * m2lon = new SparseMatrix(dst_grid_size, src_grid_size, num_links, dst_address_cvalue, src_address_cvalue, w2lon);
    delete w1;
    delete w2lat;
    delete w2lon;
    delete src_address_cvalue;
    delete dst_address_cvalue;
    delete dst_lat;
    delete dst_lon;
    // SparseMatrix final should be <dst_grid_size x src_grid_size>
    SparseMatrix * final = grad->Calculate_final_matrix (m1, m2lat, m2lon);
    cdf->Modify_dim_data (dim_num_links->ID, final->current_size);
    cdf->Modify_var_data (src_address->ID, final->Get_col_list ());
    cdf->Modify_var_data (dst_address->ID, final->Get_row_list ());
    cdf->Modify_var_data (remap_matrix->ID, final->Get_value_list ());
    cdf->Write_file (argv[2]);
    return 0;
}
