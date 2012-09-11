#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include "types.h"
// note: to improve add operation efficency, we could use std::Vector class etc.
class SparseMatrix
{
    private:
        size_t row_dim;
        size_t col_dim;
        _ValueType alpha;      // parameter of increasement factor
        size_t increase;
        size_t * row_list;
        size_t * row_ptr;     // used for row compessed format
        size_t * col_list;
        //size_t * col_ptr;     // used for col compressed format
        _ValueType * value_list;
        size_t buffer_size;
        size_t current_size;
    public:
        SparseMatrix (size_t _row_dim, size_t _col_dim);
        SparseMatrix (size_t _row_dim, size_t _col_dim, size_t _current_size, size_t * _row_list, size_t * _col_list, _ValueType * _value_list);
        SparseMatrix (size_t _row_dim, size_t _col_dim, size_t * _row_ptr, size_t * _col_list, _ValueType * _value_list);
        SparseMatrix (SparseMatrix * copyer);
        ~SparseMatrix ();

        size_t Get_row_dim ()  {   return row_dim; }
        size_t Get_col_dim ()  {   return col_dim; }
        size_t Get_current_size () {   return current_size;    }
        size_t * Get_row_list ()   {   return row_list;    }
        size_t * Get_col_list ()   {   return col_list;    }
        _ValueType * Get_value_list ()  {   return value_list;  }
        void Add_entry (size_t row, size_t col, _ValueType value);
        void Complete_row_ptr ();
        void Resize ();
        void Trim ();
        SparseMatrix * Matrix_transpose ();
        SparseMatrix * Matrix_scalar_multiple (_ValueType alpha);
        SparseMatrix * Matrix_add (SparseMatrix * adder);
        SparseMatrix * Matrix_substract (SparseMatrix * suber);
        SparseMatrix * Matrix_multiple (SparseMatrix * multier);
        // matrix vector multiplication
        void Matrix_vector_multiple (_ValueType * output, size_t osize, _ValueType * input, size_t isize);
        void Print();
};

#endif
