CC=icpc -g -O0

GEMPATH=$(shell pwd)
SRCPATH=$(GEMPATH)/src
INCPATH=$(GEMPATH)/include
TOOLS_SRCPATH=$(GEMPATH)/tools/src
TOOLS_INCPATH=$(GEMPATH)/tools/include
MODPATH=$(GEMPATH)/mod

# overwrite with your own lib path of netCDF
NETCDF_LIB=-L/opt/intel/netCDF/lib
NETCDF_INCLUDE=-I/opt/intel/netCDF/include
LIB=-lnetcdf -lm

OBJS=$(MODPATH)/IO_netCDF.o $(MODPATH)/SParseMatrix.o $(MODPATH)/STL_SparseMatrix.o $(MODPATH)/field.o $(MODPATH)/gradient.o \
     $(MODPATH)/toy_test.o $(MODPATH)/sparse_test.o \
	 $(MODPATH)/unit_test.o $(MODPATH)/gem.o \
     $(MODPATH)/xml_base.o $(MODPATH)/meta.o $(MODPATH)/cdfcopyer.o $(MODPATH)/remap.o $(MODPATH)/units.o\
     $(MODPATH)/xml_test.o

# binaries or testers
units: $(MODPATH)/xml_base.o $(MODPATH)/IO_netCDF.o $(MODPATH)/units.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
remap: $(MODPATH)/remap.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
cdfcopyer: $(MODPATH)/xml_base.o $(MODPATH)/IO_netCDF.o $(MODPATH)/cdfcopyer.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
meta: $(MODPATH)/xml_base.o $(MODPATH)/meta.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
xml_test: $(MODPATH)/xml_base.o $(MODPATH)/xml_test.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
gem: $(MODPATH)/xml_base.o $(MODPATH)/IO_netCDF.o $(MODPATH)/SParseMatrix.o $(MODPATH)/STL_SparseMatrix.o $(MODPATH)/field.o $(MODPATH)/gradient.o $(MODPATH)/gem.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
unit_test: $(MODPATH)/xml_base.o $(MODPATH)/IO_netCDF.o $(MODPATH)/SParseMatrix.o $(MODPATH)/STL_SparseMatrix.o $(MODPATH)/field.o $(MODPATH)/gradient.o $(MODPATH)/unit_test.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
toy_test: $(MODPATH)/toy_test.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
sparse_test: $(MODPATH)/SParseMatrix.o $(MODPATH)/sparse_test.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@

#%.cxx: %.h
#	touch $@

$(MODPATH)/%.o: %.cxx
	$(CC) -c $(NETCDF_INCLUDE) -I $(INCPATH) -I $(TOOLS_INCPATH) $< -o $@

#vpath %.o mod
vpath %.cxx UnitTest src tools/UnitTest tools/src

lib: 
	$(shell mkdir -p $(MODPATH))
	make $(OBJS)

clean:
	rm -f $(MODPATH)/*.o
distclean:
	rm -rf $(MODPATH) unit_test toy_test sparse_test
