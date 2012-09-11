#include <stdio.h>
#include <netcdf.h>
#include "xml_base.h"
#include "types.h"

#define NC_CHECK(error_id) {Check_NC_Error(error_id);}
inline void Check_NC_Error (int error_id)
{
    if (error_id != NC_NOERR)
        //fprintf(stderr, "NC operation failed!\n");
        fprintf(stderr, "%s\n", nc_strerror (error_id));
}

int main (int argc, char ** argv)
{
    if (argc < 3)
    {
        printf ("Usage: ./meta ncfile xmlfile\n");
        return -1;
    }
    
    int ncid, ndim, nvar, natts, unlimited_ndim;
    NC_CHECK (nc_open (argv[1], NC_NOWRITE, &ncid));
    NC_CHECK (nc_inq (ncid, &ndim, &nvar, &natts, &unlimited_ndim));

    /* root and its elements */
    XMLElement * root = new XMLElement ("netcdf", 3);
    XMLElement * dimension_list = new XMLElement ("dimension_list", ndim+1);
    //XMLElement * unlimited_dimension_list = new XMLElement ("unlimited_dimension_list", unlimited_ndim);
    XMLElement * variable_list = new XMLElement ("variable_list", nvar+1);
    XMLElement * global_attribute_list = new XMLElement ("global_attribute_list", natts+1);
    root->addElement (dimension_list);
    //root->addElement (unlimited_dimension_list);
    root->addElement (variable_list);
    root->addElement (global_attribute_list);

    /* dimensions */
    char dim_name [128];
    char str_dim_size [128];
    CDF_INT dim_size;
    XMLElement * node;
    XMLElement * subnode;
    XMLElement * subsubnode;
    //printf ("number of dims: %d\n", ndim);
    sprintf (str_dim_size, "%d", ndim);
    dimension_list->addElement (new XMLElement ("ndim", str_dim_size));
    for (int i = 0; i < ndim; i ++)
    {
        NC_CHECK (nc_inq_dim (ncid, i, dim_name, &dim_size));
        //printf ("\tdimname: %s\n", dim_name);
        //printf ("\tsize: %d\n", dim_size);
        dimension_list->addElement (new XMLElement ("dimension", 5));
        node = dimension_list->getChildren ()[i+1];
        node->addElement (new XMLElement ("name", dim_name));
        node->addElement (new XMLElement ("gname", dim_name));
        sprintf (str_dim_size, "%ld", dim_size);
        node->addElement (new XMLElement ("size", str_dim_size));
        node->addElement (new XMLElement ("action", "KEEP"));
        node->addElement (new XMLElement ("state", "DONE"));
    }

    /* unlimited dimensions */
    /*
    int unlimited_dimids[16];
    int num_of_unlimited_dims;
    printf ("number of unlimited dims: %d\n", unlimited_ndim);
    NC_CHECK (nc_inq_unlimdims (ncid, &num_of_unlimited_dims, unlimited_dimids));
    */
    /* variables */
    char var_name [XML_TEXTLEN];
    char str_var_size [XML_TEXTLEN];
    nc_type var_type;
    nc_type att_type;
    char str_tmp [XML_TEXTLEN];
    int var_ndim;
    int var_dimids [8];
    int var_natts;
    char str_key [XML_TEXTLEN];
    char str_value [XML_TEXTLEN];
    double d_value;
    float f_value;
    int i_value;
    CDF_INT len_str_value;
    //printf ("number of vars: %d\n", nvar);
    sprintf (str_var_size, "%d", nvar);
    variable_list->addElement (new XMLElement ("nvar", str_var_size));
    for (int i = 0; i < nvar; i ++)
    {
        NC_CHECK (nc_inq_var (ncid, i, var_name, &var_type, &var_ndim, var_dimids, &var_natts));
        //printf ("\tvarname: %s\n", var_name);
        //printf ("\tvartype: %d\n", var_type);
        //printf ("\tvarndim: %d\n", var_ndim);
        //printf ("\tvarnatts: %d\n", var_natts);
        variable_list->addElement (new XMLElement ("variable", 9));
        node = variable_list->getChildren ()[i+1];
        node->addElement (new XMLElement ("name", var_name));
        node->addElement (new XMLElement ("gname", var_name));

        if (var_type == NC_INT)
            strcpy (str_tmp, "INT_TYPE");
        else if (var_type == NC_FLOAT)
            strcpy (str_tmp, "FLOAT_TYPE");
        else if (var_type == NC_DOUBLE)
            strcpy (str_tmp, "DOUBLE_TYPE");
        node->addElement (new XMLElement ("type", str_tmp));

        sprintf (str_tmp, "%d", var_ndim);
        node->addElement (new XMLElement ("ndim", str_tmp));
        node->addElement (new XMLElement ("dimensions_of_variable", var_ndim));
        subnode = node->getChild ("dimensions_of_variable");
        char * str_name;
        for (int j = 0; j < var_ndim; j ++)
        {
            str_name = dimension_list->getChildren ()[var_dimids[j]+1]->getChild ("name") ->getElementText ();
            //printf ("\t\t%s\n", str_name);
            subnode->addElement (new XMLElement ("name", str_name));
        }

        sprintf (str_tmp, "%d", var_natts);
        node->addElement (new XMLElement ("natts", str_tmp));
        node->addElement (new XMLElement ("attributes_of_variable", var_natts));
        subnode = node->getChild ("attributes_of_variable");
        for (int j = 0; j < var_natts; j ++)
        {
            NC_CHECK (nc_inq_attname (ncid, i, j, str_key));
            NC_CHECK (nc_inq_att (ncid, i, str_key, &att_type, &len_str_value));
            if (att_type == NC_INT)
            {
                NC_CHECK (nc_get_att_int (ncid, i, str_key, &i_value));
                sprintf (str_value, "%d", i_value);
            }
            else if (att_type == NC_DOUBLE)
            {
                NC_CHECK (nc_get_att_double (ncid, i, str_key, &d_value));
                //sprintf (str_value, "%lf", d_value);
                sprintf (str_value, "%1.0ed", d_value);
            }
            else if (att_type == NC_FLOAT)
            {
                NC_CHECK (nc_get_att_float (ncid, i, str_key, &f_value));
                //sprintf (str_value, "%f", f_value);
                sprintf (str_value, "%1.0ef", f_value);
            }
            else
            {
                NC_CHECK (nc_get_att_text (ncid, i, str_key, str_value));
                str_value[len_str_value] = '\0';
            }
            //printf ("\t\tattribute_name: %s\n", str_key);
            //printf ("\t\tattribute_value: %s\n", str_value);
            subnode->addElement (new XMLElement ("attribute", 4));
            subsubnode = subnode->getChildren ()[j];
            subsubnode->addElement (new XMLElement ("key", str_key));
            subsubnode->addElement (new XMLElement ("value", str_value));
            subsubnode->addElement (new XMLElement ("action", "KEEP"));
            subsubnode->addElement (new XMLElement ("state", "DONE"));
        }
        node->addElement (new XMLElement ("action", "KEEP"));
        node->addElement (new XMLElement ("state", "DONE"));
    }

    char str_att_size [XML_TEXTLEN];
    //printf ("number of atts: %d\n", natts);
    sprintf (str_att_size, "%d", natts);
    global_attribute_list->addElement (new XMLElement ("natts", str_att_size));

    for (int i = 0; i < natts; i ++)
    {
        NC_CHECK (nc_inq_attname (ncid, NC_GLOBAL, i, str_key));
        NC_CHECK (nc_inq_att (ncid, NC_GLOBAL, str_key, &att_type, &len_str_value));
        if (att_type == NC_INT)
        {
            NC_CHECK (nc_get_att_int (ncid, NC_GLOBAL, str_key, &i_value));
            sprintf (str_value, "%d", i_value);
        }
        else if (att_type == NC_DOUBLE)
        {
            NC_CHECK (nc_get_att_double (ncid, NC_GLOBAL, str_key, &d_value));
            sprintf (str_value, "%lf", d_value);
        }
        else if (att_type == NC_FLOAT)
        {
            NC_CHECK (nc_get_att_float (ncid, NC_GLOBAL, str_key, &f_value));
            sprintf (str_value, "%f", f_value);
        }
        else
        {
            NC_CHECK (nc_get_att_text (ncid, NC_GLOBAL, str_key, str_value));
            str_value[len_str_value] = '\0';
        }
        //printf ("\tattribute_name: %s\n", str_key);
        //printf ("\tattribute_value: %s\n", str_value);
        global_attribute_list->addElement (new XMLElement ("global_attribute", 4));
        node = global_attribute_list->getChildren ()[i+1];
        node->addElement (new XMLElement ("key", str_key));
        node->addElement (new XMLElement ("value", str_value));
        node->addElement (new XMLElement ("action", "KEEP"));
        node->addElement (new XMLElement ("state", "DONE"));
    }
    NC_CHECK (nc_close (ncid));

    if (root->check ())
    {
        printf ("check pass!\n");
        root->writeToFile (argv[2]);
    }
    else
        printf ("check fail!\n");
    return 0;
}
