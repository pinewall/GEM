#ifndef IO_NETCDF_H
#define IO_NETCDF_H

#include <string.h>
#include <stdio.h>
#include <netcdf.h>

const int NETCDF_STRLEN = 128;
const int NETCDF_DIMLEN = 2;
const int NETCDF_ATTLEN = 1;

typedef enum {DONE, TODO} State;
typedef enum {KEEP, CHANGE_NAME, CHANGE_DATA, DESERT} Action;
typedef enum {INT, DOUBLE} Data_Type;

#define USE_32BITS
#ifdef USE_64BITS
typedef unsigned long   UINT;
#endif
#ifdef USE_32BITS
typedef unsigned int    UINT;
#endif

struct _Prep
{
    char        name[NETCDF_STRLEN];
    char        info[NETCDF_STRLEN];
    _Prep (char * _name)
    {
        strcpy (name, _name);
        strcpy (info, "null");
    }

    _Prep (char * _name, char * _info)
    {
        strcpy (name, _name);
        strcpy (info, _info);
    }

    _Prep (_Prep * pprep)
    {
        strcpy (name, pprep->name);
        strcpy (info, pprep->info);
    }
};
typedef _Prep * Prep;

struct _Dim
{
    char        name[NETCDF_STRLEN];
    char        gname[NETCDF_STRLEN];       // corresponding name in GEM, used for searching and locating
    UINT        data;
    Action      action;
    State       state;

    _Dim (char * _name, char * _gname) : data(0), action(KEEP), state(DONE) {
        strcpy (name, _name);
        strcpy (gname, _gname);
    } 

    _Dim (char * _name, char * _gname, Action _action, State _state) : data(0), action(_action), state(_state) {
        strcpy (name, _name);
        strcpy (gname, _gname);
    }
    
    _Dim (_Dim * pdim)
    {
        strcpy (name, pdim->name);
        strcpy (gname, pdim->gname);
        data    = pdim->data;
        action  = pdim->action;
        state   = pdim->state;
    }

};
typedef _Dim * Dim;

struct _Var
{
    char        name[NETCDF_STRLEN];   // corresponding name in GEM, used for searching and locating
    char        gname[NETCDF_STRLEN];  
    int         dim_size;
    Dim         dim_list[NETCDF_DIMLEN];
    int         prep_size;
    Prep        prep_list[NETCDF_ATTLEN];
    Data_Type   type;
    Prep        prep;
    void *      data;
    Action      action;
    State       state;

    _Var (char * _name, char * _gname, int _dim_size, Dim * _dim_list, Data_Type _type) : dim_size(_dim_size), prep_size(0), type(_type), action(KEEP), state(DONE) {
        strcpy (name, _name);
        strcpy (gname, _gname);
        for (int i = 0; i < dim_size; i ++)
            dim_list[i] = _dim_list[i];
        //data = NULL;
        data = (void *) 0;
    }

    _Var (char * _name, char * _gname, int _dim_size, Dim * _dim_list, int _prep_size, Prep * _prep_list, Data_Type _type) : dim_size(_dim_size), prep_size(_prep_size), type(_type), action(KEEP), state(DONE) {
        strcpy (name, _name);
        strcpy (gname, _gname);
        for (int i = 0; i < dim_size; i ++)
            dim_list[i] = _dim_list[i];
        for (int i = 0; i < prep_size; i ++)
        {
            prep_list[i] = new _Prep ("null", "null");
            strcpy (prep_list[i]->name, _prep_list[i]->name);
            strcpy (prep_list[i]->info, _prep_list[i]->info);
        }
        //data = NULL;
        data = (void *) 0;
    }

    _Var (char * _name, char * _gname, int _dim_size, Dim * _dim_list, Data_Type _type, Action _action, State _state) : dim_size(_dim_size), prep_size(0), type(_type), action(_action), state(_state) {
        strcpy (name, _name);
        strcpy (gname, _gname);
        for (int i = 0; i < dim_size; i ++)
            dim_list[i] = _dim_list[i];
        //data = NULL;
        data = (void *) 0;
    }

    _Var (char * _name, char * _gname, int _dim_size, Dim * _dim_list, int _prep_size, Prep * _prep_list, Data_Type _type, Action _action, State _state) : dim_size(_dim_size), prep_size(_prep_size), type(_type), action(_action), state(_state) {
        strcpy (name, _name);
        strcpy (gname, _gname);
        for (int i = 0; i < dim_size; i ++)
            dim_list[i] = _dim_list[i];
        for (int i = 0; i < prep_size; i ++)
        {
            prep_list[i] = new _Prep ("null", "null");
            strcpy (prep_list[i]->name, _prep_list[i]->name);
            strcpy (prep_list[i]->info, _prep_list[i]->info);
        }
        //data = NULL;
        data = (void *) 0;
    }

    _Var (_Var * pvar)
    {
        strcpy (name, pvar->name);
        strcpy (gname, pvar->gname);
        dim_size = pvar->dim_size;
        for (int i = 0; i < dim_size; i ++)
            dim_list[i] = (pvar->dim_list)[i];
        prep_size = pvar->prep_size;
        for (int i = 0; i < prep_size; i ++)
        {
            prep_list[i] = new _Prep ("null", "null");
            strcpy (prep_list[i]->name, pvar->prep_list[i]->name);
            strcpy (prep_list[i]->info, pvar->prep_list[i]->info);
        }

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
        Prep *  global_prep;
        int     dim_set_size;
        int     var_set_size;
        int     global_prep_size;
        int     current_dim_set_size;       // note1
        int     current_var_set_size;       // note1
        int     current_global_prep_size;   // note1
    public:
        IO_netCDF (int _dim_set_size, int _var_set_size, int _global_prep_size);   // note1
        IO_netCDF (char * config_filename);
        ~IO_netCDF ();

        void Add_new_dim (Dim _dim);        // note1
        void Add_new_var (Var _var);        // note1
        void Add_new_global_prep (Prep _prep);
        void Print_dims ();
        void Print_vars ();
        void Print_global_preps ();
        void Read_file (char * netcdf_file);        // to load data part of Dim and Var
        Dim Get_dim_by_gname (const char * gname);
        Var Get_var_by_gname (const char * gname);
        bool Check_dimlist_of_var (Var var);
        int Calculate_all_dims_of_var (Var var);    
        void Modify_dim_name (const char * gname, char * new_name);
        void Modify_dim_data (const char * gname, UINT new_data);    // note2
        void Modify_var_name (const char * gname, char * new_name);
        void Modify_var_data (const char * gname, void * new_data);
        void Modify_var_prep (const char * gname, const char * prep_name, char * new_prep_info);  // used for change units, so new_data is not needed
        void Write_file (char * netcdf_file);       // after modification, write to new netCDF file
        static void Check_NC_Error (int error_id)
        {
            if (error_id != NC_NOERR)
                //fprintf(stderr, "NC operation failed!\n");
                fprintf(stderr, "%s\n", nc_strerror (error_id));
        }
};
#endif
