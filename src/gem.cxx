#include "IO_netCDF.h"
#include "gradient.h"
#include "field.h"
#include "types.h"
#include <assert.h>

int main(int argc, char ** argv)
{
    if (argc < 4)
    {
        printf ("Usage: ./gem input_nc_filename output_nc_filename xml_meta\n");
        return -1;
    }
    IO_netCDF * cdf = new IO_netCDF (argv[3]);
    cdf->Read_file (argv[1]);

    CDF_INT src_grid_size = cdf->Get_dim_by_gname ("src_size")->data;
    CDF_INT dst_grid_size = cdf->Get_dim_by_gname ("dst_size")->data;
    Dim dim_num_links = cdf->Get_dim_by_gname ("num_links");
    CDF_INT num_links = dim_num_links->data;
    Dim dim_num_wgts = cdf->Get_dim_by_gname ("num_wgts");
    CDF_INT num_wgts = dim_num_wgts->data;

    Var center_lat = cdf->Get_var_by_gname ("src_center_lat");
    Var center_lon = cdf->Get_var_by_gname ("src_center_lon");
    Var grid_dims = cdf->Get_var_by_gname ("src_dims");
    Var src_address = cdf->Get_var_by_gname ("src_address");
    Var dst_address = cdf->Get_var_by_gname ("dst_address");
    Var remap_matrix = cdf->Get_var_by_gname ("remap_matrix");
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
    double * cdf_dst_lat = (double *) cdf->Get_var_by_gname ("dst_center_lat")->data;
    double * cdf_dst_lon = (double *) cdf->Get_var_by_gname ("dst_center_lon")->data;
    double * dst_lat = new double [dst_grid_size];
    double * dst_lon = new double [dst_grid_size];
    double deg2rad = 3.14159265359 / 180;
    if (strcmp (cdf->Get_var_by_gname ("dst_center_lat")->prep_list[0]->info, "degrees") == 0)
    {
        for (int i = 0; i < dst_grid_size; i ++)
        {
            dst_lat[i] = cdf_dst_lat[i] * deg2rad;
            dst_lon[i] = cdf_dst_lon[i] * deg2rad;
        }
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
    //SparseMatrix * final = new SparseMatrix (1, 1);
    cdf->Modify_dim_data ("num_links", final->Get_current_size ());
    cdf->Modify_dim_data ("num_wgts", 1);
    cdf->Modify_var_data ("src_address", final->Get_col_list ());
    cdf->Modify_var_data ("dst_address", final->Get_row_list ());
    cdf->Modify_var_data ("remap_matrix", final->Get_value_list ());
    cdf->Print_dims ();
    cdf->Print_vars ();
    cdf->Print_global_preps ();
    cdf->Write_file (argv[2]);
    printf ("final remap matrix size: %d\n", final->Get_current_size ());
    delete final;
    delete cdf;
    return 0;
}
