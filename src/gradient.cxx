#include "gradient.h"
#include "types.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

grad_latlon::grad_latlon (INT _lat_dim, INT _lon_dim, double * _coord_lat, double * _coord_lon)
{
    lat_dim = _lat_dim;
    lon_dim = _lon_dim;
    coord_lat = new double [lat_dim * lon_dim];
    coord_lon = new double [lat_dim * lon_dim];
    memcpy (coord_lat, _coord_lat, lat_dim * lon_dim * sizeof (double));
    memcpy (coord_lon, _coord_lon, lat_dim * lon_dim * sizeof (double));
    memset (neighbor_list, 0, sizeof (INT) * MAX_NEIGHBOR);
    memset (neighbor_order, 0, sizeof (INT) * (MAX_NEIGHBOR + 1));
    memset (delta_lat, 0, sizeof (double) * MAX_NEIGHBOR);
    memset (delta_lon, 0, sizeof (double) * MAX_NEIGHBOR);
    partial_lat = new SparseMatrix (lat_dim * lon_dim, lat_dim * lon_dim);
    partial_lon = new SparseMatrix (lat_dim * lon_dim, lat_dim * lon_dim);
}

grad_latlon::~grad_latlon ()
{
    delete coord_lat;
    delete coord_lon;
    delete partial_lat;
    delete partial_lon;
}

void grad_latlon::Get_single_neighbors (INT id, INT & neighbor_size)
{
    assert (id >= 0 && id < lat_dim * lon_dim);
    INT lat_id = id / lon_dim;
    INT lon_id = id % lon_dim;
    INT neighbor_id = 0;

    // bottom; from left to right
    if (lat_id > 0)
    {
        neighbor_list[neighbor_id++] = (lat_id - 1) * lon_dim + (lon_id + lon_dim - 1) % lon_dim;
        neighbor_list[neighbor_id++] = (lat_id - 1) * lon_dim + lon_id;
        neighbor_list[neighbor_id++] = (lat_id - 1) * lon_dim + (lon_id + 1) % lon_dim;
    }

    // right
    neighbor_list[neighbor_id++] = lat_id * lon_dim + (lon_id + 1) % lon_dim;

    // up; from right to left
    if (lat_id < lat_dim - 1)
    {
        neighbor_list[neighbor_id++] = (lat_id + 1) * lon_dim + (lon_id + 1) % lon_dim;
        neighbor_list[neighbor_id++] = (lat_id + 1) * lon_dim + lon_id;
        neighbor_list[neighbor_id++] = (lat_id + 1) * lon_dim + (lon_id + lon_dim - 1) % lon_dim;
    }
    
    // left
    neighbor_list[neighbor_id++] = lat_id * lon_dim + (lon_id + lon_dim - 1) % lon_dim;

    neighbor_size = neighbor_id;
}

void grad_latlon::Get_single_neighbor_order_by_row (INT id, INT neighbor_size)
{
    // diaginal element is expressed by -1, others by neighbor id
    INT lat_id = id / lon_dim;
    INT lon_id = id % lon_dim;
    INT lon[3];
    INT current_id = 0;
    if (lon_id  == 0)
    {
        lon[0] = 1;
        lon[1] = 2;
        lon[2] = 0;
    }
    else if (lon_id == lon_dim - 1)
    {
        lon[0] = 2;
        lon[1] = 0;
        lon[2] = 1;
    }
    else
    {
        lon[0] = 0;
        lon[1] = 1;
        lon[2] = 2;
    }

    int left = 7;
    int right = 3;
    if (lat_id == 0)
    {
        right = 0;
        left = 4;
    }
    if (lat_id == lat_dim - 1)
    {
        right = 3;
        left = 4;
    }
    // bottom lat part
    if (lat_id > 0)
    {
        neighbor_order[current_id++] = neighbor_list[lon[0]];
        neighbor_order[current_id++] = neighbor_list[lon[1]];
        neighbor_order[current_id++] = neighbor_list[lon[2]];
    }
    // same lat part
    if (lon[0] == 0)
    {
        neighbor_order[current_id++] = neighbor_list[left];
        neighbor_order[current_id++] = -1;
        neighbor_order[current_id++] = neighbor_list[right];
    }
    else if (lon[0] == 1)
    {
        neighbor_order[current_id++] = -1;
        neighbor_order[current_id++] = neighbor_list[right];
        neighbor_order[current_id++] = neighbor_list[left];
    }
    else
    {
        neighbor_order[current_id++] = neighbor_list[right];
        neighbor_order[current_id++] = neighbor_list[left];
        neighbor_order[current_id++] = -1;
    }
    // up lat part
    if (lat_id < lat_dim - 1)
    {
        neighbor_order[current_id++] = neighbor_list[neighbor_size - 2 - lon[0]];
        neighbor_order[current_id++] = neighbor_list[neighbor_size - 2 - lon[1]];
        neighbor_order[current_id++] = neighbor_list[neighbor_size - 2 - lon[2]];
    }
}

void grad_latlon::Calculate_single_grad_latlon (INT id, INT neighbor_size)
{
    double sum_square_delta_lat = 0.0;
    double sum_square_delta_lon = 0.0;
    double sum_delta_lat_by_delta_lon = 0.0;
    assert (neighbor_size > 0 && neighbor_size < MAX_NEIGHBOR);
#ifdef OLD_VERSION
    for (int i = 0; i < neighbor_size; i ++)
    {
        delta_lat[i] = coord_lat[neighbor_list[i]] - coord_lat[id];
        delta_lon[i] = coord_lon[neighbor_list[i]] - coord_lon[id];
        if (delta_lon[i] < - CROSS_LONGITUDE)
            delta_lon[i] += FULL_LONGITUDE;
        else if (delta_lon[i] > CROSS_LONGITUDE)
            delta_lon[i] -= FULL_LONGITUDE;

        sum_square_delta_lat += delta_lat[i] * delta_lat[i];
        sum_square_delta_lon += delta_lon[i] * delta_lon[i];
        sum_delta_lat_by_delta_lon += delta_lat[i] * delta_lon[i];
    }
#endif
#ifdef NEW_VERSION
    INT neighbor_id = 0;
    INT neighbor_count = 0;
    for (int i = 0; i < neighbor_size + 1; i ++)
    {
        neighbor_id = neighbor_order[i];
        if (neighbor_id != -1)
        {
            delta_lat[neighbor_count] = coord_lat[neighbor_id] - coord_lat[id];
            delta_lon[neighbor_count] = coord_lon[neighbor_id] - coord_lon[id];
            if (delta_lon[neighbor_count] < - CROSS_LONGITUDE)
                delta_lon[neighbor_count] += FULL_LONGITUDE;
            else if (delta_lon[neighbor_count] > CROSS_LONGITUDE)
                delta_lon[neighbor_count] -= FULL_LONGITUDE;
            sum_square_delta_lat += delta_lat[neighbor_count] * delta_lat[neighbor_count];
            sum_square_delta_lon += delta_lon[neighbor_count] * delta_lon[neighbor_count];
            sum_delta_lat_by_delta_lon += delta_lat[neighbor_count] * delta_lon[neighbor_count];
            neighbor_count ++;
        }
    }
    assert (neighbor_count == neighbor_size);
#endif
    double determine = sum_square_delta_lat * sum_square_delta_lon
                     - sum_delta_lat_by_delta_lon * sum_delta_lat_by_delta_lon;
    assert (determine > 1e-30);
    double lat_alpha = sum_square_delta_lon / determine;
    double lat_beta = -sum_delta_lat_by_delta_lon / determine;
    double lon_alpha = -sum_delta_lat_by_delta_lon / determine;
    double lon_beta = sum_square_delta_lat / determine;

    double sum_delta_lat = 0.0;
    double sum_delta_lon = 0.0;
    double coslat = cos (coord_lat[id]);
    assert (coslat > 1e-30 || coslat < - 1e-30);
    //printf("costlat = %3.6f\n", coslat);
#ifdef NEW_VERSION
    // add entry by order
    
    for (INT i = 0; i < neighbor_size; i ++)
    {
        sum_delta_lat += delta_lat[i];
        sum_delta_lon += delta_lon[i];
    }
    neighbor_count = 0;
    for (INT i = 0; i < neighbor_size + 1; i ++)
    {
        neighbor_id = neighbor_order[i];
        if (neighbor_id != -1)
        {
            //printf ("add entry (%6d, %6d)\n", id, neighbor_id);
            partial_lat->Add_entry (id, neighbor_id, lat_alpha * delta_lat[neighbor_count] + lat_beta * delta_lon[neighbor_count]);
            partial_lon->Add_entry (id, neighbor_id, lon_alpha * delta_lat[neighbor_count] / coslat + lon_beta * delta_lon[neighbor_count] / coslat);
            neighbor_count ++;
        }
        else
        {
            //printf ("add entry (%6d, %6d)\n", id, id);
            partial_lat->Add_entry (id, id, - lat_alpha * sum_delta_lat - lat_beta * sum_delta_lon);
            partial_lon->Add_entry (id, id, - lon_alpha * sum_delta_lat / coslat - lon_beta * sum_delta_lon / coslat);
        }
    }
#endif
#ifdef OLD_VERSION
    for (INT i = 0; i < neighbor_size; i ++)
    {
        sum_delta_lat += delta_lat[i];
        sum_delta_lon += delta_lon[i];
        partial_lat->Add_entry (id, neighbor_list[i], lat_alpha * delta_lat[i] + lat_beta * delta_lon[i]);
        partial_lon->Add_entry (id, neighbor_list[i], lon_alpha * delta_lat[i] / coslat + lon_beta * delta_lon[i] / coslat);
    }
    partial_lat->Add_entry (id, id, - lat_alpha * sum_delta_lat - lat_beta * sum_delta_lon);
    partial_lon->Add_entry (id, id, - lon_alpha * sum_delta_lat / coslat - lon_beta * sum_delta_lon / coslat);
#endif
}

void grad_latlon::Calculate_grad_latlon_matrix ()
{
    INT max_id = lat_dim * lon_dim;
    INT neighbor_size = 0;
    for (INT i = 0; i < max_id; i ++)
    {
        Get_single_neighbors (i, neighbor_size);
        Get_single_neighbor_order_by_row (i, neighbor_size);
        Calculate_single_grad_latlon (i, neighbor_size);
    }
    partial_lat->Complete_row_ptr ();
    partial_lon->Complete_row_ptr ();
}

SparseMatrix * grad_latlon::Calculate_final_matrix (SparseMatrix * m1, SparseMatrix * m2lat, SparseMatrix * m2lon)
{
    printf ("sizeof(partial_lat): %d\n", partial_lat->Get_current_size ());
    printf ("sizeof(partial_lon): %d\n", partial_lon->Get_current_size ());
    SparseMatrix * tmpfinal1 = m2lat->Matrix_multiple (partial_lat);
    SparseMatrix * tmpfinal2 = m2lon->Matrix_multiple (partial_lon);
    SparseMatrix * tmpfinal = tmpfinal1->Matrix_add (tmpfinal2);
    SparseMatrix * final = tmpfinal->Matrix_add (m1);
    delete tmpfinal1;
    delete tmpfinal2;
    delete tmpfinal;
    return final;
}

SparseMatrix * grad_latlon::Get_grad_lat_matrix ()
{
    return partial_lat;
}

SparseMatrix * grad_latlon::Get_grad_lon_matrix ()
{
    return partial_lon;
}

void grad_latlon::Test_grad_latlon (double (* function)(double, double), double (* partial_lat_function)(double, double), double (* partial_lon_function)(double, double))
{
    printf ("Test_grad_latlon\n");
    // TODO
    INT size = lat_dim * lon_dim;
    double * analytic_function = new double [size];
    double * analytic_partial_lat = new double [size];
    double * analytic_partial_lon = new double [size];
    double * discrete_partial_lat = new double [size];
    double * discrete_partial_lon = new double [size];
    assert (analytic_function != (double *) 0);
    assert (analytic_partial_lat != (double *) 0);
    assert (analytic_partial_lon != (double *) 0);
    assert (discrete_partial_lat != (double *) 0);
    assert (discrete_partial_lon != (double *) 0);
    
    for (INT i = 0; i < size; i ++)
    {
        analytic_function[i] = function (coord_lat[i], coord_lon[i]);
        analytic_partial_lat[i] = partial_lat_function (coord_lat[i], coord_lon[i]);
        analytic_partial_lon[i] = partial_lon_function (coord_lat[i], coord_lon[i]);
    }

    // use grad matrix to calculate discrete grad
    partial_lat->Matrix_vector_multiple (discrete_partial_lat, size, analytic_function, size);
    partial_lon->Matrix_vector_multiple (discrete_partial_lon, size, analytic_function, size);

    printf ("test partial lat\n");
    for (INT i = 0; i < size; i ++)
        printf ("%5.5f\t\t\t%5.5f\n", analytic_partial_lat[i], discrete_partial_lat[i]);
    printf ("test partial lon\n");
    for (INT i = 0; i < size; i ++)
        printf ("%5.5f\t\t\t%5.5f\n", analytic_partial_lat[i], discrete_partial_lat[i]);
    delete [] analytic_function;
    delete [] analytic_partial_lat;
    delete [] analytic_partial_lon;
    delete [] discrete_partial_lat;
    delete [] discrete_partial_lon;
}
// note: temp use; we will export final matrix to files
void grad_latlon::Test_final_results (SparseMatrix * m1, SparseMatrix * m2lat, SparseMatrix * m2lon, double (* function)(double, double), double (* partial_lat_function)(double, double), double (* partial_lon_function)(double, double), double * dst_lat, double * dst_lon, int * dst_mask)
{
    //printf ("Test_final_results\n");
    printf ("Analytic\t\tOrder1\t\tOrder2_analytic\t\tOrder2_discrete\n");
    INT size = lat_dim * lon_dim;
    INT src_dim = m1->Get_col_dim ();
    INT dst_dim = m1->Get_row_dim ();
    assert (size == src_dim);
    //partial_lat->print ();
    //partial_lon->print ();
    //m1->print ();
    //m2lat->print ();
    //m2lon->print ();
    double * analytic_function = new double [src_dim];
    double * analytic_partial_lat = new double [src_dim];
    double * analytic_partial_lon = new double [src_dim];
    double * discrete_partial_lat = new double [src_dim];
    double * discrete_partial_lon = new double [src_dim];
    double * results_analytic = new double [dst_dim];
    double * results_order1 = new double [dst_dim];
    double * results_order1_rerrors = new double [dst_dim];
    double * results_order2_analytic = new double [dst_dim];
    double * results_order2_analytic_rerrors = new double [dst_dim];
    double * results_order2_analytic_lat = new double [dst_dim];
    double * results_order2_analytic_lon = new double [dst_dim];
    double * results_order2_discrete = new double [dst_dim];
    double * results_order2_discrete_rerrors = new double [dst_dim];
    double * results_order2_discrete_lat = new double [dst_dim];
    double * results_order2_discrete_lon = new double [dst_dim];

    assert (analytic_function != (double *) 0);
    assert (analytic_partial_lat != (double *) 0);
    assert (analytic_partial_lon != (double *) 0);
    assert (discrete_partial_lat != (double *) 0);
    assert (discrete_partial_lon != (double *) 0);
    assert (results_analytic != (double *) 0);
    assert (results_order1 != (double *) 0);
    assert (results_order1_rerrors != (double *) 0);
    assert (results_order2_analytic != (double *) 0);
    assert (results_order2_analytic_rerrors != (double *) 0);
    assert (results_order2_analytic_lat != (double *) 0);
    assert (results_order2_analytic_lon != (double *) 0);
    assert (results_order2_discrete != (double *) 0);
    assert (results_order2_discrete_rerrors != (double *) 0);
    assert (results_order2_discrete_lat != (double *) 0);
    assert (results_order2_discrete_lon != (double *) 0);
    
    for (INT i = 0; i < src_dim; i ++)
    {
        analytic_function[i] = function (coord_lat[i], coord_lon[i]);
        analytic_partial_lat[i] = partial_lat_function (coord_lat[i], coord_lon[i]);
        analytic_partial_lon[i] = partial_lon_function (coord_lat[i], coord_lon[i]);
    }

    for (INT i = 0; i < dst_dim; i ++)
    {
        results_analytic[i] = function (dst_lat[i], dst_lon[i]);
    }
    // use grad matrix to calculate discrete grad
    partial_lat->Matrix_vector_multiple (discrete_partial_lat, src_dim, analytic_function, src_dim);
    partial_lon->Matrix_vector_multiple (discrete_partial_lon, src_dim, analytic_function, src_dim);

    // use remap matrix to calculate field value
    m1->Matrix_vector_multiple (results_order1, dst_dim, analytic_function, src_dim);
    m2lat->Matrix_vector_multiple (results_order2_analytic_lat, dst_dim, analytic_partial_lat, src_dim);
    m2lon->Matrix_vector_multiple (results_order2_analytic_lon, dst_dim, analytic_partial_lon, src_dim);
    m2lat->Matrix_vector_multiple (results_order2_discrete_lat, dst_dim, discrete_partial_lat, src_dim);
    m2lon->Matrix_vector_multiple (results_order2_discrete_lon, dst_dim, discrete_partial_lon, src_dim);

    for (INT i = 0; i < dst_dim; i ++)
    {
        if (dst_mask[i] == 0)
        {
            results_analytic[i] = 0.0;
            results_order1[i] = 0.0;
            results_order2_analytic_lat[i] = 0.0;
            results_order2_analytic_lon[i] = 0.0;
            results_order2_discrete_lat[i] = 0.0;
            results_order2_discrete_lon[i] = 0.0;
        }
    }
    double rmsd_order1 = 0.0;               // root mean square deviation
    double rmsd_order2_analytic = 0.0;
    double rmsd_order2_discrete = 0.0;
    for (INT i = 0; i < dst_dim; i ++)
    {
        results_order2_analytic[i] = results_order1[i] + results_order2_analytic_lat[i] + results_order2_analytic_lon[i];
        results_order2_discrete[i] = results_order1[i] + results_order2_discrete_lat[i] + results_order2_discrete_lon[i];
        results_order1_rerrors[i] = results_order1[i] - results_analytic[i];
        results_order2_analytic_rerrors[i] = results_order2_analytic[i] - results_analytic[i];
        results_order2_discrete_rerrors[i] = results_order2_discrete[i] - results_analytic[i];
        if (results_analytic[i] > 1e-30 || results_analytic[i] < - 1e-30)
        {
            results_order1_rerrors[i] /= results_analytic[i];
            results_order2_analytic_rerrors[i] /= results_analytic[i];
            results_order2_discrete_rerrors[i] /= results_analytic[i];
        }
        else
        {
            if (results_order1_rerrors[i] > 1e-30 || results_order1_rerrors[i] < - 1e-30)
                results_order1_rerrors[i] = 1.0;
            else
                results_order1_rerrors[i] = 0.0;

            if (results_order2_analytic_rerrors[i] > 1e-30 || results_order2_analytic_rerrors[i] < - 1e-30)
                results_order2_analytic_rerrors[i] = 1.0;
            else
                results_order2_analytic_rerrors[i] = 0.0;

            if (results_order2_discrete_rerrors[i] > 1e-30 || results_order2_discrete_rerrors[i] < - 1e-30)
                results_order2_discrete_rerrors[i] = 1.0;
            else
                results_order2_discrete_rerrors[i] = 0.0;
        }
        rmsd_order1 += results_order1_rerrors[i] * results_order1_rerrors[i];
        rmsd_order2_analytic += results_order2_analytic_rerrors[i] * results_order2_analytic_rerrors[i];
        rmsd_order2_discrete += results_order2_discrete_rerrors[i] * results_order2_discrete_rerrors[i];

        printf ("%5.8f\t\t\t%5.8f\t\t\t%5.8f\t\t\t%5.8f\t\t%5.8f\t\t%5.8f\t\t%5.8f\n", results_analytic[i], results_order1[i], results_order2_analytic[i], results_order2_discrete[i], results_order1_rerrors[i], results_order2_analytic_rerrors[i], results_order2_discrete_rerrors[i]);
    }
    rmsd_order1 = sqrt (rmsd_order1 / dst_dim);
    rmsd_order2_analytic = sqrt (rmsd_order2_analytic / dst_dim);
    rmsd_order2_discrete = sqrt (rmsd_order2_discrete / dst_dim);
    printf ("rmsd_order1: %f\n", rmsd_order1);
    printf ("rmsd_order2_analytic: %f\n", rmsd_order2_analytic);
    printf ("rmsd_order2_discrete: %f\n", rmsd_order2_discrete);
    //return;
    delete [] analytic_function;
    delete [] analytic_partial_lat;
    delete [] analytic_partial_lon;
    delete [] discrete_partial_lat;
    delete [] discrete_partial_lon;
    delete [] results_order1;
    delete [] results_order1_rerrors;
    delete [] results_order2_analytic;
    delete [] results_order2_analytic_rerrors;
    delete [] results_order2_analytic_lat;
    delete [] results_order2_analytic_lon;
    delete [] results_order2_discrete;
    delete [] results_order2_discrete_rerrors;
    delete [] results_order2_discrete_lat;
    delete [] results_order2_discrete_lon;
}

// note: temp use; we will export final matrix to files
void grad_latlon::Test_final_results (SparseMatrix * m1, SparseMatrix * m2lat, SparseMatrix * m2lon, double (* function)(double, double, double, double), double (* partial_lat_function)(double, double), double (* partial_lon_function)(double, double), double * dst_lat, double * dst_lon, int * dst_mask, CDF_INT dst_nlat, CDF_INT dst_nlon)
{
    //printf ("Test_final_results\n");
    printf ("Analytic\t\tOrder1\t\tOrder2_analytic\t\tOrder2_discrete\n");
    INT size = lat_dim * lon_dim;
    INT src_dim = m1->Get_col_dim ();
    INT dst_dim = m1->Get_row_dim ();
    assert (size == src_dim);
    //partial_lat->print ();
    //partial_lon->print ();
    //m1->print ();
    //m2lat->print ();
    //m2lon->print ();
    double * analytic_function = new double [src_dim];
    double * analytic_partial_lat = new double [src_dim];
    double * analytic_partial_lon = new double [src_dim];
    double * discrete_partial_lat = new double [src_dim];
    double * discrete_partial_lon = new double [src_dim];
    double * results_analytic = new double [dst_dim];
    double * results_order1 = new double [dst_dim];
    double * results_order1_rerrors = new double [dst_dim];
    double * results_order2_analytic = new double [dst_dim];
    double * results_order2_analytic_rerrors = new double [dst_dim];
    double * results_order2_analytic_lat = new double [dst_dim];
    double * results_order2_analytic_lon = new double [dst_dim];
    double * results_order2_discrete = new double [dst_dim];
    double * results_order2_discrete_rerrors = new double [dst_dim];
    double * results_order2_discrete_lat = new double [dst_dim];
    double * results_order2_discrete_lon = new double [dst_dim];

    assert (analytic_function != (double *) 0);
    assert (analytic_partial_lat != (double *) 0);
    assert (analytic_partial_lon != (double *) 0);
    assert (discrete_partial_lat != (double *) 0);
    assert (discrete_partial_lon != (double *) 0);
    assert (results_analytic != (double *) 0);
    assert (results_order1 != (double *) 0);
    assert (results_order1_rerrors != (double *) 0);
    assert (results_order2_analytic != (double *) 0);
    assert (results_order2_analytic_rerrors != (double *) 0);
    assert (results_order2_analytic_lat != (double *) 0);
    assert (results_order2_analytic_lon != (double *) 0);
    assert (results_order2_discrete != (double *) 0);
    assert (results_order2_discrete_rerrors != (double *) 0);
    assert (results_order2_discrete_lat != (double *) 0);
    assert (results_order2_discrete_lon != (double *) 0);
    
    double dlat = FULL_LONGITUDE / 2 / lat_dim / 2;
    double dlon = FULL_LONGITUDE / lon_dim / 2;
    printf ("dlat: %f\n", dlat);
    printf ("dlon: %f\n", dlon);
    for (INT i = 0; i < src_dim; i ++)
    {
        analytic_function[i] = function (coord_lat[i], coord_lon[i], dlat, dlon);
        analytic_partial_lat[i] = partial_lat_function (coord_lat[i], coord_lon[i]);
        analytic_partial_lon[i] = partial_lon_function (coord_lat[i], coord_lon[i]);
    }

    dlat = FULL_LONGITUDE / 2 / dst_nlat / 2;
    dlon = FULL_LONGITUDE / dst_nlon / 2;
    printf ("dlat: %f\n", dlat);
    printf ("dlon: %f\n", dlon);
    for (INT i = 0; i < dst_dim; i ++)
    {
        results_analytic[i] = function (dst_lat[i], dst_lon[i], dlat, dlon);
    }
    // use grad matrix to calculate discrete grad
    partial_lat->Matrix_vector_multiple (discrete_partial_lat, src_dim, analytic_function, src_dim);
    partial_lon->Matrix_vector_multiple (discrete_partial_lon, src_dim, analytic_function, src_dim);

    // use remap matrix to calculate field value
    m1->Matrix_vector_multiple (results_order1, dst_dim, analytic_function, src_dim);
    m2lat->Matrix_vector_multiple (results_order2_analytic_lat, dst_dim, analytic_partial_lat, src_dim);
    m2lon->Matrix_vector_multiple (results_order2_analytic_lon, dst_dim, analytic_partial_lon, src_dim);
    m2lat->Matrix_vector_multiple (results_order2_discrete_lat, dst_dim, discrete_partial_lat, src_dim);
    m2lon->Matrix_vector_multiple (results_order2_discrete_lon, dst_dim, discrete_partial_lon, src_dim);

    for (INT i = 0; i < dst_dim; i ++)
    {
        if (dst_mask[i] == 0)
        {
            results_analytic[i] = 0.0;
            results_order1[i] = 0.0;
            results_order2_analytic_lat[i] = 0.0;
            results_order2_analytic_lon[i] = 0.0;
            results_order2_discrete_lat[i] = 0.0;
            results_order2_discrete_lon[i] = 0.0;
        }
    }
    double rmsd_order1 = 0.0;               // root mean square deviation
    double rmsd_order2_analytic = 0.0;
    double rmsd_order2_discrete = 0.0;
    for (INT i = 0; i < dst_dim; i ++)
    {
        results_order2_analytic[i] = results_order1[i] + results_order2_analytic_lat[i] + results_order2_analytic_lon[i];
        results_order2_discrete[i] = results_order1[i] + results_order2_discrete_lat[i] + results_order2_discrete_lon[i];
        results_order1_rerrors[i] = results_order1[i] - results_analytic[i];
        results_order2_analytic_rerrors[i] = results_order2_analytic[i] - results_analytic[i];
        results_order2_discrete_rerrors[i] = results_order2_discrete[i] - results_analytic[i];
        if (results_analytic[i] > 1e-30)
        {
            results_order1_rerrors[i] /= results_analytic[i];
            results_order2_analytic_rerrors[i] /= results_analytic[i];
            results_order2_discrete_rerrors[i] /= results_analytic[i];
        }
        else
        {
            if (results_order1_rerrors[i] > 1e-30 || results_order1_rerrors[i] < - 1e-30)
                results_order1_rerrors[i] = 1.0;
            else
                results_order1_rerrors[i] = 0.0;

            if (results_order2_analytic_rerrors[i] > 1e-30 || results_order2_analytic_rerrors[i] < - 1e-30)
                results_order2_analytic_rerrors[i] = 1.0;
            else
                results_order2_analytic_rerrors[i] = 0.0;

            if (results_order2_discrete_rerrors[i] > 1e-30 || results_order2_discrete_rerrors[i] < - 1e-30)
                results_order2_discrete_rerrors[i] = 1.0;
            else
                results_order2_discrete_rerrors[i] = 0.0;
        }
        rmsd_order1 += results_order1_rerrors[i] * results_order1_rerrors[i];
        rmsd_order2_analytic += results_order2_analytic_rerrors[i] * results_order2_analytic_rerrors[i];
        rmsd_order2_discrete += results_order2_discrete_rerrors[i] * results_order2_discrete_rerrors[i];

        printf ("%5.8f\t\t\t%5.8f\t\t\t%5.8f\t\t\t%5.8f\t\t%5.8f\t\t%5.8f\t\t%5.8f\n", results_analytic[i], results_order1[i], results_order2_analytic[i], results_order2_discrete[i], results_order1_rerrors[i], results_order2_analytic_rerrors[i], results_order2_discrete_rerrors[i]);
    }
    rmsd_order1 = sqrt (rmsd_order1 / dst_dim);
    rmsd_order2_analytic = sqrt (rmsd_order2_analytic / dst_dim);
    rmsd_order2_discrete = sqrt (rmsd_order2_discrete / dst_dim);
    printf ("rmsd_order1: %f\n", rmsd_order1);
    printf ("rmsd_order2_analytic: %f\n", rmsd_order2_analytic);
    printf ("rmsd_order2_discrete: %f\n", rmsd_order2_discrete);
    //return;
    delete [] analytic_function;
    delete [] analytic_partial_lat;
    delete [] analytic_partial_lon;
    delete [] discrete_partial_lat;
    delete [] discrete_partial_lon;
    delete [] results_order1;
    delete [] results_order1_rerrors;
    delete [] results_order2_analytic;
    delete [] results_order2_analytic_rerrors;
    delete [] results_order2_analytic_lat;
    delete [] results_order2_analytic_lon;
    delete [] results_order2_discrete;
    delete [] results_order2_discrete_rerrors;
    delete [] results_order2_discrete_lat;
    delete [] results_order2_discrete_lon;
}

