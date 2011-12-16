#ifndef IO_NETCDF_H
#define IO_NETCDF_H

#include <string.h>
#include <stdio.h>
#include <netcdf.h>

const int NETCDF_STRLEN = 128;
const int NETCDF_DIMLEN = 2;

typedef enum {DONE, TODO} State;
typedef enum {KEEP, CHANGE_NAME, CHANGE_DATA, DESERT} Action;
typedef enum {INT, DOUBLE} Data_Type;

struct _Dim
{
    int         ID;
    char        name[NETCDF_STRLEN];
    unsigned int         data;
    Action      action;
    State       state;

    _Dim (int _ID, char * _name) : ID(_ID), data(0), action(KEEP), state(DONE) {
        strcpy (name, _name);
    } 

    _Dim (int _ID, char * _name, Action _action, State _state) : ID(_ID), data(0), action(_action), state(_state) {
        strcpy (name, _name);
    }
    
    _Dim (_Dim * pdim)
    {
        ID      = pdim->ID;
        strcpy (name, pdim->name);
        data    = pdim->data;
        action  = pdim->action;
        state   = pdim->state;
    }

};
typedef _Dim * Dim;

struct _Var
{
    int         ID;
    char        name[NETCDF_STRLEN];
    int         dim_size;
    int         dim_list[NETCDF_DIMLEN];
    Data_Type   type;
    void *      data;
    Action      action;
    State       state;

    _Var (int _ID, char * _name, int _dim_size, int * _dim_list, Data_Type _type) : ID(_ID), dim_size(_dim_size), type(_type), action(KEEP), state(DONE) {
        strcpy (name, _name);
        for (int i = 0; i < dim_size; i ++)
            dim_list[i] = _dim_list[i];
        //data = NULL;
        data = (void *) 0;
    }

    _Var (int _ID, char * _name, int _dim_size, int * _dim_list, Data_Type _type, Action _action, State _state) : ID(_ID), dim_size(_dim_size), type(_type), action(_action), state(_state) {
        strcpy (name, _name);
        for (int i = 0; i < dim_size; i ++)
            dim_list[i] = _dim_list[i];
        //data = NULL;
        data = (void *) 0;
    }

    _Var (_Var * pvar)
    {
        ID  = pvar->ID;
        strcpy (name, pvar->name);
        dim_size = pvar->dim_size;
        for (int i = 0; i < dim_size; i ++)
            dim_list[i] = (pvar->dim_list)[i];
        //data = NULL;
        data = (void *) 0;
        type = pvar->type;
        action  = pvar->action;
        state   = pvar->state;
    }
};
typedef _Var * Var;

// note1: current way to generate IO_netCDF class without configuration file
// nodte2: we may add new patterns of data select
class IO_netCDF
{
    private:
        Dim *   dim_set;
        Var *   var_set;
        int     dim_set_size;
        int     var_set_size;
        int     current_dim_set_size;       // note1
        int     current_var_set_size;       // note1
    public:
        IO_netCDF (int _dim_set_size, int _var_set_size);   // note1
        IO_netCDF (char * config_filename);
        ~IO_netCDF ();

        void Add_new_dim (Dim _dim);        // note1
        void Add_new_var (Var _var);        // note1
        void Print_dims ();
        void Print_vars ();
        void Read_file (char * netcdf_file);        // to load data part of Dim and Var
        Dim Get_dim_by_name (const char * dim_name);
        Var Get_var_by_name (const char * var_name);
        Dim Get_dim_by_ID (int dim_id);
        Var Get_var_by_ID (int var_id);
        bool Check_dimlist_of_var (int var_id);
        int Calculate_all_dims_of_var (int var_id);    
        void Modify_dim_name (int dim_id, char * new_name);
        void Modify_dim_data (int dim_id, unsigned int new_data);    // note2
        void Modify_var_name (int var_id, char * new_name);
        void Modify_var_data (int var_id, void * new_data);
        void Write_file (char * netcdf_file);       // after modification, write to new netCDF file
        static void Check_NC_Error (int error_id)
        {
            if (error_id != NC_NOERR)
                //fprintf(stderr, "NC operation failed!\n");
                fprintf(stderr, "%s\n", nc_strerror (error_id));
        }
};
#endif
