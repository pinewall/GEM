#include <SParseMatrix.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

SparseMatrix::SparseMatrix (int _row_dim, int _col_dim)
{
    row_dim = _row_dim;
    col_dim = _col_dim;
    row_ptr = new int [row_dim + 1];
    memset (row_ptr, 0, sizeof (int) * (row_dim + 1));
    //col_ptr = new int [col_dim + 1];
    alpha = 0.0001;               // type is double; note the declaration in header file
    increase = alpha * (row_dim * col_dim);
    increase = (increase > 4) ? (increase - increase % 4) : 4;
    //printf ("SparseMatrix::increase = %d\n", increase);
    assert (increase > 0);
    buffer_size = increase;
    current_size = 0;

    row_list = new int [buffer_size];
    col_list = new int [buffer_size];
    value_list = new double [buffer_size];
    assert ((row_list != (int *) 0));
    assert ((col_list != (int *) 0));
    assert ((value_list != (double *) 0));
}

SparseMatrix::SparseMatrix (int _row_dim, int _col_dim, int _current_size, int * _row_list, int * _col_list, double * _value_list)
{
    row_dim = _row_dim;
    col_dim = _col_dim;
    row_ptr = new int [row_dim + 1];
    alpha = 0.0001;               // type is double; note the declaration in header file
    increase = alpha * (row_dim * col_dim);
    increase = (increase > 4) ? (increase - increase % 4) : 4;
    //printf ("SparseMatrix::increase = %d\n", increase);
    assert (increase > 0);

    current_size = _current_size;
    buffer_size = _current_size;
    row_list = new int [buffer_size];
    col_list = new int [buffer_size];
    value_list = new double [buffer_size];
    assert ((row_list != (int *) 0));
    assert ((col_list != (int *) 0));
    assert ((value_list != (double *) 0));

    memcpy (row_list, _row_list, sizeof (int) * buffer_size);
    memcpy (col_list, _col_list, sizeof (int) * buffer_size);
    memcpy (value_list, _value_list, sizeof (double) * buffer_size);
#ifdef OLD_VERSION
    int current_row = 0;
    row_ptr [current_row] = 0;
    for (int i = 0; i < current_size; i ++)
    {
        if (row_list[i] == current_row)
        {
            //printf ("row_list[i], current_row: %d %d\n", row_list[i], current_row);
            continue;
        }
        else
        {
            // e.g. 0003
            for (int j = current_row + 1; j < row_list[i]; j ++)
            {
                //printf ("j belongs %d to %d\n", current_row + 1, row_list[i]);
                row_ptr[j] = row_ptr[current_row];
            }
            current_row = row_list[i];
            row_ptr[current_row] = i;
        }
    }
    if (current_row < row_dim - 1)
        for (int i = current_row + 1; i < row_dim; i ++)
            row_ptr[i] = current_size;
    row_ptr[row_dim] = current_size;
#endif
    Complete_row_ptr ();
}

SparseMatrix::SparseMatrix (SparseMatrix * copyer)
{
    row_dim = copyer->row_dim;
    col_dim = copyer->col_dim;
    row_ptr = new int [row_dim + 1];
    alpha = copyer->alpha;
    increase = copyer->increase;
    buffer_size = copyer->buffer_size;
    current_size = copyer->current_size;
    row_list = new int [buffer_size];
    col_list = new int [buffer_size];
    value_list = new double [buffer_size];
    memcpy (row_ptr, copyer->row_ptr, (row_dim + 1) * sizeof (int));
    memcpy (row_list, copyer->row_list, current_size * sizeof (int));
    memcpy (col_list, copyer->col_list, current_size * sizeof (int));
    memcpy (value_list, copyer->value_list, current_size * sizeof (double));
}

SparseMatrix::~SparseMatrix()
{
    delete row_list;
    delete col_list;
    delete value_list;
    delete row_ptr;
}

// note: currently we do not support addition to existed place
void SparseMatrix::Add_entry (int row, int col, double value)
{
    if (current_size == buffer_size)
        Resize ();
    row_list[current_size] = row;
    col_list[current_size] = col;
    value_list[current_size] = value;
    //printf ("add triple (%6d, %6d, %6.6f)\n", row, col, value);
    current_size ++;
}

void SparseMatrix::Complete_row_ptr ()
{
    // TODO: complete row_ptr for matrix generated by Add_entry in order
    int current_row = 0;
    row_ptr [current_row] = 0;
    for (int i = 0; i < current_size; i ++)
    {
        if (row_list[i] == current_row)
        {
            //printf ("row_list[i], current_row: %d %d\n", row_list[i], current_row);
            continue;
        }
        else
        {
            // e.g. 0003
            for (int j = current_row + 1; j < row_list[i]; j ++)
            {
                //printf ("j belongs %d to %d\n", current_row + 1, row_list[i]);
                row_ptr[j] = row_ptr[current_row];
            }
            current_row = row_list[i];
            row_ptr[current_row] = i;
        }
    }
    if (current_row < row_dim - 1)
        for (int i = current_row + 1; i < row_dim; i ++)
            row_ptr[i] = current_size;
    row_ptr[row_dim] = current_size;
}

void SparseMatrix::Resize ()
{
    assert (increase > 0);
    buffer_size += increase;
    int * tmp_row_list = new int [buffer_size];
    int * tmp_col_list = new int [buffer_size];
    double * tmp_value_list = new double [buffer_size];
    memcpy (tmp_row_list, row_list, sizeof (int) * current_size);
    memcpy (tmp_col_list, col_list, sizeof (int) * current_size);
    memcpy (tmp_value_list, value_list, sizeof (double) * current_size);
    delete row_list;
    delete col_list;
    delete value_list;
    row_list = tmp_row_list;
    col_list = tmp_col_list;
    value_list = tmp_value_list;
}

void SparseMatrix::Trim ()
{
    int * tmp_row_list = new int [current_size];
    int * tmp_col_list = new int [current_size];
    double * tmp_value_list = new double [current_size];
    memcpy (tmp_row_list, row_list, sizeof (int) * current_size);
    memcpy (tmp_col_list, col_list, sizeof (int) * current_size);
    memcpy (tmp_value_list, value_list, sizeof (double) * current_size);
    delete row_list;
    delete col_list;
    delete value_list;
    row_list = tmp_row_list;
    col_list = tmp_col_list;
    value_list = tmp_value_list;
    buffer_size = current_size;
}

SparseMatrix * SparseMatrix::Matrix_transpose ()
{
    // TODO
    printf ("Matrix_transpose\n");
    int * new_row_list = new int [current_size];
    int * new_col_list = new int [current_size];
    int * new_row_ptr = new int [col_dim + 1];
    double * new_value_list = new double [current_size];

    // calculate number of nonzero element in each column
    int * temp = new int [col_dim];
    //memset (temp, 0, sizeof (int) * col_dim);
    for (int i = 0; i < col_dim; i ++)
        temp[i] = 0;
    for (int i = 0; i < current_size; i ++)
        temp[col_list[i]] ++;

    // new header of each old column
    new_row_ptr[0] = 0;
    for (int i = 0; i < col_dim; i ++)
    {
        printf ("temp[%d] = %d\n", i, temp[i]);
        new_row_ptr[i + 1] = new_row_ptr[i] + temp[i];
        temp[i] = new_row_ptr[i];
    }

    // loop through old matrix
    int new_id;
    for (int i = 0; i < row_dim; i ++)
    {
        for (int j = row_ptr[i]; j < row_ptr[i+1]; j ++)
        {
            new_id = temp[col_list[j]]++;
            new_row_list[new_id] = col_list[j];
            new_col_list[new_id] = i;
            new_value_list[new_id] = value_list[j];
        }
    }

    SparseMatrix * transposed = new SparseMatrix (col_dim, row_dim, current_size, new_row_list, new_col_list, new_value_list);
    transposed->print();
    delete new_row_list;
    delete new_col_list;
    delete new_row_ptr;
    delete new_value_list;
    return transposed;
}

SparseMatrix * SparseMatrix::Matrix_add (SparseMatrix * adder)
{
    // TODO
    printf ("Matrix_add\n");
    assert (row_dim == adder->row_dim);
    assert (col_dim == adder->col_dim);
    return (SparseMatrix *) 0;
}

SparseMatrix * SparseMatrix::Matrix_substract (SparseMatrix * suber)
{
    // TODO
    printf ("Matrix_substract\n");
    assert (row_dim == suber->row_dim);
    assert (col_dim == suber->col_dim);
    return (SparseMatrix *) 0;
}

SparseMatrix * SparseMatrix::Matrix_multiple (SparseMatrix * multier)
{
    // TODO
    //printf ("Matrix_multiple\n");
    assert (col_dim == multier->row_dim);
    return (SparseMatrix *) 0;
}


// matrix vector multiplication
void SparseMatrix::Matrix_vector_multiple (double * output, int osize, double * input, int isize)
{
    //printf ("Matrix_vector_multiple\n");
    assert (col_dim == isize);
    assert (row_dim == osize);
    for (int i = 0; i < col_dim; i ++)
        output[i] = 0.0;
    for (int i = 0; i < row_dim; i ++)
    {
        for (int j = row_ptr[i]; j < row_ptr[i+1]; j ++)
        {
            output[i] += input[col_list[j]] * value_list[j];
        }
    }
}

void SparseMatrix::print()
{
    printf ("current_size: %d\n", current_size);
    //for (int i = 0; i < current_size; i ++)
    //    printf ("%6d\t%6d\t\t%3.6f\n", row_list[i], col_list[i], value_list[i]);
    for (int i = 0; i < row_dim + 1; i ++)
        printf ("row_ptr[%6d] = %6d\n", i, row_ptr[i]);
}
