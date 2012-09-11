#include "IO_netCDF.h"
#include "gradient.h"
#include "field.h"
#include "types.h"
#include <assert.h>

int main(int argc, char ** argv)
{
    if (argc < 4)
    {
        printf ("Usage: ./gem input_nc_filename input_nc_xml output_nc_filename\n");
        return -1;
    }
    IO_netCDF * cdf = new IO_netCDF (argv[2]);
    cdf->Read_file (argv[1]);

    size_t src_grid_size = cdf->Get_dim_by_gname ("src_grid_size")->data;
    size_t dst_grid_size = cdf->Get_dim_by_gname ("dst_grid_size")->data;
    Dim dim_num_links = cdf->Get_dim_by_gname ("num_links");
    Dim dim_num_wgts = cdf->Get_dim_by_gname ("num_wgts");
    size_t num_links = dim_num_links->data;
    size_t num_wgts = dim_num_wgts->data;
	printf("num_links: %ld\n", num_links);
	printf("num_wgts: %ld\n", num_wgts);

    Var src_center_lat = cdf->Get_var_by_gname ("src_grid_center_lat");
    Var src_center_lon = cdf->Get_var_by_gname ("src_grid_center_lon");
    Var dst_center_lat = cdf->Get_var_by_gname ("dst_grid_center_lat");
    Var dst_center_lon = cdf->Get_var_by_gname ("dst_grid_center_lon");
    Var src_grid_dims = cdf->Get_var_by_gname ("src_grid_dims");
    Var src_address = cdf->Get_var_by_gname ("src_address");
    Var dst_address = cdf->Get_var_by_gname ("dst_address");
    Var remap_matrix = cdf->Get_var_by_gname ("remap_matrix");
    assert (src_center_lat != (Var) 0);
    assert (src_center_lon != (Var) 0);
    assert (dst_center_lat != (Var) 0);
    assert (dst_center_lon != (Var) 0);
    assert (src_address != (Var) 0);
    assert (dst_address != (Var) 0);
    assert (remap_matrix != (Var) 0);

    int src_lon_dim = ((int *) src_grid_dims->data)[0];
    int src_lat_dim = ((int *) src_grid_dims->data)[1];
    printf ("src_lat_dim = %d\n", src_lat_dim);
    printf ("src_lon_dim = %d\n", src_lon_dim);

    double * dp_src_center_lat = (double *) src_center_lat->data;
    double * dp_src_center_lon = (double *) src_center_lon->data;
    //double * dp_dst_center_lat = (double *) dst_center_lat->data;
    //double * dp_dst_center_lon = (double *) dst_center_lon->data;
    
    // load src|dst _center_ lat|lon
    _ValueType * d_src_center_lat = new _ValueType [src_grid_size];
    _ValueType * d_src_center_lon = new _ValueType [src_grid_size];
    //_ValueType * d_dst_center_lat = new _ValueType [dst_grid_size];
    //_ValueType * d_dst_center_lon = new _ValueType [dst_grid_size];
    assert (d_src_center_lat != (_ValueType *) 0);
    assert (d_src_center_lon != (_ValueType *) 0);
    //assert (d_dst_center_lat != (_ValueType *) 0);
    //assert (d_dst_center_lon != (_ValueType *) 0);
    for (int i = 0; i < src_grid_size; i ++)
    {
        d_src_center_lat[i] = (_ValueType) dp_src_center_lat[i];
        d_src_center_lon[i] = (_ValueType) dp_src_center_lon[i];
    }
	if (strcmp (src_center_lat->prep_list[0]->info, "degrees") == 0)
	{
		for (int i = 0; i < src_grid_size; i ++)
		{
            d_src_center_lat[i] = d_src_center_lat[i] * deg2rad;
            d_src_center_lon[i] = d_src_center_lon[i] * deg2rad;
		}
	}
    //for (int i = 0; i < dst_grid_size; i ++)
    //{
    //    d_dst_center_lat[i] = (_ValueType) dp_dst_center_lat[i];
    //    d_dst_center_lon[i] = (_ValueType) dp_dst_center_lon[i];
    //}

    //if (strcmp (dst_center_lat->prep_list[0]->info, "degrees") == 0)
    //{
    //    for (int i = 0; i < dst_grid_size; i ++)
    //    {
    //        d_dst_center_lat[i] = d_dst_center_lat[i] * deg2rad;
    //        d_dst_center_lon[i] = d_dst_center_lon[i] * deg2rad;
    //    }
    //}

    // load remap matrix
    int * ip_src_address = (int *) src_address->data;
	int * ip_dst_address = (int *) dst_address->data;
    double * dp_remap_matrix = (double *) remap_matrix->data;
    int * i_src_address = new int [num_links];
    int * i_dst_address = new int [num_links];
    _ValueType * d_w1       = new _ValueType [num_links];
    _ValueType * d_w2lat    = new _ValueType [num_links];
    _ValueType * d_w2lon    = new _ValueType [num_links];
    assert (i_src_address != (int *) 0);
    assert (i_dst_address != (int *) 0);
    assert (d_w1 != (_ValueType *) 0);
    assert (d_w2lat != (_ValueType *) 0);
    assert (d_w2lon != (_ValueType *) 0);
    int wid = 0;
    for (int i = 0; i < num_links; i ++)
    {
        i_src_address[i] = ip_src_address[i] - 1;
        i_dst_address[i] = ip_dst_address[i] - 1;
        d_w1[i] = (_ValueType) dp_remap_matrix[wid++];
        d_w2lat[i] = (_ValueType) dp_remap_matrix[wid++];
        d_w2lon[i] = (_ValueType) dp_remap_matrix[wid++];
        //printf("%d %d: %lf %lf %lf\n", i_src_address[i], i_dst_address[i],  d_w1[i], d_w2lat[i], d_w2lon[i]);
        std::cout << i_src_address[i] << "\t" << i_dst_address[i] << "\t" << d_w1[i] << "\t" << d_w2lat[i] << "\t" << d_w2lon[i] << std::endl;
    }
	
    grad_latlon * grad = new grad_latlon (src_lat_dim, src_lon_dim, d_src_center_lat, d_src_center_lon);
    delete [] d_src_center_lat;
    delete [] d_src_center_lon;
    grad->Calculate_grad_latlon_matrix();

#ifdef STL_SPARSEMATRIX_VERSION
    SpMat * m1      = new SpMat((int)dst_grid_size, (int)src_grid_size, i_dst_address, i_src_address, d_w1, num_links);
    SpMat * m2lat   = new SpMat((int)dst_grid_size, (int)src_grid_size, i_dst_address, i_src_address, d_w2lat, num_links);
    SpMat * m2lon   = new SpMat((int)dst_grid_size, (int)src_grid_size, i_dst_address, i_src_address, d_w2lon, num_links);
#else
    SpMat * m1      = new SpMat((int)dst_grid_size, (int)src_grid_size, num_links, i_dst_address, i_src_address, d_w1);
    SpMat * m2lat   = new SpMat((int)dst_grid_size, (int)src_grid_size, num_links, i_dst_address, i_src_address, d_w2lat);
    SpMat * m2lon   = new SpMat((int)dst_grid_size, (int)src_grid_size, num_links, i_dst_address, i_src_address, d_w2lon);
#endif
    delete [] d_w1;
    delete [] d_w2lat;
    delete [] d_w2lon;
    delete [] i_src_address;
    delete [] i_dst_address;
    //delete [] d_dst_center_lat;
    //delete [] d_dst_center_lon;
    // SpMat final should be <dst_grid_size x src_grid_size>
    SpMat * final = grad->Calculate_final_matrix (m1, m2lat, m2lon);
    //SpMat * final = new SpMat (1, 1);
#ifdef STL_SPARSEMATRIX_VERSION
    cdf->Modify_dim_data ("num_links", final->get_nnz ());
    cdf->Modify_dim_data ("num_wgts", 1);
	final->generate_rcvlist();
	_IDX * row_list = final->get_rlist();
	_IDX * col_list = final->get_clist();;
	_ValueType * val_list = final->get_vlist();
    int nnz_cdf = final->get_nnz();
    int * row_list_cdf = new int [nnz_cdf];
    int * col_list_cdf = new int [nnz_cdf];
    double * val_list_cdf = new double [nnz_cdf];
    for (int i = 0; i < nnz_cdf; i ++)
    {
        row_list_cdf[i] = row_list[i] + 1;
        col_list_cdf[i] = col_list[i] + 1;
        val_list_cdf[i] = (double) val_list[i];
        std::cout << val_list_cdf[i] << std::endl;
    }
    cdf->Modify_var_data ("src_address", col_list_cdf);
    cdf->Modify_var_data ("dst_address", row_list_cdf);
    cdf->Modify_var_data ("remap_matrix", val_list_cdf);
	final->clear_rcvlist();
    delete [] row_list_cdf;
    delete [] col_list_cdf;
    delete [] val_list_cdf;
    printf ("final remap matrix size: %d\n", final->get_nnz ());

#else
    cdf->Modify_dim_data ("num_links", final->Get_current_size ());
    cdf->Modify_dim_data ("num_wgts", 1);
    cdf->Modify_var_data ("src_address", col_list_cdf);
    cdf->Modify_var_data ("dst_address", row_list_cdf);
    cdf->Modify_var_data ("remap_matrix", val_list_cdf);
    printf ("final remap matrix size: %ld\n", final->Get_current_size ());
    //final->print_elems();
#endif
    //cdf->Print_dims ();
    //cdf->Print_vars ();
    //cdf->Print_global_preps ();
    cdf->Write_file (argv[3]);
	delete m1, m2lat, m2lon, final;
    delete cdf;

    return 0;
}
