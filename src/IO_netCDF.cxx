#include "IO_netCDF.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
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

IO_netCDF::IO_netCDF (int convention, int token)
{
    // TODO
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

    Prep prep_units = new _Prep ("units");
    dim_array[0] = this->Get_dim_by_gname ("src_rank");
    this->Add_new_var (new _Var ("src_grid_dims", "src_dims", 1, dim_array, INT_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("dst_rank");
    this->Add_new_var (new _Var ("dst_grid_dims", "dst_dims", 1, dim_array, INT_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    this->Add_new_var (new _Var ("src_grid_center_lat", "src_center_lat", 1, dim_array, 1, &prep_units, DOUBLE_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    this->Add_new_var (new _Var ("dst_grid_center_lat", "dst_center_lat", 1, dim_array, 1, &prep_units, DOUBLE_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    this->Add_new_var (new _Var ("src_grid_center_lon", "src_center_lon", 1, dim_array, 1, &prep_units, DOUBLE_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    this->Add_new_var (new _Var ("dst_grid_center_lon", "dst_center_lon", 1, dim_array, 1, &prep_units, DOUBLE_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    dim_array[1] = this->Get_dim_by_gname ("src_corner_size");
    this->Add_new_var (new _Var ("src_grid_corner_lat", "src_corner_lat", 2, dim_array, 1, &prep_units, DOUBLE_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    dim_array[1] = this->Get_dim_by_gname ("dst_corner_size");
    this->Add_new_var (new _Var ("dst_grid_corner_lat", "dst_corner_lat", 2, dim_array, 1, &prep_units, DOUBLE_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    dim_array[1] = this->Get_dim_by_gname ("src_corner_size");
    this->Add_new_var (new _Var ("src_grid_corner_lon", "src_corner_lon", 2, dim_array, 1, &prep_units, DOUBLE_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    dim_array[1] = this->Get_dim_by_gname ("dst_corner_size");
    this->Add_new_var (new _Var ("dst_grid_corner_lon", "dst_corner_lon", 2, dim_array, 1, &prep_units, DOUBLE_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    this->Add_new_var (new _Var ("src_grid_imask", "src_mask", 1, dim_array, 1, &prep_units, INT_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    this->Add_new_var (new _Var ("dst_grid_imask", "dst_mask", 1, dim_array, 1, &prep_units, INT_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    this->Add_new_var (new _Var ("src_grid_area", "src_area", 1, dim_array, 1, &prep_units, DOUBLE_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    this->Add_new_var (new _Var ("dst_grid_area", "dst_area", 1, dim_array, 1, &prep_units, DOUBLE_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("src_size");
    this->Add_new_var (new _Var ("src_grid_frac", "src_frac", 1, dim_array, 1, &prep_units, DOUBLE_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("dst_size");
    this->Add_new_var (new _Var ("dst_grid_frac", "dst_frac", 1, dim_array, 1, &prep_units, DOUBLE_TYPE));

    dim_array[0] = this->Get_dim_by_gname ("num_links");
    this->Add_new_var (new _Var ("src_address", "src_address", 1, dim_array, INT_TYPE, CHANGE_DATA, TODO));

    dim_array[0] = this->Get_dim_by_gname ("num_links");
    this->Add_new_var (new _Var ("dst_address", "dst_address", 1, dim_array, INT_TYPE, CHANGE_DATA, TODO));

    dim_array[0] = this->Get_dim_by_gname ("num_links");
    dim_array[1] = this->Get_dim_by_gname ("num_wgts");
    this->Add_new_var (new _Var ("remap_matrix", "remap_matrix", 2, dim_array, DOUBLE_TYPE, CHANGE_DATA, TODO));

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

IO_netCDF::IO_netCDF (const char * xml_meta)
{
    XMLElement * cdf = new XMLElement (xml_meta);
    XMLElement * dimension_list         = cdf->getChildren ()[0];
    XMLElement * variable_list          = cdf->getChildren ()[1];
    XMLElement * global_attribute_list  = cdf->getChildren ()[2];
    int ndim, nvar, natts;
    ndim    = dimension_list->getNumberOfChildren ();
    nvar    = variable_list->getNumberOfChildren ();
    natts   = global_attribute_list->getNumberOfChildren ();

    dim_set_size = 0;
    var_set_size = 0;
    global_prep_size = 0;
    current_dim_set_size = 0;
    current_var_set_size = 0;
    current_global_prep_size = 0;

    // initialize units with tag KEEP/CHANGE_NAME/CHANGE_DATA
    // dimensions
    for (int i = 0; i < ndim; i ++)
    {
        if (strcmp (dimension_list->getChildren ()[i]->getChildren ()[3]->getElementName (), "DESERT") == 0)
            continue;
        else
            dim_set_size ++;
    }
    // variables
    for (int i = 0; i < nvar; i ++)
    {
        if (strcmp (variable_list->getChildren ()[i]->getChildren ()[6]->getElementText (), "DESERT") == 0)
            continue;
        else
            var_set_size ++;
    }
    // global attributes
    for (int i = 0; i < natts; i ++)
    {
        if (strcmp (global_attribute_list->getChildren ()[i]->getChildren ()[3]->getElementName (), "DESERT") == 0)
            continue;
        else
            global_prep_size ++;
    }
    //printf ("<Constructor> dim_set_size: %d\n", dim_set_size);
    //printf ("<Constructor> var_set_size: %d\n", var_set_size);
    //printf ("<Constructor> global_prep_size: %d\n", global_prep_size);
    // allocate
    dim_set = new Dim [dim_set_size];
    var_set = new Var [var_set_size];
    global_prep = new Prep [global_prep_size];

    int iaction;
    int istate;
    int itype;
    XMLElement * node;
    XMLElement * subnode;
    XMLElement * subsubnode;
    Dim * dim_array;
    Prep * prep_array;

    // fill dimensions
    for (int i = 0; i < dim_set_size; i ++)
    {
        node = dimension_list->getChildren ()[i];
        String_to_enum (node->getChildren () [3]->getElementText (), &iaction);
        String_to_enum (node->getChildren () [4]->getElementText (), &istate);
        if ((Action) iaction != DESERT)
        {
            String_to_enum (node->getChildren () [3]->getElementText (), &iaction);
            String_to_enum (node->getChildren () [4]->getElementText (), &istate);
            dim_set[current_dim_set_size] = new _Dim (  node->getChildren ()[0]->getElementText (),
                                                        node->getChildren ()[1]->getElementText (),
                                                        (Action) iaction, (State) istate);
            current_dim_set_size ++;
        }
    }

    // fill variables
    int current_ndim;
    int current_natts;
    Dim current_Dim;
    Prep current_Prep;
    for (int i = 0; i < var_set_size; i ++)
    {
        node = variable_list->getChildren ()[i];
        current_ndim = 0;
        current_natts = 0;
        String_to_enum (node->getChildren () [2]->getElementText (), &itype);
        String_to_enum (node->getChildren () [7]->getElementText (), &iaction);
        String_to_enum (node->getChildren () [8]->getElementText (), &istate);
        if ((Action) iaction != DESERT)
        {
            subnode = node->getChildren () [3];             /* ndim */
            sscanf (subnode->getElementText (), "%d", &ndim);    
            subnode = node->getChildren () [4];             /** dimensions_of_variable **/
            dim_array = new Dim[ndim];
            for (int j = 0; j < ndim; j ++)
            {
                subsubnode = subnode->getChildren ()[j];
                current_Dim = Get_dim_by_name (subsubnode->getElementText ());
                if (current_Dim != (Dim) 0 && current_Dim->action != DESERT)
                    dim_array[current_ndim++] = current_Dim;
            }
            subnode = node->getChildren () [5];             /* natts */
            sscanf (subnode->getElementText (), "%d", &natts);
            subnode = node->getChildren () [6];             /** attributes_of_variable **/
            if (natts != 0)
                prep_array = new Prep[natts];
            for (int j = 0; j < natts; j ++)
            {
                subsubnode = subnode->getChildren ()[j];    /* attribute */ 
                String_to_enum (subsubnode->getChildren () [2]->getElementText (), &iaction);   /* action */
                String_to_enum (subsubnode->getChildren () [3]->getElementText (), &istate);    /* state */
                if ((Action) iaction != DESERT)
                    prep_array[current_natts++] = new _Prep (subsubnode->getChildren ()[0]->getElementText (), (Action) iaction, (State) istate);
            }

            var_set[current_var_set_size] = new _Var (  node->getChildren ()[0]->getElementText (),
                                                        node->getChildren ()[1]->getElementText (),
                                                        current_ndim, dim_array,
                                                        current_natts, prep_array,
                                                        (Data_Type) itype, (Action) iaction, (State) istate);
            current_var_set_size ++;
            delete dim_array;
            if (natts != 0)
                delete prep_array;
        }
    }

    // fill global attributes
    for (int i = 0; i < global_prep_size; i ++)
    {
        node = global_attribute_list->getChildren ()[i];                        /* global_attribute */
        subnode = node->getChildren ()[0];                                          /* key */
        String_to_enum (node->getChildren () [2]->getElementText (), &iaction);     /* action */
        String_to_enum (node->getChildren () [3]->getElementText (), &istate);      /* state */
        if ((Action) iaction != DESERT)
        {
            global_prep[current_global_prep_size] = new _Prep (subnode->getElementText (), (Action) iaction, (State) istate);
            current_global_prep_size ++;
            //printf ("<Write_file>: %s <%d> <%d>\n", subnode->getElementText (), iaction, istate);
        }
    }

    delete cdf;
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
    dim_set[current_dim_set_size++] = dim;
}

void IO_netCDF::Add_new_var (Var var)
{
    assert (current_var_set_size < var_set_size);
    var_set[current_var_set_size++] = var;
}

void IO_netCDF::Add_new_global_prep (Prep prep)
{
    assert (current_global_prep_size < global_prep_size);
    global_prep[current_global_prep_size++] = prep;
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

// grid_center_lat, grid_center_lon [should be] double
// grid_mask [should be] int
void IO_netCDF::Special_case ()
{
    Var var;
    Dim dim;
    dim = Get_dim_by_gname ("grid_size");
    size_t size = dim->data;
    //printf ("Special_case:: grid_size: %d\n", size);

    for (int i = 0; i < var_set_size; i ++)
    {
        var = var_set[i];
        if (strcmp (var->gname, "grid_imask") == 0)
        {
            if (var->type == INT_TYPE)
                continue;
            else
            {
                int * imask = new int [size];
                // float ==> int (default)
                float * fmask = (float *) var->data;
                for (int j = 0; j < size; j ++)
                    imask[j] = (fmask[j] > 0.5) ? 1 : 0;
                var->type = INT_TYPE;
                delete var->data;
                var->data = imask;
            }
        }
        else if (strcmp (var->gname, "grid_center_lat") == 0 || 
                 strcmp (var->gname, "grid_center_lon") == 0 ||
                 strcmp (var->gname, "physical_variable") == 0)
        {
            if (var->type == DOUBLE_TYPE)
                continue;
            else
            {
                double * dcenter = new double [size];
                // float ==> double (default)
                float * fcenter = (float *) var->data;
                for (int j = 0; j < size; j ++)
                    dcenter[j] = (double) fcenter[j];
                var->type = DOUBLE_TYPE;
                delete var->data;
                var->data = dcenter;
            }
        }
    }
}

void IO_netCDF::Signiture (const char * author, const char * date, const char * grid_name)
{
    Prep prep;
    for (int i = 0; i < global_prep_size; i ++)
    {
        prep = global_prep[i];
        
        if (strcmp (prep->name, "title") == 0)  // use variable name as title
        {
            strcpy (prep->info, "phyiscal variable: ");
            strcat (prep->info, Get_var_by_gname ("physical_variable")->name);
        }
        else if (strcmp (prep->name, "description") == 0)
        {
            strcpy (prep->info, "grid name: ");
            strcat (prep->info, grid_name);
        }
        else if (strcmp (prep->name, "history") == 0)
        {
            strcpy (prep->info, "Created on ");
            strcat (prep->info, date);
            strcat (prep->info, " by ");
            strcat (prep->info, author);
        }
        else if (strcmp (prep->name, "Conventions") == 0)
        {
            strcpy (prep->info, "Tsinghua U");
        }
    }
}

void IO_netCDF::Read_file (char * netcdf_file)
{
    // TODO
    Prep prep;
    Dim dim;
    Var var;
    int ncid, dimid, varid;
    nc_type att_type;
    int i_value;
    float f_value;
    double d_value;

    NC_CHECK( nc_open (netcdf_file, NC_NOWRITE, &ncid));

    // read all dims
    for (int i = 0; i < dim_set_size; i ++)
    {
        dim = dim_set[i];
        NC_CHECK (nc_inq_dimid (ncid, dim->name, &dimid));
        NC_CHECK (nc_inq_dimlen (ncid, dimid, &(dim->data)));
		printf("dimlen(%s): %ld\n", dim->name, dim->data);
    }

    // read all variables
    for (int i = 0; i < var_set_size; i ++)
    {
        var = var_set[i];
        //printf ("reading %s\n", var->name);
        NC_CHECK (nc_inq_varid (ncid, var->name, &varid));
        //printf ("after reading %s\n", var->name);
        // get attributes of var
        for (int j = 0; j < var->prep_size; j ++)
        {
            prep = var->prep_list[j];
            NC_CHECK (nc_inq_atttype (ncid, varid, prep->name, &att_type));
            if (att_type == NC_INT)
            {
                NC_CHECK (nc_get_att_int (ncid, varid, prep->name, &i_value));
                sprintf (prep->info, "%d", i_value);
            }
            else if (att_type == NC_FLOAT)
            {
                NC_CHECK (nc_get_att_float (ncid, varid, prep->name, &f_value));
                sprintf (prep->info, "%1.0e", f_value);
            }
            else if (att_type == NC_DOUBLE)
            {
                NC_CHECK (nc_get_att_double (ncid, varid, prep->name, &d_value));
                sprintf (prep->info, "%1.0e", d_value);
            }
            else
                NC_CHECK (nc_get_att_text (ncid, varid, prep->name, prep->info));
        }
        if (var->data != 0)
            delete var->data;
        if (var->type == INT_TYPE)
        {
            int * tmp = new int [Calculate_all_dims_of_var (var)];
            NC_CHECK (nc_get_var_int (ncid, varid, tmp));
            var->data = tmp;
            //printf ("ID %d: %d\n", i, ((int *)var->data)[0]);
        }
        else if (var->type == FLOAT_TYPE)
        {
            float * tmp = new float [Calculate_all_dims_of_var (var)];
            NC_CHECK (nc_get_var_float (ncid, varid, tmp));
            var->data = tmp;
            //printf ("ID %d: %3.6f\n", i, ((float *)var->data)[0]);
        }
        else if (var->type == DOUBLE_TYPE)
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
        NC_CHECK (nc_inq_atttype (ncid, NC_GLOBAL, prep->name, &att_type));
        if (att_type == NC_INT)
        {
            NC_CHECK (nc_get_att_int (ncid, NC_GLOBAL, prep->name, &i_value));
            sprintf (prep->info, "%d", i_value);
        }
        else if (att_type == NC_FLOAT)
        {
            NC_CHECK (nc_get_att_float (ncid, NC_GLOBAL, prep->name, &f_value));
            sprintf (prep->info, "%1.0e", f_value);
        }
        else if (att_type == NC_DOUBLE)
        {
            NC_CHECK (nc_get_att_double (ncid, NC_GLOBAL, prep->name, &d_value));
            sprintf (prep->info, "%1.0e", d_value);
        }
        else
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

Dim IO_netCDF::Get_dim_by_name (const char * dim_name)
{
    for (int i = 0; i < dim_set_size; i ++)
        if (strcmp (dim_name, dim_set[i]->name) == 0)
            return dim_set[i];
    return (Dim) 0;
}

int IO_netCDF::Get_dimID_by_name (const char * dim_name)
{
    for (int i = 0; i < dim_set_size; i ++)
        if (strcmp (dim_name, dim_set[i]->name) == 0)
            return i;
    return -1;
}

Var IO_netCDF::Get_var_by_name (const char * var_name)
{
    for (int i = 0; i < var_set_size; i ++)
        if (strcmp (var_name, var_set[i]->name) == 0)
            return var_set[i];
    return (Var) 0;
}

int IO_netCDF::Get_varID_by_name (const char * var_name)
{
    for (int i = 0; i < var_set_size; i ++)
        if (strcmp (var_name, var_set[i]->name) == 0)
            return i;
    return -1;
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

size_t IO_netCDF::Calculate_all_dims_of_var (Var var)
{
    size_t all_dims = 1;
    Dim dim;
    for (int i = 0; i < var->dim_size; i ++)
    {
        dim = var->dim_list[i];
        all_dims *= dim->data;
    }
    return all_dims;
}

void IO_netCDF::Modify_dim_name (const char * gname, const char * new_name)
{
    Dim dim = Get_dim_by_gname (gname);
    strcpy (dim->name, new_name);
    
    // TODO: action and state change
    dim->state = DONE;
    dim->action = KEEP;
}

void IO_netCDF::Modify_dim_data (const char * gname, size_t new_data)
{
    Dim dim = Get_dim_by_gname (gname);
    dim->data = new_data;
    
    // TODO: action and state change
    dim->state = DONE;
    dim->action = KEEP;
}

void IO_netCDF::Modify_var_name (const char * gname, const char * new_name)
{
    Var var = Get_var_by_gname (gname);
    strcpy (var->name, new_name);
    
    // TODO: action and state change
    var->state = DONE;
    var->action = KEEP;
}

void IO_netCDF::Modify_var_data (const char * gname, const void * new_data)
{
    Var var = Get_var_by_gname (gname);
    // make sure we complete changes on dimensions
    assert (Check_dimlist_of_var (var));
    delete var->data;
    size_t var_all_dims = Calculate_all_dims_of_var (var);
    if (var->type == INT_TYPE)
        var_all_dims *= sizeof (int);
    else if (var->type == FLOAT_TYPE)
        var_all_dims *= sizeof (float);
    else if (var->type == DOUBLE_TYPE)
        var_all_dims *= sizeof (double);
    var->data = new char [var_all_dims];
    // make sure memory allocation success
    assert (var->data != (char *) 0);
    memcpy (var->data, new_data, var_all_dims);
    
    // TODO: action and state change
    var->state = DONE;
    var->action = KEEP;
}

void IO_netCDF::Modify_var_prep (const char * gname, const char * key, const char * new_value)
{
    Var var = Get_var_by_gname (gname);
    size_t var_all_dims = Calculate_all_dims_of_var (var);
    assert (var->type == DOUBLE_TYPE || var->type == FLOAT_TYPE);
    assert (strcmp (var->prep_list[0]->name, key) == 0);
    assert (strcmp (key, "units") == 0);

    double * data = (double *) var->data;
    double rate = 1.0;
    if (strcmp (var->prep_list[0]->info, "degrees") == 0)
    {
        if (strcmp (new_value, "radians") == 0)
            rate = 3.14159265359 / 180;
    }
    else if (strcmp (var->prep_list[0]->info, "radians") == 0)
    {
        if (strcmp (new_value, "degrees") == 0)
            rate = 180 / 3.14159265359;
    }
    for (int i = 0; i < var_all_dims; i ++)
        data[i] *= rate;
    // TODO: action and state change
    var->state = DONE;
    var->action = KEEP;
}

void Modify_prep_key (const char * key, const char * new_key)
{
}

void Modify_prep_value(const char * key, const char * new_value)
{
}

void IO_netCDF::Write_file (char * netcdf_file)
{
    int ncid;
    int * dimid = new int [dim_set_size];
    int * varid = new int [var_set_size];
    Dim dim;
    Var var;
    Prep prep;
    int i_value;
    float f_value;
    double d_value;

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
            NC_CHECK (nc_def_dim (ncid, dim->gname, dim->data, &(dimid[i])));
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
                dim_array[j] = dimid [ Get_dimID_by_name (var->dim_list[j]->name) ];
            if (var->type == INT_TYPE)
                vartype = NC_INT;
            else if (var->type == FLOAT_TYPE)
                vartype = NC_FLOAT;
            else if (var->type == DOUBLE_TYPE)
                vartype = NC_DOUBLE;
            NC_CHECK (nc_def_var (ncid, var->gname, vartype, var->dim_size, dim_array, &(varid[i])));

            if (var->prep_size > 0)
                for (int j = 0; j < var->prep_size; j ++)
                {
                    prep = var->prep_list[j];
                    if (prep->action != KEEP)
                        continue;
                    if (strcmp (prep->info, "_FillValue") == 0 || strcmp (prep->info, "missing_value") == 0)
                    {
                        if (var->type == INT_TYPE)
                        {
                            sscanf (prep->info, "%d", &i_value);
                            NC_CHECK (nc_put_att_int (ncid, varid[i], prep->name, NC_INT, 1, &i_value));
                        }
                        else if (var->type == FLOAT_TYPE)
                        {
                            sscanf (prep->info, "%f", &f_value);
                            NC_CHECK (nc_put_att_float (ncid, varid[i], prep->name, NC_FLOAT, 1, &f_value));
                        }
                        else if (var->type == DOUBLE_TYPE)
                        {
                            sscanf (prep->info, "%lf", &d_value);
                            NC_CHECK (nc_put_att_double (ncid, varid[i], prep->name, NC_DOUBLE, 1, &d_value));
                        }
                    }
                    if (prep->type == TEXT_TYPE)
                    {
                        NC_CHECK (nc_put_att_text (ncid, varid[i], prep->name, strlen (prep->info), prep->info));
                    }
                    else if (prep->type == INT_TYPE)
                    {
                        sscanf (prep->info, "%d", &i_value);
                        NC_CHECK (nc_put_att_int (ncid, varid[i], prep->name, NC_INT, 1, &i_value));
                    }
                    else if (prep->type == FLOAT_TYPE)
                    {
                        sscanf (prep->info, "%f", &f_value);
                        NC_CHECK (nc_put_att_float (ncid, varid[i], prep->name, NC_FLOAT, 1, &f_value));
                    }
                    else if (prep->type == DOUBLE_TYPE)
                    {
                        sscanf (prep->info, "%lf", &d_value);
                        NC_CHECK (nc_put_att_double (ncid, varid[i], prep->name, NC_DOUBLE, 1, &d_value));
                    }
                }
        }
    }

    //printf ("define global attributes\n");
    for (int i = 0; i < global_prep_size; i ++)
    {
        prep = global_prep[i];
        if (prep->action != KEEP)
            continue;
        if (prep->type == TEXT_TYPE)
        {
            NC_CHECK (nc_put_att_text (ncid, NC_GLOBAL, prep->name, strlen (prep->info), prep->info));
            //printf ("new global attribute!\n");
        }
        else if (prep->type == INT_TYPE)
        {
            sscanf (prep->info, "%d", &i_value);
            NC_CHECK (nc_put_att_int (ncid, NC_GLOBAL, prep->name, NC_INT, 1, &i_value));
        }
        else if (prep->type == FLOAT_TYPE)
        {
            sscanf (prep->info, "%f", &f_value);
            NC_CHECK (nc_put_att_float (ncid, NC_GLOBAL, prep->name, NC_FLOAT, 1, &f_value));
        }
        else if (prep->type == DOUBLE_TYPE)
        {
            sscanf (prep->info, "%lf", &d_value);
            NC_CHECK (nc_put_att_double (ncid, NC_GLOBAL, prep->name, NC_DOUBLE, 1, &d_value));
        }
    }
    NC_CHECK (nc_enddef (ncid));
    //printf ("exit define mode\n");

    // put vars
    //printf ("put vars\n");
    for (int i = 0; i < var_set_size; i ++)
    {
        var = var_set[i];
        if (var->action == KEEP)
        {
            //NC_CHECK (nc_inq_varid(ncid, var->name, &varid));
            if (var->type == INT_TYPE)
            {
                NC_CHECK (nc_put_var_int (ncid, varid[i], (int *)(var->data)));
            }
            else if (var->type == FLOAT_TYPE)
            {
                NC_CHECK (nc_put_var_float (ncid, varid[i], (float *)(var->data)));
            }
            else if (var->type == DOUBLE_TYPE)
            {
                NC_CHECK (nc_put_var_double (ncid, varid[i], (double *)(var->data)));
            }
        }
    }
    NC_CHECK (nc_close (ncid));
    //printf ("close file\n");
}
