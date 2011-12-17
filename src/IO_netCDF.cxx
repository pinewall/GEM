#include "IO_netCDF.h"
#include <assert.h>
#include <stdio.h>

#define NC_CHECK(error_id) {IO_netCDF::Check_NC_Error(error_id);}

IO_netCDF::IO_netCDF (int _dim_set_size, int _var_set_size)
{
    dim_set_size = _dim_set_size;
    var_set_size = _var_set_size;
    current_dim_set_size = 0;
    current_var_set_size = 0;
    dim_set = new Dim [dim_set_size];
    var_set = new Var [dim_set_size];
}

IO_netCDF::IO_netCDF (char * config_filename)
{
    // TODO
    //IO_netCDF * cdf = new IO_netCDF (8, 19);
    dim_set_size = 8;
    var_set_size = 19;
    current_dim_set_size = 0;
    current_var_set_size = 0;
    dim_set = new Dim [dim_set_size];
    var_set = new Var [dim_set_size];
    this->Add_new_dim (new _Dim (0, "src_grid_size"));
    this->Add_new_dim (new _Dim (1, "dst_grid_size"));
    this->Add_new_dim (new _Dim (2, "src_grid_corners"));
    this->Add_new_dim (new _Dim (3, "dst_grid_corners"));
    this->Add_new_dim (new _Dim (4, "src_grid_rank"));
    this->Add_new_dim (new _Dim (5, "dst_grid_rank"));
    this->Add_new_dim (new _Dim (6, "num_links", CHANGE_DATA, TODO));
    this->Add_new_dim (new _Dim (7, "num_wgts", CHANGE_DATA, TODO));

    int dim_array[2];

    //printf ("INT: %d\n", INT);
    //printf ("DOUBLE: %d\n", DOUBLE);
    dim_array[0] = 4;
    this->Add_new_var (new _Var (0, "src_grid_dims", 1, dim_array, INT));

    dim_array[0] = 5;
    this->Add_new_var (new _Var (1, "dst_grid_dims", 1, dim_array, INT));

    dim_array[0] = 0;
    this->Add_new_var (new _Var (2, "src_grid_center_lat", 1, dim_array, DOUBLE));

    dim_array[0] = 1;
    this->Add_new_var (new _Var (3, "dst_grid_center_lat", 1, dim_array, DOUBLE));

    dim_array[0] = 0;
    this->Add_new_var (new _Var (4, "src_grid_center_lon", 1, dim_array, DOUBLE));

    dim_array[0] = 1;
    this->Add_new_var (new _Var (5, "dst_grid_center_lon", 1, dim_array, DOUBLE));

    dim_array[0] = 0; dim_array[1] = 2;
    this->Add_new_var (new _Var (6, "src_grid_corner_lat", 2, dim_array, DOUBLE));

    dim_array[0] = 1; dim_array[1] = 3;
    this->Add_new_var (new _Var (7, "dst_grid_corner_lat", 2, dim_array, DOUBLE));

    dim_array[0] = 0; dim_array[1] = 2;
    this->Add_new_var (new _Var (8, "src_grid_corner_lon", 2, dim_array, DOUBLE));

    dim_array[0] = 1; dim_array[1] = 3;
    this->Add_new_var (new _Var (9, "dst_grid_corner_lon", 2, dim_array, DOUBLE));

    dim_array[0] = 0;
    this->Add_new_var (new _Var (10, "src_grid_imask", 1, dim_array, INT));

    dim_array[0] = 1;
    this->Add_new_var (new _Var (11, "dst_grid_imask", 1, dim_array, INT));

    dim_array[0] = 0;
    this->Add_new_var (new _Var (12, "src_grid_area", 1, dim_array, DOUBLE));

    dim_array[0] = 1;
    this->Add_new_var (new _Var (13, "dst_grid_area", 1, dim_array, DOUBLE));

    dim_array[0] = 0;
    this->Add_new_var (new _Var (14, "src_grid_frac", 1, dim_array, DOUBLE));

    dim_array[0] = 1;
    this->Add_new_var (new _Var (15, "dst_grid_frac", 1, dim_array, DOUBLE));

    dim_array[0] = 6;
    this->Add_new_var (new _Var (16, "src_address", 1, dim_array, INT, CHANGE_DATA, TODO));

    dim_array[0] = 6;
    this->Add_new_var (new _Var (17, "dst_address", 1, dim_array, INT, CHANGE_DATA, TODO));

    dim_array[0] = 6; dim_array[1] = 7;
    this->Add_new_var (new _Var (18, "remap_matrix", 2, dim_array, DOUBLE, CHANGE_DATA, TODO));
    //return cdf;
}

IO_netCDF::~IO_netCDF ()
{
    // note: how to delete struct ?
    for (int i = 0; i < dim_set_size; i ++)
        delete dim_set[i];
    for (int i = 0; i < var_set_size; i ++)
        delete var_set[i];
    delete [] dim_set;
    delete [] var_set;
}

void IO_netCDF::Add_new_dim (Dim dim)
{
    //printf("%d    %d\n", current_dim_set_size, dim_set_size);
    assert (current_dim_set_size < dim_set_size);
    dim_set[current_dim_set_size++] = new _Dim(dim);
}

void IO_netCDF::Add_new_var (Var var)
{
    assert (current_var_set_size < var_set_size);
    var_set[current_var_set_size++] = new _Var(var);
}

void IO_netCDF::Print_dims ()
{
    Dim dim;
    for (int i = 0; i < dim_set_size; i ++)
    {
        dim = dim_set[i];
        printf("Dim %d: \n\tID: %d\n\tname: %s\n\taction: %d\n\tstate: %d\n", i, dim->ID, dim->name, dim->action, dim->state);
    }
}

void IO_netCDF::Print_vars ()
{   
    Var var;
    Dim dim;
    for (int i = 0; i < var_set_size; i ++)
    {
        var = var_set[i];
        printf("Var %d: \n\tID: %d\n\tname: %s\n\tdim_size: %d\n", i, var->ID, var->name, var->dim_size);
        printf("\tdim_list: \n");
        for (int j = 0; j < var->dim_size; j ++)
        {
            dim = dim_set[(var->dim_list)[j]];
            printf("\t\tdim %d: \n\t\t\tID: %d\n\t\t\tname: %s\n\t\t\taction: %d\n\t\t\tstate: %d\n", j, dim->ID, dim->name, dim->action, dim->state);
        }
        printf("\ttype: %d\n", var->type);
    }
}

void IO_netCDF::Read_file (char * netcdf_file)
{
    // TODO
    Dim dim;
    Var var;
    int ncid, dimid, varid;

    NC_CHECK( nc_open (netcdf_file, NC_NOWRITE, &ncid));

    // read all dims
    for (int i = 0; i < dim_set_size; i ++)
    {
        dim = dim_set[i];
        NC_CHECK (nc_inq_dimid (ncid, dim->name, &dimid));
        NC_CHECK (nc_inq_dimlen (ncid, dimid, &(dim->data)));
    }

    // read all variables
    for (int i = 0; i < var_set_size; i ++)
    {
        var = var_set[i];
        NC_CHECK (nc_inq_varid (ncid, var->name, &varid));
        if (var->data != 0)
            delete var->data;
        if (var->type == INT)
        {
            int * tmp = new int [Calculate_all_dims_of_var (i)];
            NC_CHECK (nc_get_var_int (ncid, varid, tmp));
            var->data = tmp;
            //printf ("ID %d: %d\n", i, ((int *)var->data)[0]);
        }
        else if (var->type == DOUBLE)
        {
            double * tmp = new double [Calculate_all_dims_of_var (i)];
            NC_CHECK (nc_get_var_double (ncid, varid, tmp));
            var->data = tmp;
            //printf ("ID %d: %3.6f\n", i, ((double *)var->data)[0]);
        }
    }
    NC_CHECK (nc_close (ncid));
}

Dim IO_netCDF::Get_dim_by_name (const char * dim_name)
{
    for (int i = 0; i < dim_set_size; i ++)
        if (strcmp (dim_name, dim_set[i]->name) == 0)
            return dim_set[i];
    return (Dim) 0;
}

Var IO_netCDF::Get_var_by_name (const char * var_name)
{
    for (int i = 0; i < var_set_size; i ++)
        if (strcmp (var_name, var_set[i]->name) == 0)
            return var_set[i];
    return (Var) 0;
}

Dim IO_netCDF::Get_dim_by_ID (int dimid)
{
    assert (dimid < dim_set_size);
    return dim_set[dimid];
}

Var IO_netCDF::Get_var_by_ID (int varid)
{
    assert (varid < var_set_size);
    return var_set[varid];
}

bool IO_netCDF::Check_dimlist_of_var (int var_id)
{
    Var var = var_set[var_id];
    Dim dim;
    for (int i = 0; i < var->dim_size; i ++)
    {
        dim = dim_set[(var->dim_list)[i]];
        if (dim->state == TODO)
            return false;
    }
    return true;
}

int IO_netCDF::Calculate_all_dims_of_var (int var_id)
{
    int all_dims = 1;
    Var var = var_set[var_id];
    Dim dim;
    for (int i = 0; i < var->dim_size; i ++)
    {
        dim = dim_set[(var->dim_list)[i]];
        all_dims *= dim->data;
    }
    return all_dims;
}

void IO_netCDF::Modify_dim_name (int dim_id, char * new_name)
{
    Dim dim = dim_set[dim_id];
    strcpy (dim->name, new_name);
    
    // TODO: action and state change
    dim->state = DONE;
    dim->action = KEEP;
}

void IO_netCDF::Modify_dim_data (int dim_id, unsigned int new_data)
{
    Dim dim = dim_set[dim_id];
    dim->data = new_data;
    
    // TODO: action and state change
    dim->state = DONE;
    dim->action = KEEP;
}

void IO_netCDF::Modify_var_name (int var_id, char * new_name)
{
    Var var = var_set[var_id];
    strcpy (var->name, new_name);
    
    // TODO: action and state change
    var->state = DONE;
    var->action = KEEP;
}

void IO_netCDF::Modify_var_data (int var_id, void * new_data)
{
    Var var = var_set[var_id];
    // make sure we complete changes on dimensions
    assert (Check_dimlist_of_var (var_id));
    delete var->data;
    int var_all_dims = Calculate_all_dims_of_var (var_id);
    if (var->type == INT)
        var_all_dims *= sizeof (int);
    else if (var->type == DOUBLE)
        var_all_dims *= sizeof (double);
    var->data = new char [var_all_dims];
    // make sure memory allocation success
    assert (var->data != (char *) 0);
    memcpy (var->data, new_data, var_all_dims);
    
    // TODO: action and state change
    var->state = DONE;
    var->action = KEEP;
}
void IO_netCDF::Write_file (char * netcdf_file)
{
    // TODO: to write out all dims or vars whose action == KEEP
    int ncid;
    int * dimid = new int [dim_set_size];
    int * varid = new int [var_set_size];
    Dim dim;
    Var var;
    printf ("create ncfile\n");
    NC_CHECK (nc_create (netcdf_file, NC_CLOBBER, &ncid));
    //printf ("enter redefine mode\n");
    //NC_CHECK (nc_redef (ncid));

    // define dims
    printf ("define dims\n");
    int dimcount = 0;
    for (int i = 0; i < dim_set_size; i ++)
    {
        dim = dim_set[i];
        if (dim->action == KEEP)
        {
            NC_CHECK (nc_def_dim (ncid, dim->name, dim->data, &(dimid[i])));
            dimcount ++;
        }
    }
    // define vars
    printf ("define vars\n");
    int dim_array[NETCDF_DIMLEN];
    nc_type vartype = NC_INT;
    int varcount = 0;
    for (int i = 0; i < var_set_size; i ++)
    {
        var = var_set[i];
        if (var->action == KEEP)
        {
            for (int j = 0; j < var->dim_size; j ++)
                dim_array[j] = dimid[var->dim_list[j]];
            if (var->type == INT)
                vartype = NC_INT;
            else if (var->type == DOUBLE)
                vartype = NC_DOUBLE;
            NC_CHECK (nc_def_var (ncid, var->name, vartype, var->dim_size, dim_array, &(varid[i])));
        }
    }
    printf ("exit define mode\n");
    NC_CHECK (nc_enddef (ncid));

    // put vars
    printf ("put vars\n");
    for (int i = 0; i < var_set_size; i ++)
    {
        var = var_set[i];
        if (var->action == KEEP)
        {
            //NC_CHECK (nc_inq_varid(ncid, var->name, &varid));
            if (var->type == INT)
            {
                NC_CHECK (nc_put_var_int (ncid, varid[i], (int *)var->data));
            }
            else if (var->type == DOUBLE)
            {
                NC_CHECK (nc_put_var_double (ncid, varid[i], (double *)var->data));
            }
        }
    }
    NC_CHECK (nc_close (ncid));
}
