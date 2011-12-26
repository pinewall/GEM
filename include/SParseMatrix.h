#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include "types.h"
// note: to improve add operation efficency, we could use std::Vector class etc.
class SparseMatrix
{
    private:
        UINT row_dim;
        UINT col_dim;
        double alpha;      // parameter of increasement factor
        UINT increase;
        UINT * row_list;
        UINT * row_ptr;     // used for row compessed format
        UINT * col_list;
        //UINT * col_ptr;     // used for col compressed format
        double * value_list;
        UINT buffer_size;
        UINT current_size;
    public:
        SparseMatrix (UINT _row_dim, UINT _col_dim);
        SparseMatrix (UINT _row_dim, UINT _col_dim, UINT _current_size, UINT * _row_list, UINT * _col_list, double * _value_list);
        SparseMatrix (UINT _row_dim, UINT _col_dim, UINT * _row_ptr, UINT * _col_list, double * _value_list);
        SparseMatrix (SparseMatrix * copyer);
        ~SparseMatrix ();

        UINT Get_row_dim ()  {   return row_dim; }
        UINT Get_col_dim ()  {   return col_dim; }
        UINT Get_current_size () {   return current_size;    }
        UINT * Get_row_list ()   {   return row_list;    }
        UINT * Get_col_list ()   {   return col_list;    }
        double * Get_value_list ()  {   return value_list;  }
        void Add_entry (UINT row, UINT col, double value);
        void Complete_row_ptr ();
        void Resize ();
        void Trim ();
        SparseMatrix * Matrix_transpose ();
        SparseMatrix * Matrix_scalar_multiple (double alpha);
        SparseMatrix * Matrix_add (SparseMatrix * adder);
        SparseMatrix * Matrix_substract (SparseMatrix * suber);
        SparseMatrix * Matrix_multiple (SparseMatrix * multier);
        // matrix vector multiplication
        void Matrix_vector_multiple (double * output, UINT osize, double * input, UINT isize);
        void Print();
};

#endif
