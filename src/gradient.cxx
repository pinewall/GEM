#include "gradient.h"
#include "types.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

grad_latlon::grad_latlon (int _lat_dim, int _lon_dim, _ValueType * _coord_lat, _ValueType * _coord_lon)
{
	//Set_ID_LIMIT();
    lat_dim = _lat_dim;
    lon_dim = _lon_dim;
	ID_self = -1;
    coord_lat = new _ValueType [lat_dim * lon_dim];
    coord_lon = new _ValueType [lat_dim * lon_dim];
    memcpy (coord_lat, _coord_lat, lat_dim * lon_dim * sizeof (_ValueType));
    memcpy (coord_lon, _coord_lon, lat_dim * lon_dim * sizeof (_ValueType));
    memset (neighbor_list, 0, sizeof (int) * MAX_NEIGHBOR);
    memset (neighbor_order, 0, sizeof (int) * (MAX_NEIGHBOR + 1));
    memset (delta_lat, 0, sizeof (_ValueType) * MAX_NEIGHBOR);
    memset (delta_lon, 0, sizeof (_ValueType) * MAX_NEIGHBOR);
    partial_lat = new SpMat (lat_dim * lon_dim, lat_dim * lon_dim);
    partial_lon = new SpMat (lat_dim * lon_dim, lat_dim * lon_dim);
}

grad_latlon::~grad_latlon ()
{
    delete [] coord_lat;
    delete [] coord_lon;
    delete [] partial_lat;
    delete [] partial_lon;
}

/*
int grad_latlon::ID_self
{
	return grad_latlon::ID_LIMIT;
}

void grad_latlon::Set_ID_LIMIT()
{
	grad_latlon::ID_LIMIT = std::numeric_limits<int>::max();
}
*/
void grad_latlon::Get_single_neighbors (int id, int & neighbor_size)
{
    assert (id < lat_dim * lon_dim);
    int lat_id = id / lon_dim;
    int lon_id = id % lon_dim;
    int neighbor_id = 0;

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

void grad_latlon::Get_single_neighbor_order_by_row (int id, int neighbor_size)
{
    // diaginal element is expressed by -1, others by neighbor id
    int lat_id = id / lon_dim;
    int lon_id = id % lon_dim;
    int lon[3];
    int current_id = 0;
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
        neighbor_order[current_id++] = ID_self;
        neighbor_order[current_id++] = neighbor_list[right];
    }
    else if (lon[0] == 1)
    {
        neighbor_order[current_id++] = ID_self;
        neighbor_order[current_id++] = neighbor_list[right];
        neighbor_order[current_id++] = neighbor_list[left];
    }
    else
    {
        neighbor_order[current_id++] = neighbor_list[right];
        neighbor_order[current_id++] = neighbor_list[left];
        neighbor_order[current_id++] = ID_self;
    }
    // up lat part
    if (lat_id < lat_dim - 1)
    {
        neighbor_order[current_id++] = neighbor_list[neighbor_size - 2 - lon[0]];
        neighbor_order[current_id++] = neighbor_list[neighbor_size - 2 - lon[1]];
        neighbor_order[current_id++] = neighbor_list[neighbor_size - 2 - lon[2]];
    }
}

void grad_latlon::Calculate_single_grad_latlon (int id, int neighbor_size)
{
    _ValueType sum_square_delta_lat = 0.0;
    _ValueType sum_square_delta_lon = 0.0;
    _ValueType sum_delta_lat_by_delta_lon = 0.0;
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
    int neighbor_id = 0;
    int neighbor_count = 0;
    for (int i = 0; i < neighbor_size + 1; i ++)
    {
        neighbor_id = neighbor_order[i];
        if (neighbor_id != ID_self)
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
    _ValueType determine = sum_square_delta_lat * sum_square_delta_lon
                     - sum_delta_lat_by_delta_lon * sum_delta_lat_by_delta_lon;
    assert (determine > 1e-50);
    _ValueType lat_alpha = sum_square_delta_lon / determine;
    _ValueType lat_beta = -sum_delta_lat_by_delta_lon / determine;
    _ValueType lon_alpha = -sum_delta_lat_by_delta_lon / determine;
    _ValueType lon_beta = sum_square_delta_lat / determine;

    _ValueType sum_delta_lat = 0.0;
    _ValueType sum_delta_lon = 0.0;
    _ValueType coslat = cos (coord_lat[id]);
    assert (coslat > 1e-50 || coslat < - 1e-50);
    //printf("costlat = %3.6f\n", coslat);
#ifdef NEW_VERSION
    // add entry by order
    
    for (int i = 0; i < neighbor_size; i ++)
    {
        sum_delta_lat += delta_lat[i];
        sum_delta_lon += delta_lon[i];
    }
    neighbor_count = 0;
    for (int i = 0; i < neighbor_size + 1; i ++)
    {
        neighbor_id = neighbor_order[i];
        if (neighbor_id != ID_self)
        {
            //printf ("add entry (%6d, %6d)\n", id, neighbor_id);
#ifdef STL_SPARSEMATRIX_VERSION
			partial_lat->add_elem_no_update (id, neighbor_id, lat_alpha *delta_lat[neighbor_count] + lat_beta * delta_lon[neighbor_count]);
			partial_lon->add_elem_no_update (id, neighbor_id, lon_alpha *delta_lat[neighbor_count] / coslat + lon_beta * delta_lon[neighbor_count] / coslat);
#else
            partial_lat->Add_entry (id, neighbor_id, lat_alpha * delta_lat[neighbor_count] + lat_beta * delta_lon[neighbor_count]);
            partial_lon->Add_entry (id, neighbor_id, lon_alpha * delta_lat[neighbor_count] / coslat + lon_beta * delta_lon[neighbor_count] / coslat);
#endif
            neighbor_count ++;
        }
        else
        {
            //printf ("add entry (%6d, %6d)\n", id, id);
#ifdef STL_SPARSEMATRIX_VERSION
			partial_lat->add_elem_no_update (id, id, - lat_alpha * sum_delta_lat - lat_beta * sum_delta_lon);
			partial_lon->add_elem_no_update (id, id, - lon_alpha * sum_delta_lat / coslat -lon_beta * sum_delta_lon / coslat);
#else
            partial_lat->Add_entry (id, id, - lat_alpha * sum_delta_lat - lat_beta * sum_delta_lon);
            partial_lon->Add_entry (id, id, - lon_alpha * sum_delta_lat / coslat - lon_beta * sum_delta_lon / coslat);
#endif
        }
    }
#endif
#ifdef OLD_VERSION
    for (int i = 0; i < neighbor_size; i ++)
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
    int max_id = lat_dim * lon_dim;
    int neighbor_size = 0;
    for (int i = 0; i < max_id; i ++)
    {
        Get_single_neighbors (i, neighbor_size);
        Get_single_neighbor_order_by_row (i, neighbor_size);
        Calculate_single_grad_latlon (i, neighbor_size);
    }
#ifdef STL_SPARSEMATRIX_VERSION
	partial_lat->update_rptr();
	partial_lon->update_rptr();
#else
    partial_lat->Complete_row_ptr ();
    partial_lon->Complete_row_ptr ();
#endif
}

SpMat * grad_latlon::Calculate_final_matrix (SpMat * m1, SpMat * m2lat, SpMat * m2lon)
{
#ifdef STL_SPARSEMATRIX_VERSION
    printf ("sizeof(partial_lat): %d\n", partial_lat->get_nnz ());
    printf ("sizeof(partial_lon): %d\n", partial_lon->get_nnz ());
	SpMat * tmpfinal1 = SpMat::mat_mul(m2lat, partial_lat);
	SpMat * tmpfinal2 = SpMat::mat_mul(m2lon, partial_lon);
	SpMat * tmpfinal = SpMat::mat_add(tmpfinal1,tmpfinal2);
	SpMat * final = SpMat::mat_add(m1, tmpfinal);
    printf ("sizeof(tmpfinal1): %d\n", tmpfinal1->get_nnz ());
    printf ("sizeof(tmpfinal2): %d\n", tmpfinal2->get_nnz ());
    printf ("sizeof(tmpfinal): %d\n", tmpfinal->get_nnz ());
    printf ("sizeof(final): %d\n", final->get_nnz ());
#else
    printf ("sizeof(partial_lat): %ld\n", partial_lat->Get_current_size ());
    printf ("sizeof(partial_lon): %ld\n", partial_lon->Get_current_size ());
    SpMat * tmpfinal1 = m2lat->Matrix_multiple (partial_lat);
    SpMat * tmpfinal2 = m2lon->Matrix_multiple (partial_lon);
    SpMat * tmpfinal = tmpfinal1->Matrix_add (tmpfinal2);
    SpMat * final = tmpfinal->Matrix_add (m1);
#endif
    delete tmpfinal1;
    delete tmpfinal2;
    delete tmpfinal;
    return final;
}

SpMat * grad_latlon::Get_grad_lat_matrix ()
{
    return partial_lat;
}

SpMat * grad_latlon::Get_grad_lon_matrix ()
{
    return partial_lon;
}

void grad_latlon::Test_grad_latlon (_ValueType (* function)(_ValueType, _ValueType), _ValueType (* partial_lat_function)(_ValueType, _ValueType), _ValueType (* partial_lon_function)(_ValueType, _ValueType))
{
    printf ("Test_grad_latlon\n");
    int size = lat_dim * lon_dim;
#ifdef STL_SPARSEMATRIX_VERSION	
	Vec * analytic_function = new Vec;
	Vec * analytic_partial_lat = new Vec;
	Vec * analytic_partial_lon = new Vec;
	Vec * discrete_partial_lat;
	Vec * discrete_partial_lon;
	analytic_partial_lat->reserve(size);
	analytic_partial_lon->reserve(size);
    
    for (int i = 0; i < size; i ++)
    {
        analytic_function->push_back(function (coord_lat[i], coord_lon[i]));
        analytic_partial_lat->push_back(partial_lat_function (coord_lat[i], coord_lon[i]));
        analytic_partial_lon->push_back(partial_lon_function (coord_lat[i], coord_lon[i]));
    }
    
    // use grad matrix to calculate discrete grad
    discrete_partial_lat = SpMat::mat_vec_mul (partial_lat, analytic_function);
    discrete_partial_lon = SpMat::mat_vec_mul (partial_lon, analytic_function);

    printf ("test partial lat\n");
    for (int i = 0; i < size; i ++)
    //    printf ("%5.5f\t\t\t%5.5f\n", (*analytic_partial_lat)[i], (*discrete_partial_lat)[i]);
        std::cout << (*analytic_partial_lat)[i] << "\t" << (*discrete_partial_lat)[i] << std::endl;
    printf ("test partial lon\n");
    for (int i = 0; i < size; i ++)
    //    printf ("%5.5f\t\t\t%5.5f\n", (*analytic_partial_lat)[i], (*discrete_partial_lat)[i]);
        std::cout << (*analytic_partial_lon)[i] << "\t" << (*discrete_partial_lon)[i] << std::endl;

	delete analytic_function;
	delete analytic_partial_lat;
	delete analytic_partial_lon;
	delete discrete_partial_lat;
	delete discrete_partial_lon;
#else
    _ValueType * analytic_function = new _ValueType [size];
    _ValueType * analytic_partial_lat = new _ValueType [size];
    _ValueType * analytic_partial_lon = new _ValueType [size];
    _ValueType * discrete_partial_lat = new _ValueType [size];
    _ValueType * discrete_partial_lon = new _ValueType [size];
    assert (analytic_function != (_ValueType *) 0);
    assert (analytic_partial_lat != (_ValueType *) 0);
    assert (analytic_partial_lon != (_ValueType *) 0);
    assert (discrete_partial_lat != (_ValueType *) 0);
    assert (discrete_partial_lon != (_ValueType *) 0);
    
    for (int i = 0; i < size; i ++)
    {
        analytic_function[i] = function (coord_lat[i], coord_lon[i]);
        analytic_partial_lat[i] = partial_lat_function (coord_lat[i], coord_lon[i]);
        analytic_partial_lon[i] = partial_lon_function (coord_lat[i], coord_lon[i]);
    }

    // use grad matrix to calculate discrete grad
    partial_lat->Matrix_vector_multiple (discrete_partial_lat, size, analytic_function, size);
    partial_lon->Matrix_vector_multiple (discrete_partial_lon, size, analytic_function, size);

    printf ("test partial lat\n");
    for (int i = 0; i < size; i ++)
        printf ("%5.5f\t\t\t%5.5f\n", analytic_partial_lat[i], discrete_partial_lat[i]);
    printf ("test partial lon\n");
    for (int i = 0; i < size; i ++)
        printf ("%5.5f\t\t\t%5.5f\n", analytic_partial_lat[i], discrete_partial_lat[i]);
    delete [] analytic_function;
    delete [] analytic_partial_lat;
    delete [] analytic_partial_lon;
    delete [] discrete_partial_lat;
    delete [] discrete_partial_lon;
#endif
}

// note: temp use; we will export final matrix to files
/* case 1: for analytical functions */
void grad_latlon::Test_final_results (SpMat * m1, SpMat * m2lat, SpMat * m2lon, _ValueType (* function)(_ValueType, _ValueType), _ValueType (* partial_lat_function)(_ValueType, _ValueType), _ValueType (* partial_lon_function)(_ValueType, _ValueType), _ValueType * dst_lat, _ValueType * dst_lon, int * dst_mask)
{
    //printf ("Test_final_results\n");
    printf ("Analytic\t\tOrder1\t\tOrder2_analytic\t\tOrder2_discrete\n");
    int size = lat_dim * lon_dim;
#ifdef STL_SPARSEMATRIX_VERSION
	_IDX src_dim = m1->get_col_dim ();
	_IDX dst_dim = m1->get_row_dim ();
	Vec * analytic_function, * analytic_partial_lat, * analytic_partial_lon;
	Vec * discrete_partial_lat, * discrete_partial_lon;
	Vec * results_analytic;
	Vec * results_order1, * results_order1_rerrors;
	Vec * results_order2_analytic_lat, * results_order2_analytic_lon, * results_order2_analytic, * results_order2_analytic_rerrors;
	Vec * results_order2_discrete_lat, * results_order2_discrete_lon, * results_order2_discrete, * results_order2_discrete_rerrors;

	analytic_function = new Vec;
	analytic_partial_lat = new Vec;
	analytic_partial_lon = new Vec;
	results_analytic = new Vec;
	analytic_function->reserve(src_dim);
	analytic_partial_lat->reserve(src_dim);
	analytic_partial_lon->reserve(src_dim);
	results_analytic->reserve(dst_dim);
    for (int i = 0; i < src_dim; i ++)
    {
        analytic_function->push_back(function (coord_lat[i], coord_lon[i]));
        analytic_partial_lat->push_back(partial_lat_function (coord_lat[i], coord_lon[i]));
        analytic_partial_lon->push_back(partial_lon_function (coord_lat[i], coord_lon[i]));
    }
    for (int i = 0; i < dst_dim; i ++)
    {
        results_analytic->push_back(function (dst_lat[i], dst_lon[i]));
    }
#else
    int src_dim = m1->Get_col_dim ();
    int dst_dim = m1->Get_row_dim ();
    assert (size == src_dim);
    //partial_lat->print ();
    //partial_lon->print ();
    //m1->print ();
    //m2lat->print ();
    //m2lon->print ();
    _ValueType * analytic_function = new _ValueType [src_dim];
    _ValueType * analytic_partial_lat = new _ValueType [src_dim];
    _ValueType * analytic_partial_lon = new _ValueType [src_dim];
    _ValueType * discrete_partial_lat = new _ValueType [src_dim];
    _ValueType * discrete_partial_lon = new _ValueType [src_dim];
    _ValueType * results_analytic = new _ValueType [dst_dim];
    _ValueType * results_order1 = new _ValueType [dst_dim];
    _ValueType * results_order1_rerrors = new _ValueType [dst_dim];
    _ValueType * results_order2_analytic = new _ValueType [dst_dim];
    _ValueType * results_order2_analytic_rerrors = new _ValueType [dst_dim];
    _ValueType * results_order2_analytic_lat = new _ValueType [dst_dim];
    _ValueType * results_order2_analytic_lon = new _ValueType [dst_dim];
    _ValueType * results_order2_discrete = new _ValueType [dst_dim];
    _ValueType * results_order2_discrete_rerrors = new _ValueType [dst_dim];
    _ValueType * results_order2_discrete_lat = new _ValueType [dst_dim];
    _ValueType * results_order2_discrete_lon = new _ValueType [dst_dim];

    assert (analytic_function != (_ValueType *) 0);
    assert (analytic_partial_lat != (_ValueType *) 0);
    assert (analytic_partial_lon != (_ValueType *) 0);
    assert (discrete_partial_lat != (_ValueType *) 0);
    assert (discrete_partial_lon != (_ValueType *) 0);
    assert (results_analytic != (_ValueType *) 0);
    assert (results_order1 != (_ValueType *) 0);
    assert (results_order1_rerrors != (_ValueType *) 0);
    assert (results_order2_analytic != (_ValueType *) 0);
    assert (results_order2_analytic_rerrors != (_ValueType *) 0);
    assert (results_order2_analytic_lat != (_ValueType *) 0);
    assert (results_order2_analytic_lon != (_ValueType *) 0);
    assert (results_order2_discrete != (_ValueType *) 0);
    assert (results_order2_discrete_rerrors != (_ValueType *) 0);
    assert (results_order2_discrete_lat != (_ValueType *) 0);
    assert (results_order2_discrete_lon != (_ValueType *) 0);
    
    for (int i = 0; i < src_dim; i ++)
    {
        analytic_function[i] = function (coord_lat[i], coord_lon[i]);
        analytic_partial_lat[i] = partial_lat_function (coord_lat[i], coord_lon[i]);
        analytic_partial_lon[i] = partial_lon_function (coord_lat[i], coord_lon[i]);
    }
    for (int i = 0; i < dst_dim; i ++)
    {
        results_analytic[i] = function (dst_lat[i], dst_lon[i]);
    }
#endif
#ifdef STL_SPARSEMATRIX_VERSION
    // use grad matrix to calculate discrete grad
	discrete_partial_lat = SpMat::mat_vec_mul(partial_lat,analytic_function);
	discrete_partial_lon = SpMat::mat_vec_mul(partial_lon,analytic_function);

    // use remap matrix to calculate field value
	results_order1 = SpMat::mat_vec_mul(m1,analytic_function);
	results_order2_analytic_lat = SpMat::mat_vec_mul(m2lat,analytic_partial_lat);
	results_order2_analytic_lon = SpMat::mat_vec_mul(m2lon,analytic_partial_lon);
	results_order2_discrete_lat = SpMat::mat_vec_mul(m2lat,discrete_partial_lat);
	results_order2_discrete_lon = SpMat::mat_vec_mul(m2lon,discrete_partial_lon);

    for (int i = 0; i < dst_dim; i ++)
    {
        if (dst_mask[i] == 0)
        {
            (*results_analytic)[i] = 0.0;
            (*results_order1)[i] = 0.0;
            (*results_order2_analytic_lat)[i] = 0.0;
            (*results_order2_analytic_lon)[i] = 0.0;
            (*results_order2_discrete_lat)[i] = 0.0;
            (*results_order2_discrete_lon)[i] = 0.0;
        }
    }
	
	results_order2_analytic = SpMat::vec_nadd(3, results_order1, results_order2_analytic_lat, results_order2_analytic_lon);
	results_order2_discrete = SpMat::vec_nadd(3, results_order1, results_order2_discrete_lat, results_order2_discrete_lon);

	//TODO: mask is not considered here
	results_order1_rerrors = SpMat::vec_subdiv(results_order1, results_analytic);
	results_order2_analytic_rerrors = SpMat::vec_subdiv(results_order2_analytic, results_analytic);
	results_order2_discrete_rerrors = SpMat::vec_subdiv(results_order2_discrete, results_analytic);
    _ValueType rmsd_order1 = SpMat::vec_rmsd(results_order1_rerrors);               // root mean square deviation
    _ValueType rmsd_order2_analytic = SpMat::vec_rmsd(results_order2_analytic_rerrors);
    _ValueType rmsd_order2_discrete = SpMat::vec_rmsd(results_order2_discrete_rerrors);
    //printf ("rmsd_order1: %lf\n", rmsd_order1);
    //printf ("rmsd_order2_analytic: %lf\n", rmsd_order2_analytic);
    //printf ("rmsd_order2_discrete: %lf\n", rmsd_order2_discrete);
    std::cout << "rmsd_order1: " << rmsd_order1 << std::endl;
    std::cout << "rmsd_order2_analytic: :" << rmsd_order2_analytic << std::endl;
    std::cout << "rmsd_order2_discrete: :" << rmsd_order2_discrete << std::endl;
#else
    // use grad matrix to calculate discrete grad
    partial_lat->Matrix_vector_multiple (discrete_partial_lat, src_dim, analytic_function, src_dim);
    partial_lon->Matrix_vector_multiple (discrete_partial_lon, src_dim, analytic_function, src_dim);

    // use remap matrix to calculate field value
    m1->Matrix_vector_multiple (results_order1, dst_dim, analytic_function, src_dim);
    m2lat->Matrix_vector_multiple (results_order2_analytic_lat, dst_dim, analytic_partial_lat, src_dim);
    m2lon->Matrix_vector_multiple (results_order2_analytic_lon, dst_dim, analytic_partial_lon, src_dim);
    m2lat->Matrix_vector_multiple (results_order2_discrete_lat, dst_dim, discrete_partial_lat, src_dim);
    m2lon->Matrix_vector_multiple (results_order2_discrete_lon, dst_dim, discrete_partial_lon, src_dim);

    for (int i = 0; i < dst_dim; i ++)
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
    _ValueType rmsd_order1 = 0.0;               // root mean square deviation
    _ValueType rmsd_order2_analytic = 0.0;
    _ValueType rmsd_order2_discrete = 0.0;
    for (int i = 0; i < dst_dim; i ++)
    {
        results_order2_analytic[i] = results_order1[i] + results_order2_analytic_lat[i] + results_order2_analytic_lon[i];
        results_order2_discrete[i] = results_order1[i] + results_order2_discrete_lat[i] + results_order2_discrete_lon[i];
        results_order1_rerrors[i] = results_order1[i] - results_analytic[i];
        results_order2_analytic_rerrors[i] = results_order2_analytic[i] - results_analytic[i];
        results_order2_discrete_rerrors[i] = results_order2_discrete[i] - results_analytic[i];
        if (results_analytic[i] > 1e-50 || results_analytic[i] < - 1e-50)
        {
            results_order1_rerrors[i] /= results_analytic[i];
            results_order2_analytic_rerrors[i] /= results_analytic[i];
            results_order2_discrete_rerrors[i] /= results_analytic[i];
        }
        else
        {
            if (results_order1_rerrors[i] > 1e-50 || results_order1_rerrors[i] < - 1e-50)
                results_order1_rerrors[i] = 1.0;
            else
                results_order1_rerrors[i] = 0.0;

            if (results_order2_analytic_rerrors[i] > 1e-50 || results_order2_analytic_rerrors[i] < - 1e-50)
                results_order2_analytic_rerrors[i] = 1.0;
            else
                results_order2_analytic_rerrors[i] = 0.0;

            if (results_order2_discrete_rerrors[i] > 1e-50 || results_order2_discrete_rerrors[i] < - 1e-50)
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
#endif
}

/* case 2: for area-averaged analytical functions */
void grad_latlon::Test_final_results (SpMat * m1, SpMat * m2lat, SpMat * m2lon, _ValueType (* function)(_ValueType, _ValueType, _ValueType, _ValueType), _ValueType (* partial_lat_function)(_ValueType, _ValueType), _ValueType (* partial_lon_function)(_ValueType, _ValueType), _ValueType * dst_lat, _ValueType * dst_lon, int * dst_mask, int dst_nlat, int dst_nlon)
{
    //printf ("Test_final_results\n");
    printf ("Analytic\t\tOrder1\t\tOrder2_analytic\t\tOrder2_discrete\n");
#ifdef STL_SPARSEMATRIX_VERSION 
#else
    int size = lat_dim * lon_dim;
    int src_dim = m1->Get_col_dim ();
    int dst_dim = m1->Get_row_dim ();
    assert (size == src_dim);
    //partial_lat->print ();
    //partial_lon->print ();
    //m1->print ();
    //m2lat->print ();
    //m2lon->print ();
    _ValueType * analytic_function = new _ValueType [src_dim];
    _ValueType * analytic_partial_lat = new _ValueType [src_dim];
    _ValueType * analytic_partial_lon = new _ValueType [src_dim];
    _ValueType * discrete_partial_lat = new _ValueType [src_dim];
    _ValueType * discrete_partial_lon = new _ValueType [src_dim];
    _ValueType * results_analytic = new _ValueType [dst_dim];
    _ValueType * results_order1 = new _ValueType [dst_dim];
    _ValueType * results_order1_rerrors = new _ValueType [dst_dim];
    _ValueType * results_order2_analytic = new _ValueType [dst_dim];
    _ValueType * results_order2_analytic_rerrors = new _ValueType [dst_dim];
    _ValueType * results_order2_analytic_lat = new _ValueType [dst_dim];
    _ValueType * results_order2_analytic_lon = new _ValueType [dst_dim];
    _ValueType * results_order2_discrete = new _ValueType [dst_dim];
    _ValueType * results_order2_discrete_rerrors = new _ValueType [dst_dim];
    _ValueType * results_order2_discrete_lat = new _ValueType [dst_dim];
    _ValueType * results_order2_discrete_lon = new _ValueType [dst_dim];

    assert (analytic_function != (_ValueType *) 0);
    assert (analytic_partial_lat != (_ValueType *) 0);
    assert (analytic_partial_lon != (_ValueType *) 0);
    assert (discrete_partial_lat != (_ValueType *) 0);
    assert (discrete_partial_lon != (_ValueType *) 0);
    assert (results_analytic != (_ValueType *) 0);
    assert (results_order1 != (_ValueType *) 0);
    assert (results_order1_rerrors != (_ValueType *) 0);
    assert (results_order2_analytic != (_ValueType *) 0);
    assert (results_order2_analytic_rerrors != (_ValueType *) 0);
    assert (results_order2_analytic_lat != (_ValueType *) 0);
    assert (results_order2_analytic_lon != (_ValueType *) 0);
    assert (results_order2_discrete != (_ValueType *) 0);
    assert (results_order2_discrete_rerrors != (_ValueType *) 0);
    assert (results_order2_discrete_lat != (_ValueType *) 0);
    assert (results_order2_discrete_lon != (_ValueType *) 0);
    
    _ValueType dlat = FULL_LONGITUDE / 2 / lat_dim / 2;
    _ValueType dlon = FULL_LONGITUDE / lon_dim / 2;
    //printf ("dlat: %f\n", dlat);
    //printf ("dlon: %f\n", dlon);
    for (int i = 0; i < src_dim; i ++)
    {
        analytic_function[i] = function (coord_lat[i], coord_lon[i], dlat, dlon);
        analytic_partial_lat[i] = partial_lat_function (coord_lat[i], coord_lon[i]);
        analytic_partial_lon[i] = partial_lon_function (coord_lat[i], coord_lon[i]);
    }

    dlat = FULL_LONGITUDE / 2 / dst_nlat / 2;
    dlon = FULL_LONGITUDE / dst_nlon / 2;
    printf ("dlat: %f\n", dlat);
    printf ("dlon: %f\n", dlon);
    for (int i = 0; i < dst_dim; i ++)
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

    for (int i = 0; i < dst_dim; i ++)
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
    _ValueType rmsd_order1 = 0.0;               // root mean square deviation
    _ValueType rmsd_order2_analytic = 0.0;
    _ValueType rmsd_order2_discrete = 0.0;
    for (int i = 0; i < dst_dim; i ++)
    {
        results_order2_analytic[i] = results_order1[i] + results_order2_analytic_lat[i] + results_order2_analytic_lon[i];
        results_order2_discrete[i] = results_order1[i] + results_order2_discrete_lat[i] + results_order2_discrete_lon[i];
        results_order1_rerrors[i] = results_order1[i] - results_analytic[i];
        results_order2_analytic_rerrors[i] = results_order2_analytic[i] - results_analytic[i];
        results_order2_discrete_rerrors[i] = results_order2_discrete[i] - results_analytic[i];
        if (results_analytic[i] > 1e-50)
        {
            results_order1_rerrors[i] /= results_analytic[i];
            results_order2_analytic_rerrors[i] /= results_analytic[i];
            results_order2_discrete_rerrors[i] /= results_analytic[i];
        }
        else
        {
            if (results_order1_rerrors[i] > 1e-50 || results_order1_rerrors[i] < - 1e-50)
                results_order1_rerrors[i] = 1.0;
            else
                results_order1_rerrors[i] = 0.0;

            if (results_order2_analytic_rerrors[i] > 1e-50 || results_order2_analytic_rerrors[i] < - 1e-50)
                results_order2_analytic_rerrors[i] = 1.0;
            else
                results_order2_analytic_rerrors[i] = 0.0;

            if (results_order2_discrete_rerrors[i] > 1e-50 || results_order2_discrete_rerrors[i] < - 1e-50)
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
#endif
}

/* case 3: for discrete distribution, e.g. pseudo-analytical function values */
void grad_latlon::Test_final_results (SpMat * m1, SpMat * m2lat, SpMat * m2lon, const _ValueType * src_field_data, int src_grid_size, const _ValueType * dst_field_data, int dst_grid_size, _ValueType * dst_lat, _ValueType * dst_lon, int * dst_mask)
{
    //printf ("Test_final_results\n");
    printf ("Analytic\t\tOrder1\t\tOrder2_analytic\t\tOrder2_discrete\n");
#ifdef STL_SPARSEMATRIX_VERSION 
    int size = lat_dim * lon_dim;
    int src_dim = m1->get_col_dim ();
    int dst_dim = m1->get_row_dim ();
    printf ("src_dim:%d\n", src_dim);
    printf ("dst_dim:%d\n", dst_dim);
    printf ("src_grid_size:%d\n", src_grid_size);
    printf ("dst_grid_size:%d\n", dst_grid_size);
    assert (src_grid_size == (int)src_dim);
    assert (dst_grid_size == (int)dst_dim);
    //partial_lat->print ();
    //partial_lon->print ();
    //m1->print ();
    //m2lat->print ();
    //m2lon->print ();
    Vec * src_field_dataV = new Vec;
    Vec * dst_field_dataV = new Vec;
    src_field_dataV->reserve(src_dim);
    dst_field_dataV->reserve(dst_dim);

    Vec * results_order1;
    Vec * results_order1_rerrors;

    Vec * discrete_partial_lat;
    Vec * discrete_partial_lon;
    Vec * results_order2_discrete_lat;
    Vec * results_order2_discrete_lon;
    Vec * results_order2_discrete;
    Vec * results_order2_discrete_rerrors;

    // copy source field and destination field
    for (int i = 0; i < src_dim; i ++)
        src_field_dataV->push_back(src_field_data[i]);
    for (int i = 0; i < dst_dim; i ++)
        dst_field_dataV->push_back(dst_field_data[i]);
    printf("src_field_dataV: %ld\n", src_field_dataV->size());
    
    // use grad matrix to calculate discrete grad
    discrete_partial_lat = SpMat::mat_vec_mul(partial_lat, src_field_dataV);
    discrete_partial_lon = SpMat::mat_vec_mul(partial_lon, src_field_dataV);

    // use remap matrix to calculate field value
    results_order1 = SpMat::mat_vec_mul(m1, src_field_dataV);
    results_order2_discrete_lat = SpMat::mat_vec_mul(m2lat, discrete_partial_lat);
    results_order2_discrete_lon = SpMat::mat_vec_mul(m2lon, discrete_partial_lon);
    results_order2_discrete = SpMat::vec_nadd(3, results_order1, results_order2_discrete_lat, results_order2_discrete_lon);
    results_order1_rerrors = SpMat::vec_subdiv(results_order1, dst_field_dataV);
    results_order2_discrete_rerrors = SpMat::vec_subdiv(results_order2_discrete, dst_field_dataV);
    _ValueType rmse1 = SpMat::vec_rmsd(results_order1_rerrors);
    _ValueType rmse2 = SpMat::vec_rmsd(results_order2_discrete_rerrors);

    //printf ("rmsd_order1: %lf\n", rmse1);
    //printf ("rmsd_order2_discrete: %lf\n", rmse2);
    std::cout << "rmsd_order1: " << rmse1 << std::endl;
    std::cout << "rmsd_order2_discrete: " << rmse2 << std::endl;
    //return;
    delete  src_field_dataV;
    delete  dst_field_dataV;
    delete  results_order1;
    delete  results_order1_rerrors;
    delete  discrete_partial_lat;
    delete  discrete_partial_lon;
    delete  results_order2_discrete_lat;
    delete  results_order2_discrete_lon;
    delete  results_order2_discrete;
    delete  results_order2_discrete_rerrors;
#else
    int size = lat_dim * lon_dim;
    int src_dim = m1->Get_col_dim ();
    int dst_dim = m1->Get_row_dim ();
    printf ("src_dim:%d\n", src_dim);
    printf ("dst_dim:%d\n", dst_dim);
    printf ("src_grid_size:%d\n", src_grid_size);
    printf ("dst_grid_size:%d\n", dst_grid_size);
    assert (src_grid_size == (int)src_dim);
    assert (dst_grid_size == (int)dst_dim);
    //partial_lat->print ();
    //partial_lon->print ();
    //m1->print ();
    //m2lat->print ();
    //m2lon->print ();
    _ValueType * analytic_function = new _ValueType [src_dim];
    _ValueType * analytic_partial_lat = new _ValueType [src_dim];
    _ValueType * analytic_partial_lon = new _ValueType [src_dim];
    _ValueType * discrete_partial_lat = new _ValueType [src_dim];
    _ValueType * discrete_partial_lon = new _ValueType [src_dim];
    _ValueType * results_analytic = new _ValueType [dst_dim];
    _ValueType * results_order1 = new _ValueType [dst_dim];
    _ValueType * results_order1_rerrors = new _ValueType [dst_dim];
    _ValueType * results_order2_discrete = new _ValueType [dst_dim];
    _ValueType * results_order2_discrete_rerrors = new _ValueType [dst_dim];
    _ValueType * results_order2_discrete_lat = new _ValueType [dst_dim];
    _ValueType * results_order2_discrete_lon = new _ValueType [dst_dim];

    assert (analytic_function != (_ValueType *) 0);
    assert (analytic_partial_lat != (_ValueType *) 0);
    assert (analytic_partial_lon != (_ValueType *) 0);
    assert (discrete_partial_lat != (_ValueType *) 0);
    assert (discrete_partial_lon != (_ValueType *) 0);
    assert (results_analytic != (_ValueType *) 0);
    assert (results_order1 != (_ValueType *) 0);
    assert (results_order1_rerrors != (_ValueType *) 0);
    assert (results_order2_discrete != (_ValueType *) 0);
    assert (results_order2_discrete_rerrors != (_ValueType *) 0);
    assert (results_order2_discrete_lat != (_ValueType *) 0);
    assert (results_order2_discrete_lon != (_ValueType *) 0);
    
    // copy source field and destination field
    for (int i = 0; i < src_dim; i ++)
        analytic_function[i] = src_field_data[i];
    for (int i = 0; i < dst_dim; i ++)
        results_analytic[i] = dst_field_data[i];
    
    // use grad matrix to calculate discrete grad
    partial_lat->Matrix_vector_multiple (discrete_partial_lat, src_dim, analytic_function, src_dim);
    partial_lon->Matrix_vector_multiple (discrete_partial_lon, src_dim, analytic_function, src_dim);

    // use remap matrix to calculate field value
    m1->Matrix_vector_multiple (results_order1, dst_dim, analytic_function, src_dim);
    m2lat->Matrix_vector_multiple (results_order2_discrete_lat, dst_dim, discrete_partial_lat, src_dim);
    m2lon->Matrix_vector_multiple (results_order2_discrete_lon, dst_dim, discrete_partial_lon, src_dim);

    for (int i = 0; i < dst_dim; i ++)
    {
        if (dst_mask[i] == 0)
        {
            results_analytic[i] = 0.0;
            results_order1[i] = 0.0;
            results_order2_discrete_lat[i] = 0.0;
            results_order2_discrete_lon[i] = 0.0;
        }
    }
    _ValueType rmsd_order1 = 0.0;               // root mean square deviation
    _ValueType rmsd_order2_analytic = 0.0;
    _ValueType rmsd_order2_discrete = 0.0;
    for (int i = 0; i < dst_dim; i ++)
    {
        results_order2_discrete[i] = results_order1[i] + results_order2_discrete_lat[i] + results_order2_discrete_lon[i];
        results_order1_rerrors[i] = results_order1[i] - results_analytic[i];
        results_order2_discrete_rerrors[i] = results_order2_discrete[i] - results_analytic[i];
        if (fabs(results_analytic[i]) > 1e-50)
        {
            results_order1_rerrors[i] /= results_analytic[i];
            results_order2_discrete_rerrors[i] /= results_analytic[i];
        }
        else
        {
            if (fabs(results_order1_rerrors[i]) > 1e-50)
                results_order1_rerrors[i] = 8888.0;
            else
                results_order1_rerrors[i] = 0.0;


            if (fabs(results_order2_discrete_rerrors[i]))
                results_order2_discrete_rerrors[i] = 1.0;
            else
                results_order2_discrete_rerrors[i] = 0.0;
        }
        rmsd_order1 += results_order1_rerrors[i] * results_order1_rerrors[i];
        rmsd_order2_discrete += results_order2_discrete_rerrors[i] * results_order2_discrete_rerrors[i];

        printf ("%5.8f\t\t\t%5.8f\t\t\t%5.8f\t\t\t%5.8f\t\t%5.8f\t\t%5.8f\t\t%5.8f\n", results_analytic[i], results_order1[i], results_order2_discrete[i], results_order2_discrete[i], results_order1_rerrors[i], results_order2_discrete_rerrors[i], results_order2_discrete_rerrors[i]);
    }
    rmsd_order1 = sqrt (rmsd_order1 / dst_dim);
    rmsd_order2_analytic = sqrt (rmsd_order2_discrete / dst_dim);
    rmsd_order2_discrete = sqrt (rmsd_order2_discrete / dst_dim);
    printf ("rmsd_order1: %f\n", rmsd_order1);
    printf ("rmsd_order2_analytic: %f\n", rmsd_order2_analytic);
    printf ("rmsd_order2_discrete: %f\n", rmsd_order2_discrete);
    //return;
    delete [] analytic_function;
    delete [] discrete_partial_lat;
    delete [] discrete_partial_lon;
    delete [] results_order1;
    delete [] results_order1_rerrors;
    delete [] results_order2_discrete;
    delete [] results_order2_discrete_rerrors;
    delete [] results_order2_discrete_lat;
    delete [] results_order2_discrete_lon;
#endif
}
