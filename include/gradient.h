#ifndef GRADIENT_H
#define GRADIENT_H

#include "SParseMatrix.h"
#include "types.h"
#define NEW_VERSION

class grad_latlon
{
    private:
        static const int MAX_NEIGHBOR = 16;
#ifdef USE_DEGREES
        static const double CROSS_LONGITUDE = 270.0;
        static const double FULL_LONGITUDE = 360.0;
#endif
#ifdef USE_RADIANS
        static const double FULL_LONGITUDE = 3.14159265359 * 2;
        static const double CROSS_LONGITUDE = 3.14159265359 * 1.5;
        static const double DEGREE_TO_RADIAN = 3.14159265359 / 180.0;
#endif
        INT lat_dim;
        INT lon_dim;
        double * coord_lat;
        double * coord_lon;
        INT neighbor_list[MAX_NEIGHBOR];
        INT neighbor_order[MAX_NEIGHBOR + 1];
        double delta_lat[MAX_NEIGHBOR];
        double delta_lon[MAX_NEIGHBOR];
        SparseMatrix * partial_lat;
        SparseMatrix * partial_lon;
    public:
        grad_latlon (INT _lat_dim, INT _lon_dim, double * _coord_lat, double * _coord_lon);
        ~grad_latlon ();
        void Get_single_neighbors (INT id, INT & neighbor_size);
        void Get_single_neighbor_order_by_row (INT id, INT neighbor_size);
        void Calculate_single_grad_latlon (INT id, INT neighbor_size);
        void Calculate_grad_latlon_matrix ();
        SparseMatrix * Calculate_final_matrix (SparseMatrix * m1, SparseMatrix * m2lat, SparseMatrix * m2lon);
        SparseMatrix * Get_grad_lat_matrix ();
        SparseMatrix * Get_grad_lon_matrix ();

        void Test_grad_latlon (double (* function)(double, double), double (* partial_lat_function)(double, double), double (* partial_lon_function)(double, double));
        void Test_final_results (SparseMatrix * m1, SparseMatrix * m2lat, SparseMatrix * m2lon, double (* function)(double, double), double (* partial_lat_function)(double, double), double (* partial_lon_function)(double, double), double * dst_lat, double * dst_lon, int * dst_mask);
        void Test_final_results (SparseMatrix * m1, SparseMatrix * m2lat, SparseMatrix * m2lon, double (* function)(double, double, double, double), double (* partial_lat_function)(double, double), double (* partial_lon_function)(double, double), double * dst_lat, double * dst_lon, int * dst_mask, CDF_INT dst_nlat, CDF_INT dst_nlon);
};
#endif
