CC=icpc -g

GEMPATH=$(shell pwd)
SRCPATH=$(GEMPATH)/src
INCPATH=$(GEMPATH)/include
MODPATH=$(GEMPATH)/mod

# overwrite with your own lib path of netCDF
NETCDF_LIB=-L/opt/netCDF/lib
NETCDF_INCLUDE=-I/opt/netCDF/include
LIB=-lnetcdf -lm

vpath %.cxx UnitTest src
vpath %.o mod

OBJS=IO_netCDF.o SParseMatrix.o field.o gradient.o \
     unit_test.o toy_test.o sparse_test.o

%.cxx: %.h
	touch $@
%.o: %.cxx
	$(CC) -c $(NETCDF_INCLUDE) -I $(INCPATH) $< -o $(MODPATH)/$@

lib: 
	$(shell mkdir -p $(MODPATH))
	make $(OBJS)

#unit_test: IO_netCDF.o sparse.o gradient.o unit_test.o
unit_test: IO_netCDF.o SParseMatrix.o field.o gradient.o unit_test.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
toy_test: toy_test.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@
sparse_test: SParseMatrix.o sparse_test.o
	$(CC) $(NETCDF_LIB) $+ $(LIB) -o $@

clean:
	rm -f $(MODPATH)/*.o unit_test toy_test sparse_test *.nc
distclean:
	rm -rf $(MODPATH) *.nc unit_test toy_test sparse_test
