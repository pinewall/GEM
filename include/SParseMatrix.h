#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

// note: to improve add operation efficency, we could use std::Vector class etc.
class SparseMatrix
{
    private:
        int row_dim;
        int col_dim;
        double alpha;      // parameter of increasement factor
        int increase;
        int * row_list;
        int * row_ptr;     // used for row compessed format
        int * col_list;
        //int * col_ptr;     // used for col compressed format
        double * value_list;
        int buffer_size;
        int current_size;
    public:
        SparseMatrix (int _row_dim, int _col_dim);
        SparseMatrix (int _row_dim, int _col_dim, int _current_size, int * _row_list, int * _col_list, double * _value_list);
        SparseMatrix (int _row_dim, int _col_dim, int * _row_ptr, int * _col_list, double * _value_list);
        SparseMatrix (SparseMatrix * copyer);
        ~SparseMatrix ();

        int Get_row_dim ()  {   return row_dim; }
        int Get_col_dim ()  {   return col_dim; }
        void Add_entry (int row, int col, double value);
        void Complete_row_ptr ();
        void Resize ();
        void Trim ();
        SparseMatrix * Matrix_transpose ();
        SparseMatrix * Matrix_add (SparseMatrix * adder);
        SparseMatrix * Matrix_substract (SparseMatrix * suber);
        SparseMatrix * Matrix_multiple (SparseMatrix * multier);
        // matrix vector multiplication
        void Matrix_vector_multiple (double * output, int osize, double * input, int isize);
        void print();
};

#endif
