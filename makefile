CC=icpc -g
INCPATH=-I/opt/netCDF/include
LIBPATH=-L/opt/netCDF/lib
LIB=-lnetcdf -lm

%.cxx: %.h
	touch $@
%.o: %.cxx
	$(CC) -c $(INCPATH) $< -o $@
unit_test: IO_netCDF.o SParseMatrix.o field.o gradient.o unit_test.o
#unit_test: IO_netCDF.o sparse.o gradient.o unit_test.o
	$(CC)    $(INCPATH) $(LIBPATH) $+ $(LIB) -o $@
toy_test: toy_test.o
	$(CC)	 $(INCPATH) $(LIBPATH) $< $(LIB) -o $@
