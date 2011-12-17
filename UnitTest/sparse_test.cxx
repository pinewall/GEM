#include <SParseMatrix.h>
#include <stdio.h>

/*
 *      3 0 1 2 0 0 4
 *      0 1 0 0 1 9 2
 *      2 0 8 0 0 0 1
 *      0 1 0 1 2 0 0
 *      1 2 0 0 1 0 0
 *      1 1 0 0 0 9 8
 *      0 0 0 0 0 0 1
 */
int main()
{
    // Add entry mode
    SparseMatrix * test = new SparseMatrix (7, 7);
    test->Add_entry (0, 0, 3);
    test->Add_entry (0, 2, 1);
    test->Add_entry (0, 3, 2);
    test->Add_entry (0, 6, 4);
    test->Add_entry (1, 1, 1);
    test->Add_entry (1, 4, 1);
    test->Add_entry (1, 5, 9);
    test->Add_entry (1, 6, 2);
    test->Add_entry (2, 0, 2);
    test->Add_entry (2, 2, 8);
    test->Add_entry (2, 6, 1);
    test->Add_entry (3, 1, 1);
    test->Add_entry (3, 3, 1);
    test->Add_entry (3, 4, 2);
    test->Add_entry (4, 0, 1);
    test->Add_entry (4, 1, 2);
    test->Add_entry (4, 4, 1);
    test->Add_entry (5, 0, 1);
    test->Add_entry (5, 1, 1);
    test->Add_entry (5, 5, 9);
    test->Add_entry (5, 6, 8);
    test->Add_entry (6, 6, 1);
    test->Complete_row_ptr ();

    double * vec = new double [7];
    double * res = new double [7];
    double real[] = {10.0, 13.0, 11.0, 4.0, 4.0, 19.0, 1.0};
    for (int i = 0; i < 7; i ++)
        vec[i] = 1;
    test->Matrix_vector_multiple (res, 7, vec, 7);
    for (int i = 0; i < 7; i ++)
        printf ("res[%d] = %f\t%f\n", i, res[i], real[i]);
    return 0;
}
