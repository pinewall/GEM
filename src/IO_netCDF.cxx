#include "IO_netCDF.h"
#include <assert.h>
#include <stdio.h>
#include <debug.h>

#define NC_CHECK(error_id) {IO_netCDF::Check_NC_Error(error_id);}

IO_netCDF::IO_netCDF (int _dim_set_size, int _var_set_size, int _global_prep_size)
{
    dim_set_size = _dim_set_size;
    var_set_size = _var_set_size;
    global_prep_size = _global_prep_size;
    current_dim_set_size = 0;
    current_var_set_size = 0;
    current_global_prep_size = 0;
    dim_set = new Dim [dim_set_size];
    var_set = new Var [dim_set_size];
    global_prep = new Prep [global_prep_size];
}

IO_netCDF::IO_netCDF (char * config_filename)
{
    // TODO
    //IO_netCDF * cdf = new IO_netCDF (8, 19);
    dim_set_size = 8;
    var_set_size = 19;
    global_prep_size = 7;
    current_dim_set_size = 0;
    current_var_set_size = 0;
    current_global_prep_size = 0;
    dim_set = new Dim [dim_set_size];
    var_set = new Var [var_set_size];
    global_prep = new Prep [global_prep_size];
    this->Add_new_dim (new _Dim ("src_grid_size", "src_size"));
    this->Add_new_dim (new _Dim ("dst_grid_size", "dst_size"));
    this->Add_new_dim (new _Dim ("src_grid_corners", "src_corner_size"));
    this->Add_new_dim (new _Dim ("dst_grid_corners", "dst_corner_size"));
    this->Add_new_dim (new _Dim ("src_grid_rank", "src_rank"));
    this->Add_new_dim (new _Dim ("dst_grid_rank", "dst_rank"));
    this->Add_new_dim (new _Dim ("num_links", "num_links", CHANGE_DATA, TODO));
    this->Add_new_dim (new _Dim ("num_wgts", "num_wgts", CHANGE_DATA, TODO));

    Dim dim_array[2];

    //printf ("INT: %d\n", INT);
    //printf ("DOUBLE: %d\n", DOUBLE);
    Prep prep_units = new _Prep ("units");
    dim_array[0] = this->Get_dim_by_gname ("src_rank");
    this->Add_new_var (new _Var ("src_grid_dims", "src_dims", 1, dim_array, INT));

    dim_array[0] = this->Get_dim_by_gname ("dst_rank");
    this->Add_new_var (new _Var ("dst_grid_dims", "dst_dims", 1, dim_array, INT));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    this->Add_new_var (new _Var ("src_grid_center_lat", "src_center_lat", 1, dim_array, 1, &prep_units, DOUBLE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    this->Add_new_var (new _Var ("dst_grid_center_lat", "dst_center_lat", 1, dim_array, 1, &prep_units, DOUBLE));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    this->Add_new_var (new _Var ("src_grid_center_lon", "src_center_lon", 1, dim_array, 1, &prep_units, DOUBLE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    this->Add_new_var (new _Var ("dst_grid_center_lon", "dst_center_lon", 1, dim_array, 1, &prep_units, DOUBLE));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    dim_array[1] = this->Get_dim_by_gname ("src_corner_size");
    this->Add_new_var (new _Var ("src_grid_corner_lat", "src_corner_lat", 2, dim_array, 1, &prep_units, DOUBLE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    dim_array[1] = this->Get_dim_by_gname ("dst_corner_size");
    this->Add_new_var (new _Var ("dst_grid_corner_lat", "dst_corner_lat", 2, dim_array, 1, &prep_units, DOUBLE));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    dim_array[1] = this->Get_dim_by_gname ("src_corner_size");
    this->Add_new_var (new _Var ("src_grid_corner_lon", "src_corner_lon", 2, dim_array, 1, &prep_units, DOUBLE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    dim_array[1] = this->Get_dim_by_gname ("dst_corner_size");
    this->Add_new_var (new _Var ("dst_grid_corner_lon", "dst_corner_lon", 2, dim_array, 1, &prep_units, DOUBLE));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    this->Add_new_var (new _Var ("src_grid_imask", "src_mask", 1, dim_array, 1, &prep_units, INT));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    this->Add_new_var (new _Var ("dst_grid_imask", "dst_mask", 1, dim_array, 1, &prep_units, INT));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    this->Add_new_var (new _Var ("src_grid_area", "src_area", 1, dim_array, 1, &prep_units, DOUBLE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    this->Add_new_var (new _Var ("dst_grid_area", "dst_area", 1, dim_array, 1, &prep_units, DOUBLE));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    this->Add_new_var (new _Var ("src_grid_frac", "src_frac", 1, dim_array, 1, &prep_units, DOUBLE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    this->Add_new_var (new _Var ("dst_grid_frac", "dst_frac", 1, dim_array, 1, &prep_units, DOUBLE));

    dim_array[0] = this->Get_dim_by_gname ("num_links");
    this->Add_new_var (new _Var ("src_address", "src_address", 1, dim_array, INT, CHANGE_DATA, TODO));

    dim_array[0] = this->Get_dim_by_gname ("num_links");
    this->Add_new_var (new _Var ("dst_address", "dst_address", 1, dim_array, INT, CHANGE_DATA, TODO));

    dim_array[0] = this->Get_dim_by_gname ("num_links");
    dim_array[1] = this->Get_dim_by_gname ("num_wgts");
    this->Add_new_var (new _Var ("remap_matrix", "remap_matrix", 2, dim_array, DOUBLE, CHANGE_DATA, TODO));

    // add global preps
    this->Add_new_global_prep (new _Prep ("title"));
    this->Add_new_global_prep (new _Prep ("normalization"));
    this->Add_new_global_prep (new _Prep ("map_method"));
    this->Add_new_global_prep (new _Prep ("history"));
    this->Add_new_global_prep (new _Prep ("conventions"));
    this->Add_new_global_prep (new _Prep ("source_grid"));
    this->Add_new_global_prep (new _Prep ("dest_grid"));

    //return cdf;
}

IO_netCDF::~IO_netCDF ()
{
    // note: how to delete struct ?
    for (int i = 0; i < dim_set_size; i ++)
        delete dim_set[i];
    for (int i = 0; i < var_set_size; i ++)
        delete var_set[i];
    for (int i = 0; i < global_prep_size; i ++)
        delete global_prep[i];
    delete [] dim_set;
    delete [] var_set;
    delete [] global_prep;
}

void IO_netCDF::Add_new_dim (Dim dim)
{
    //printf("%d    %d\n", current_dim_set_size, dim_set_size);
    assert (current_dim_set_size < dim_set_size);
    dim_set[current_dim_set_size++] = new _Dim (dim);
}

void IO_netCDF::Add_new_var (Var var)
{
    assert (current_var_set_size < var_set_size);
    var_set[current_var_set_size++] = new _Var (var);
}

void IO_netCDF::Add_new_global_prep (Prep prep)
{
    assert (current_global_prep_size < global_prep_size);
    global_prep[current_global_prep_size++] = new _Prep (prep);
}
void IO_netCDF::Print_dims ()
{
    Dim dim;
    for (int i = 0; i < dim_set_size; i ++)
    {
        dim = dim_set[i];
        printf("Dim %d: \n\tname: %s\n\tgname: %s\n\taction: %d\n\tstate: %d\n", i, dim->name, dim->gname, dim->action, dim->state);
    }
}

void IO_netCDF::Print_vars ()
{   
    Prep prep;
    Var var;
    Dim dim;
    for (int i = 0; i < var_set_size; i ++)
    {
        var = var_set[i];
        printf("Var %d: \n\tname: %s\n\tgname: %s\n\tdim_size: %d\n", i, var->name, var->gname, var->dim_size);
        printf("\tdim_list: \n");
        for (int j = 0; j < var->dim_size; j ++)
        {
            dim = dim_set[j];
            printf("\t\tdim %d: \n\t\t\tname: %s\n\t\t\tgname: %s\n\t\t\taction: %d\n\t\t\tstate: %d\n", j, dim->name, dim->gname, dim->action, dim->state);
        }
        printf ("\tprep_list: \n");
        for (int j = 0; j < var->prep_size; j ++)
        {
            prep = var->prep_list[j];
            printf ("\t\tprep %d: \n\t\t\tname: %s\n\t\t\tinfo: %s\n", j, prep->name, prep->info);
        }

        printf("\ttype: %d\n", var->type);
    }
}

void IO_netCDF::Print_global_preps ()
{
    Prep prep;
    for (int i = 0; i < global_prep_size; i ++)
    {
        prep = global_prep[i];
        printf ("Global prep %d: \n\tname: %s\n\tinfo: %s\n", i, prep->name, prep->info);
    }
}

void IO_netCDF::Read_file (char * netcdf_file)
{
    // TODO
    Prep prep;
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
        printf ("reading %s\n", var->name);
        NC_CHECK (nc_inq_varid (ncid, var->name, &varid));
        printf ("after reading %s\n", var->name);
        // get attributes of var
        for (int j = 0; j < var->prep_size; j ++)
        {
            prep = var->prep_list[j];
            NC_CHECK (nc_get_att_text (ncid, varid, prep->name, prep->info));
        }
        if (var->data != 0)
            delete var->data;
        if (var->type == INT)
        {
            int * tmp = new int [Calculate_all_dims_of_var (var)];
            NC_CHECK (nc_get_var_int (ncid, varid, tmp));
            var->data = tmp;
            //printf ("ID %d: %d\n", i, ((int *)var->data)[0]);
        }
        else if (var->type == DOUBLE)
        {
            double * tmp = new double [Calculate_all_dims_of_var (var)];
            NC_CHECK (nc_get_var_double (ncid, varid, tmp));
            var->data = tmp;
            //printf ("ID %d: %3.6f\n", i, ((double *)var->data)[0]);
        }
    }

    // read all global attributes
    for (int i = 0; i < global_prep_size; i ++)
    {
        prep = global_prep[i];
        NC_CHECK (nc_get_att_text (ncid, NC_GLOBAL, prep->name, prep->info));
    }
    NC_CHECK (nc_close (ncid));
}

Dim IO_netCDF::Get_dim_by_gname (const char * dim_gname)
{
    for (int i = 0; i < dim_set_size; i ++)
        if (strcmp (dim_gname, dim_set[i]->gname) == 0)
            return dim_set[i];
    return (Dim) 0;
}

Var IO_netCDF::Get_var_by_gname (const char * var_gname)
{
    for (int i = 0; i < var_set_size; i ++)
        if (strcmp (var_gname, var_set[i]->gname) == 0)
            return var_set[i];
    return (Var) 0;
}

bool IO_netCDF::Check_dimlist_of_var (Var var)
{
    Dim dim;
    for (int i = 0; i < var->dim_size; i ++)
    {
        dim = var->dim_list[i];
        if (dim->state == TODO)
            return false;
    }
    return true;
}

int IO_netCDF::Calculate_all_dims_of_var (Var var)
{
    int all_dims = 1;
    Dim dim;
    for (int i = 0; i < var->dim_size; i ++)
    {
        dim = var->dim_list[i];
        all_dims *= dim->data;
    }
    return all_dims;
}

void IO_netCDF::Modify_dim_name (const char * gname, char * new_name)
{
    Dim dim = Get_dim_by_gname (gname);
    strcpy (dim->name, new_name);
    
    // TODO: action and state change
    dim->state = DONE;
    dim->action = KEEP;
}

void IO_netCDF::Modify_dim_data (const char * gname, unsigned int new_data)
{
    Dim dim = Get_dim_by_gname (gname);
    dim->data = new_data;
    
    // TODO: action and state change
    dim->state = DONE;
    dim->action = KEEP;
}

void IO_netCDF::Modify_var_name (const char * gname, char * new_name)
{
    Var var = Get_var_by_gname (gname);
    strcpy (var->name, new_name);
    
    // TODO: action and state change
    var->state = DONE;
    var->action = KEEP;
}

void IO_netCDF::Modify_var_data (const char * gname, void * new_data)
{
    Var var = Get_var_by_gname (gname);
    // make sure we complete changes on dimensions
    assert (Check_dimlist_of_var (var));
    delete var->data;
    int var_all_dims = Calculate_all_dims_of_var (var);
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

void IO_netCDF::Modify_var_prep (const char * gname, const char * prep_name, char * new_prep_info)
{
    Var var = Get_var_by_gname (gname);
    int var_all_dims = Calculate_all_dims_of_var (var);
    assert (var->type == DOUBLE);
    assert (strcmp (var->prep_list[0]->name, prep_name) == 0);
    assert (strcmp (prep_name, "units") == 0);

    double * data = (double *) var->data;
    double rate = 1.0;
    if (strcmp (var->prep_list[0]->info, "degrees") == 0)
    {
        if (strcmp (new_prep_info, "radians") == 0)
            rate = 3.14159265359 / 180;
    }
    else if (strcmp (var->prep_list[0]->info, "radians") == 0)
    {
        if (strcmp (new_prep_info, "degrees") == 0)
            rate = 180 / 3.14159265359;
    }
    for (int i = 0; i < var_all_dims; i ++)
        data[i] *= rate;
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
    //printf ("create ncfile\n");
    NC_CHECK (nc_create (netcdf_file, NC_CLOBBER, &ncid));
    //printf ("enter redefine mode\n");
    //NC_CHECK (nc_redef (ncid));

    // define dims
    //printf ("define dims\n");
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
    //printf ("define vars\n");
    int dim_array[NETCDF_DIMLEN];
    nc_type vartype = NC_INT;
    int varcount = 0;
    for (int i = 0; i < var_set_size; i ++)
    {
        var = var_set[i];
        if (var->action == KEEP)
        {
            for (int j = 0; j < var->dim_size; j ++)
                dim_array[j] = var->dim_list[j]->data;
            if (var->type == INT)
                vartype = NC_INT;
            else if (var->type == DOUBLE)
                vartype = NC_DOUBLE;
            NC_CHECK (nc_def_var (ncid, var->name, vartype, var->dim_size, dim_array, &(varid[i])));

            if (var->prep_size > 0)
                NC_CHECK (nc_put_att_text (ncid, varid[i], var->prep_list[0]->name, strlen (var->prep_list[0]->info), var->prep_list[0]->info));
        }
    }
    for (int i = 0; i < global_prep_size; i ++)
        NC_CHECK (nc_put_att_text (ncid, NC_GLOBAL, global_prep[i]->name, strlen (global_prep[i]->info), global_prep[i]->info));
    //printf ("exit define mode\n");
    NC_CHECK (nc_enddef (ncid));

    // put vars
    //printf ("put vars\n");
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
