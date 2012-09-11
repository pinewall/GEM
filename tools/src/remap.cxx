#include <stdio.h>
#include <netcdf.h>
#include <string.h>
#include "types.h"

#define NC_CHECK(error_id) {Check_NC_Error(error_id);}
inline void Check_NC_Error (int error_id)
{
    if (error_id != NC_NOERR)
    {
        //fprintf(stderr, "NC operation failed!\n");
        fprintf(stderr, "%s\n", nc_strerror (error_id));
        return;
    }
}

int main (int argc, char ** argv)
{
    if (argc < 3)
    {
        fprintf (stderr, "Usage: ./remap source-field-ncfile remap-matrix-ncfile destination-field-ncfile\n");
        return -1;
    }
    int nc_src_fid, nc_remap_fid, nc_dst_fid;           // file ncid
    int nc_src_grid_size_id, nc_dst_grid_size_id, nc_dst_grid_rank_id;       // dimension id
    int nc_src_address_id, nc_dst_address_id, nc_remap_matrix_id, nc_num_links_id, nc_num_wgts_id;  // remap file
    int nc_dst_grid_dims_id, nc_dst_grid_imask_id, nc_dst_grid_center_lat_id, nc_dst_grid_center_lon_id;                  // output file
    int nc_physical_variable_id;                                                                    // for variable
    size_t src_grid_size, dst_grid_size, remap_src_grid_size, num_links, num_wgts;
    NC_CHECK (nc_open (argv[1], NC_NOWRITE, &nc_src_fid));
    NC_CHECK (nc_open (argv[2], NC_NOWRITE, &nc_remap_fid));

    // dimension check
    NC_CHECK (nc_inq_dimid (nc_src_fid, "grid_size", &nc_src_grid_size_id));
    NC_CHECK (nc_inq_dimlen (nc_src_fid, nc_src_grid_size_id, &src_grid_size));
    
    NC_CHECK (nc_inq_dimid (nc_remap_fid, "src_grid_size", &nc_src_grid_size_id));
    NC_CHECK (nc_inq_dimlen (nc_remap_fid, nc_src_grid_size_id, &remap_src_grid_size));
    
    NC_CHECK (nc_inq_dimid (nc_remap_fid, "dst_grid_size", &nc_dst_grid_size_id));
    NC_CHECK (nc_inq_dimlen (nc_remap_fid, nc_dst_grid_size_id, &dst_grid_size));
    
    NC_CHECK (nc_inq_dimid (nc_remap_fid, "num_links", &nc_num_links_id));
    NC_CHECK (nc_inq_dimlen (nc_remap_fid, nc_num_links_id, &num_links));
    
    NC_CHECK (nc_inq_dimid (nc_remap_fid, "num_wgts", &nc_num_wgts_id));
    NC_CHECK (nc_inq_dimlen (nc_remap_fid, nc_num_wgts_id, &num_wgts));
    
    if (src_grid_size != remap_src_grid_size)
    {
        fprintf (stderr, "Remap file and field do not match\n");
        return -2;
    }
    else
    {
        printf ("Remap file and field match\n");
    }
    double * src_field = new double [src_grid_size];
    double * dst_field = new double [dst_grid_size];
    int * dst_grid_imask = new int [dst_grid_size];
    double * dst_grid_center_lat = new double [dst_grid_size];
    double * dst_grid_center_lon = new double [dst_grid_size];
    int * src_address = new int [num_links];
    int * dst_address = new int [num_links];
    double * remap_matrix = new double [num_links * num_wgts];
    int dst_grid_dims[2];
	char dst_lat_units[64];
	char dst_lon_units[64];
	strcpy(dst_lat_units,"");
	strcpy(dst_lon_units,"");

    // get data from src filed
    NC_CHECK (nc_inq_varid (nc_src_fid, "physical_variable", &nc_physical_variable_id));
    NC_CHECK (nc_get_var_double (nc_src_fid, nc_physical_variable_id, src_field));
    NC_CHECK (nc_close (nc_src_fid));
    printf ("get src field ---- OK\n");
#if 0
    printf ("print\n");
    for (size_t i = 0; i < src_grid_size; i ++)
        printf ("%f\n", src_field[i]);
#endif
    // get data from remap file
    NC_CHECK (nc_inq_varid (nc_remap_fid, "src_address", &nc_src_address_id));
    NC_CHECK (nc_get_var_int (nc_remap_fid, nc_src_address_id, src_address));
    NC_CHECK (nc_inq_varid (nc_remap_fid, "dst_address", &nc_dst_address_id));
    NC_CHECK (nc_get_var_int (nc_remap_fid, nc_dst_address_id, dst_address));
    NC_CHECK (nc_inq_varid (nc_remap_fid, "remap_matrix", &nc_remap_matrix_id));
    NC_CHECK (nc_get_var_double (nc_remap_fid, nc_remap_matrix_id, remap_matrix));
    NC_CHECK (nc_inq_varid (nc_remap_fid, "dst_grid_center_lat", &nc_dst_grid_center_lat_id));
	NC_CHECK (nc_get_att_text (nc_remap_fid, nc_dst_grid_center_lat_id, "units", dst_lat_units));
    NC_CHECK (nc_get_var_double (nc_remap_fid, nc_dst_grid_center_lat_id, dst_grid_center_lat));
    NC_CHECK (nc_inq_varid (nc_remap_fid, "dst_grid_center_lon", &nc_dst_grid_center_lon_id));
	NC_CHECK (nc_get_att_text (nc_remap_fid, nc_dst_grid_center_lon_id, "units", dst_lon_units));
    NC_CHECK (nc_get_var_double (nc_remap_fid, nc_dst_grid_center_lon_id, dst_grid_center_lon));
    NC_CHECK (nc_inq_varid (nc_remap_fid, "dst_grid_imask", &nc_dst_grid_imask_id));
    NC_CHECK (nc_get_var_int (nc_remap_fid, nc_dst_grid_imask_id, dst_grid_imask));
    NC_CHECK (nc_inq_varid (nc_remap_fid, "dst_grid_dims", &nc_dst_grid_dims_id));
    NC_CHECK (nc_get_var_int (nc_remap_fid, nc_dst_grid_dims_id, dst_grid_dims));
    NC_CHECK (nc_close (nc_remap_fid));
    printf ("get remap matrix ---- OK\n");
#if 0
    for (size_t i = 0; i < num_links; i ++)
        printf ("src_address[%6d] = %d\n", i, src_address[i]);
    for (size_t i = 0; i < num_links; i ++)
        printf ("dst_address[%6d] = %d\n", i, dst_address[i]);
    for (size_t i = 0; i < num_links; i ++)
        printf ("remap_matrix[%6d] = %f\n", i, remap_matrix[i]);
#endif
    // calculate dst field result
    for (size_t i = 0; i < dst_grid_size; i ++)
        dst_field[i] = 0.0;
    for (size_t i = 0; i < num_links; i ++)
        dst_field[dst_address[i]-1] += remap_matrix[i * num_wgts] * src_field[src_address[i]-1];
    printf ("remap job ---- OK\n");
    //delete[] src_field;
    delete[] src_address;
    delete[] dst_address;
    delete[] remap_matrix;
#if 0
    printf ("print\n");
    for (size_t i = 0; i < dst_grid_size; i ++)
        printf ("%f\n", dst_field[i]);
#endif

    NC_CHECK (nc_create (argv[3], NC_CLOBBER, &nc_dst_fid));
    NC_CHECK (nc_def_dim (nc_dst_fid, "grid_size", dst_grid_size, &nc_dst_grid_size_id));
    NC_CHECK (nc_def_dim (nc_dst_fid, "grid_rank", 2, &nc_dst_grid_rank_id));
    NC_CHECK (nc_def_var (nc_dst_fid, "grid_dims", NC_INT, 1, &nc_dst_grid_rank_id, &nc_dst_grid_dims_id));
    NC_CHECK (nc_def_var (nc_dst_fid, "grid_imask", NC_INT, 1, &nc_dst_grid_size_id, &nc_dst_grid_imask_id));
    NC_CHECK (nc_def_var (nc_dst_fid, "grid_center_lat", NC_DOUBLE, 1, &nc_dst_grid_size_id, &nc_dst_grid_center_lat_id));
	NC_CHECK (nc_put_att_text (nc_dst_fid, nc_dst_grid_center_lat_id, "units", strlen(dst_lat_units), dst_lat_units));
    NC_CHECK (nc_def_var (nc_dst_fid, "grid_center_lon", NC_DOUBLE, 1, &nc_dst_grid_size_id, &nc_dst_grid_center_lon_id));
	NC_CHECK (nc_put_att_text (nc_dst_fid, nc_dst_grid_center_lon_id, "units", strlen(dst_lon_units), dst_lon_units));
    NC_CHECK (nc_def_var (nc_dst_fid, "physical_variable", NC_DOUBLE, 1, &nc_dst_grid_size_id, &nc_physical_variable_id));

    NC_CHECK (nc_enddef (nc_dst_fid));
    NC_CHECK (nc_put_var_int (nc_dst_fid, nc_dst_grid_imask_id, dst_grid_imask));
    NC_CHECK (nc_put_var_int (nc_dst_fid, nc_dst_grid_dims_id, dst_grid_dims));
    NC_CHECK (nc_put_var_double (nc_dst_fid, nc_dst_grid_center_lat_id, dst_grid_center_lat));
    NC_CHECK (nc_put_var_double (nc_dst_fid, nc_dst_grid_center_lon_id, dst_grid_center_lon));
    NC_CHECK (nc_put_var_double (nc_dst_fid, nc_physical_variable_id, dst_field));

    NC_CHECK (nc_close (nc_dst_fid));
    printf ("write remap matrix ---- OK\n");

    //delete[] dst_field;
    //delete[] dst_grid_imask;
    //delete[] dst_grid_center_lat;
    //delete[] dst_grid_center_lon;
    return 0;
}
