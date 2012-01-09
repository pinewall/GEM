CC=icpc -g

GEMPATH=$(shell pwd)
SRCPATH=$(GEMPATH)/src
INCPATH=$(GEMPATH)/include
TOOLS_SRCPATH=$(GEMPATH)/tools/src
TOOLS_INCPATH=$(GEMPATH)/tools/include
MODPATH=$(GEMPATH)/mod

# overwrite with your own lib path of netCDF
NETCDF_LIB=-L/opt/netCDF/lib
NETCDF_INCLUDE=-I/opt/netCDF/include
LIB=-lnetcdf -lm

vpath %.cxx UnitTest src tools/UnitTest tools/src
vpath %.o mod

OBJS=IO_netCDF.o SParseMatrix.o field.o gradient.o \
     toy_test.o sparse_test.o \
	 unit_test.o gem.o \
     xml_base.o meta.o cdfcopyer.o remap.o units.o\
     xml_test.o

%.cxx: %.h
	touch $@
%.o: %.cxx
	$(CC) -c $(NETCDF_INCLUDE) -I $(INCPATH) -I $(TOOLS_INCPATH) $< -o $(MODPATH)/$@

lib: 
	$(shell mkdir -p $(MODPATH))
	make $(OBJS)

# binaries or testers
units: xml_base.o IO_netCDF.o units.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
remap: remap.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
cdfcopyer: xml_base.o IO_netCDF.o cdfcopyer.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
meta: xml_base.o meta.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
xml_test: xml_base.o xml_test.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
gem: xml_base.o IO_netCDF.o SParseMatrix.o field.o gradient.o gem.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
unit_test: xml_base.o IO_netCDF.o SParseMatrix.o field.o gradient.o unit_test.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
toy_test: toy_test.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
sparse_test: SParseMatrix.o sparse_test.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@

clean:
	rm -f $(MODPATH)/*.o
distclean:
	rm -rf $(MODPATH) unit_test toy_test sparse_test
