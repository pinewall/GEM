#include "IO_netCDF.h"
#include "gradient.h"
#include "types.h"
#include "field.h"
#include "assert.h"

int main(int argc, char ** argv)
{
    if (argc < 4)
    {
        printf ("Usage: ./unit_test remap_ncfile_path xml_path test_function_name\n\tunit\n\tcoslat_coslon\n\tcosbell\n\tY2_2\n\tY16_32\n");
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

    //printf ("%s\n", argv[2]);

    IO_netCDF * cdf = new IO_netCDF (argv[2]);
    //printf("after configure\n");
    //cdf->Print_dims();
    //cdf->Print_vars();
    //cdf->Print_global_preps ();
    //IO_netCDF::Check_NC_Error (NC_NOERR);
    //cdf->Read_file ("test.nc");
    cdf->Read_file (argv[1]);
    //cdf->Write_file ("output.nc");

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
    Var dst_imasks     = cdf->Get_var_by_gname ("dst_grid_imask");
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
    double * dp_dst_center_lat = (double *) dst_center_lat->data;
    double * dp_dst_center_lon = (double *) dst_center_lon->data;
    int    * ip_dst_imasks     = (int *) dst_imasks->data;
    
    // load src|dst _center_ lat|lon
    _ValueType * d_src_center_lat = new _ValueType [src_grid_size];
    _ValueType * d_src_center_lon = new _ValueType [src_grid_size];
    _ValueType * d_dst_center_lat = new _ValueType [dst_grid_size];
    _ValueType * d_dst_center_lon = new _ValueType [dst_grid_size];
    int * i_dst_imasks = new int [dst_grid_size];
    assert (d_src_center_lat != (_ValueType *) 0);
    assert (d_src_center_lon != (_ValueType *) 0);
    assert (d_dst_center_lat != (_ValueType *) 0);
    assert (d_dst_center_lon != (_ValueType *) 0);
    assert (i_dst_imasks != (int *) 0);
    for (int i = 0; i < src_grid_size; i ++)
    {
        d_src_center_lat[i] = (_ValueType) dp_src_center_lat[i];
        d_src_center_lon[i] = (_ValueType) dp_src_center_lon[i];
    }
    for (int i = 0; i < dst_grid_size; i ++)
    {
        d_dst_center_lat[i] = (_ValueType) dp_dst_center_lat[i];
        d_dst_center_lon[i] = (_ValueType) dp_dst_center_lon[i];
        i_dst_imasks[i]     = (int)        ip_dst_imasks[i];
    }
	if (strcmp (src_center_lat->prep_list[0]->info, "degrees") == 0)
	{
		for (int i = 0; i < src_grid_size; i ++)
		{
            d_src_center_lat[i] = d_src_center_lat[i] * deg2rad;
            d_src_center_lon[i] = d_src_center_lon[i] * deg2rad;
		}
	}
    if (strcmp (dst_center_lat->prep_list[0]->info, "degrees") == 0)
    {
        for (int i = 0; i < dst_grid_size; i ++)
        {
            d_dst_center_lat[i] = d_dst_center_lat[i] * deg2rad;
            d_dst_center_lon[i] = d_dst_center_lon[i] * deg2rad;
        }
    }

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
        //std::cout << i_src_address[i] << "\t" << i_dst_address[i] << "\t" << d_w1[i] << "\t" << d_w2lat[i] << "\t" << d_w2lon[i] << std::endl;
    }

    grad_latlon * grad = new grad_latlon ((int)src_lat_dim, (int)src_lon_dim, d_src_center_lat, d_src_center_lon);
    delete [] d_src_center_lat;
    delete [] d_src_center_lon;
    grad->Calculate_grad_latlon_matrix();

    // test gradient method
    //grad->Test_grad_latlon (&function_coslat_coslon, &partial_lat_function_coslat_coslon, &partial_lon_function_coslat_coslon);

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
    int dst_nlon = ((int *) cdf->Get_var_by_gname ("dst_grid_dims")->data)[0];
    int dst_nlat = ((int *) cdf->Get_var_by_gname ("dst_grid_dims")->data)[1];

    if (strcmp (argv[3], "unit") == 0)
        grad->Test_final_results (m1, m2lat, m2lon, &function_unit, &partial_lat_function_unit, &partial_lon_function_unit, d_dst_center_lat, d_dst_center_lon, i_dst_imasks);
    else if (strcmp (argv[3], "coslat_coslon") == 0)
        grad->Test_final_results (m1, m2lat, m2lon, &function_coslat_coslon, &partial_lat_function_coslat_coslon, &partial_lon_function_coslat_coslon, d_dst_center_lat, d_dst_center_lon, i_dst_imasks);
    else if (strcmp (argv[3], "cosbell") == 0)
        grad->Test_final_results (m1, m2lat, m2lon, &function_cosbell, &partial_lat_function_cosbell, &partial_lon_function_cosbell, d_dst_center_lat, d_dst_center_lon, i_dst_imasks);
    else if (strcmp (argv[3], "Y2_2") == 0)
    {
        grad->Test_grad_latlon (function_spherical_harmonic_2_2, partial_lat_function_spherical_harmonic_2_2, partial_lon_function_spherical_harmonic_2_2);
        grad->Test_final_results (m1, m2lat, m2lon, &function_spherical_harmonic_2_2, &partial_lat_function_spherical_harmonic_2_2, &partial_lon_function_spherical_harmonic_2_2, d_dst_center_lat, d_dst_center_lon, i_dst_imasks);
    }
    else if (strcmp (argv[3], "aaY2_2") == 0)
        grad->Test_final_results (m1, m2lat, m2lon, &function_aa_spherical_harmonic_2_2, &partial_lat_function_spherical_harmonic_2_2, &partial_lon_function_spherical_harmonic_2_2, d_dst_center_lat, d_dst_center_lon, i_dst_imasks, dst_nlat, dst_nlon);
    else if (strcmp (argv[3], "Y16_32") == 0)
        grad->Test_final_results (m1, m2lat, m2lon, &function_spherical_harmonic_16_32, &partial_lat_function_spherical_harmonic_16_32, &partial_lon_function_spherical_harmonic_16_32, d_dst_center_lat, d_dst_center_lon, i_dst_imasks);
    else if (strcmp (argv[3], "realdata") == 0)
    {
        if (argc < 8)
        {
            printf ("readdata usage: ./unit_test remap.nc remap.xml realdata src-ncfile src-xmlfile dst-ncfile dst-xmlfile\n");
            return -2;
        }
        else
        {
            size_t src_grid_size, dst_grid_size;
            _ValueType * src_field_data, * dst_field_data;
            IO_netCDF * cdf_src_field = new IO_netCDF (argv[5]);
            cdf_src_field->Read_file (argv[4]);
            src_grid_size = cdf_src_field->Get_dim_by_gname ("grid_size")->data;
            src_field_data = (_ValueType *) cdf_src_field->Get_var_by_gname ("physical_variable")->data;
            
            IO_netCDF * cdf_dst_field = new IO_netCDF (argv[7]);
            cdf_dst_field->Read_file (argv[6]);
            dst_grid_size = cdf_dst_field->Get_dim_by_gname ("grid_size")->data;
            dst_field_data = (_ValueType *) cdf_dst_field->Get_var_by_gname ("physical_variable")->data;
            
            grad->Test_final_results (m1, m2lat, m2lon, src_field_data, src_grid_size, dst_field_data, dst_grid_size, d_dst_center_lat, d_dst_center_lon, i_dst_imasks);
            delete cdf_src_field;
            delete cdf_dst_field;
        }
    }
    else
        printf ("Un-supported test function!\nUse the following ones:\n\tunit\n\tcoslat_coslon\n\tcosbell\n\tY2_2\n\tY16_32\n\trealdata file1 file2\n");
    delete [] d_dst_center_lat;
    delete [] d_dst_center_lon;
    delete [] i_dst_imasks;
    return 0;
}
