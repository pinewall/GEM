#ifndef GRADIENT_H
#define GRADIENT_H

#include "types.h"
#define NEW_VERSION

#define STL_SPARSEMATRIX_VERSION
#ifdef STL_SPARSEMATRIX_VERSION
	#include "STL_SparseMatrix.h"
	typedef STL_SparseMatrix SpMat;
#else
	#include "SParseMatrix.h"
	typedef SparseMatrix SpMat;
#endif

class grad_latlon
{
    private:
        static const int MAX_NEIGHBOR = 16;
#ifdef USE_DEGREES
        static const _ValueType CROSS_LONGITUDE = 270.0;
        static const _ValueType FULL_LONGITUDE = 360.0;
#endif
#ifdef USE_RADIANS
        static const _ValueType FULL_LONGITUDE = 3.14159265359 * 2;
        static const _ValueType CROSS_LONGITUDE = 3.14159265359 * 1.5;
        static const _ValueType DEGREE_TO_RADIAN = 3.14159265359 / 180.0;
#endif
        int lat_dim;
        int lon_dim;
		int ID_self;
        _ValueType * coord_lat;
        _ValueType * coord_lon;
        int neighbor_list[MAX_NEIGHBOR];
        int neighbor_order[MAX_NEIGHBOR + 1];
        _ValueType delta_lat[MAX_NEIGHBOR];
        _ValueType delta_lon[MAX_NEIGHBOR];
		SpMat * partial_lat;
		SpMat * partial_lon;

    public:
		//static size_t Get_ID_LIMIT(); 
		//static void Set_ID_LIMIT();

        grad_latlon (int _lat_dim, int _lon_dim, _ValueType * _coord_lat, _ValueType * _coord_lon);
        ~grad_latlon ();
        void Get_single_neighbors (int id, int & neighbor_size);
        void Get_single_neighbor_order_by_row (int id, int neighbor_size);
        void Calculate_single_grad_latlon (int id, int neighbor_size);
        void Calculate_grad_latlon_matrix ();
        SpMat * Calculate_final_matrix (SpMat * m1, SpMat * m2lat, SpMat * m2lon);
        SpMat * Get_grad_lat_matrix ();
        SpMat * Get_grad_lon_matrix ();

        void Test_grad_latlon (_ValueType (* function)(_ValueType, _ValueType), _ValueType (* partial_lat_function)(_ValueType, _ValueType), _ValueType (* partial_lon_function)(_ValueType, _ValueType));
        /* case 1: for analytical functions */
        void Test_final_results (SpMat * m1, SpMat * m2lat, SpMat * m2lon, _ValueType (* function)(_ValueType, _ValueType), _ValueType (* partial_lat_function)(_ValueType, _ValueType), _ValueType (* partial_lon_function)(_ValueType, _ValueType), _ValueType * dst_lat, _ValueType * dst_lon, int * dst_mask);
        /* case 2: for area-averaged analytical functions */
        void Test_final_results (SpMat * m1, SpMat * m2lat, SpMat * m2lon, _ValueType (* function)(_ValueType, _ValueType, _ValueType, _ValueType), _ValueType (* partial_lat_function)(_ValueType, _ValueType), _ValueType (* partial_lon_function)(_ValueType, _ValueType), _ValueType * dst_lat, _ValueType * dst_lon, int * dst_mask, int dst_nlat, int dst_nlon);
        /* case 3: for real flux data on two grids */
        void Test_final_results (SpMat * m1, SpMat * m2lat, SpMat * m2lon, const _ValueType * src_field_data, int src_grid_size, const _ValueType * dst_field_data, int dst_grid_size, _ValueType * dst_lat, _ValueType * dst_lon, int * dst_mask);
};

#endif
