#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include "types.h"
// note: to improve add operation efficency, we could use std::Vector class etc.
class SparseMatrix
{
    private:
        INT row_dim;
        INT col_dim;
        double alpha;      // parameter of increasement factor
        INT increase;
        INT * row_list;
        INT * row_ptr;     // used for row compessed format
        INT * col_list;
        //INT * col_ptr;     // used for col compressed format
        double * value_list;
        INT buffer_size;
        INT current_size;
    public:
        SparseMatrix (INT _row_dim, INT _col_dim);
        SparseMatrix (INT _row_dim, INT _col_dim, INT _current_size, INT * _row_list, INT * _col_list, double * _value_list);
        SparseMatrix (INT _row_dim, INT _col_dim, INT * _row_ptr, INT * _col_list, double * _value_list);
        SparseMatrix (SparseMatrix * copyer);
        ~SparseMatrix ();

        INT Get_row_dim ()  {   return row_dim; }
        INT Get_col_dim ()  {   return col_dim; }
        INT Get_current_size () {   return current_size;    }
        INT * Get_row_list ()   {   return row_list;    }
        INT * Get_col_list ()   {   return col_list;    }
        double * Get_value_list ()  {   return value_list;  }
        void Add_entry (INT row, INT col, double value);
        void Complete_row_ptr ();
        void Resize ();
        void Trim ();
        SparseMatrix * Matrix_transpose ();
        SparseMatrix * Matrix_scalar_multiple (double alpha);
        SparseMatrix * Matrix_add (SparseMatrix * adder);
        SparseMatrix * Matrix_substract (SparseMatrix * suber);
        SparseMatrix * Matrix_multiple (SparseMatrix * multier);
        // matrix vector multiplication
        void Matrix_vector_multiple (double * output, INT osize, double * input, INT isize);
        void Print();
};

#endif
