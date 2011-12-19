#include <stdio.h>
#include <math.h>

int main(int argc, char ** argv)
{
    void * array;
    double * tmp = new double [2];
    tmp[0] = 1;
    tmp[1] = 2;
    array = tmp;
    printf ("%f\n", ((double *)array)[0]);

    double angle = 30;
    printf ("sin(30) = %f\n", sin(angle));

    int list[2];
    list[0] = 128;
    list[1] = -55;
    printf ("double of two int (%d, %d) is %f\n", list[0], list[1], ((double *)list)[0]);

    for (int i = 0; i < argc; i ++)
        printf ("argv[%d] = %s\n", i, argv[i]);
    return 0;
}
