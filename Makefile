##
## Makefile for Siren and for a CPP test file.
## @author: Ricardo Pereira Maiostri (maiostri@gmail.com)

SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .cpp .o
CCC = g++
AR= ar
ARFLAGS = -rvs 
CFLAGS = -fPIC -g -Wall 
CCFLAGS = -fPIC -g -c -Wall
SOFLAGS = -shared -o


## Define the target directories.
DESTDIR = /home/ricardo/Dropbox/USP/Iniciacao/siren/trunk/
srcdir = src

# source subdirectories 
conndir		= /db/conn
queriesdir	= /db/query
sirenqueriesdir = /sirenqueries
dynamictypesdir = /dynamictypes
utilsdir	= /utils

## bin and lib dir.
bindir = build
libdir = lib

## Target: all

## Include headers
INCLUDES = \
	    -I$(DESTDIR)$(srcdir)/ \
	    -I /u01/app/oracle/product/11.2.0/dbhome_1/rdbms/public/ \
	    -I /u01/app/oracle/product/11.2.0/dbhome_1/rdbms/db/demo/ 		    

# Link libraries
LIBS = \
	-llibartemis \
	-larboretum \
	-lhermes \
	-L /usr/lib64/dcmtk \
	-loflog -lofstd \
	-L /u01/app/oracle/product/11.2.0/dbhome_1/lib/ -locci \
	-L /u01/app/oracle/product/db/11.2.0/dbhome_1/rdbms/lib/ \
	-ldl -lm \
	-lpqxx-3.2 \
	-lpq \
	-lboost_system \
	-llog4cpp \
	-lboost_filesystem \
	-lpthread \
	-lclntsh `cat /u01/app/oracle/product/11.2.0/dbhome_1/lib/sysliblist`
	

## Object files
OBJECTS =  \
	$(DESTDIR)$(bindir)/imageUtils.o \
	$(DESTDIR)$(bindir)/stDynamicZernike.o \
	$(DESTDIR)$(bindir)/stDynamicTexture.o \
	$(DESTDIR)$(bindir)/Conn.o \
	$(DESTDIR)$(bindir)/ConnOracle.o \
	$(DESTDIR)$(bindir)/Query.o \
	$(DESTDIR)$(bindir)/ConnPostgres.o \
	$(DESTDIR)$(bindir)/QueryPostgres.o \
	$(DESTDIR)$(bindir)/QueryOracle.o \
	$(DESTDIR)$(bindir)/SirenQueries.o \
	$(DESTDIR)$(bindir)/Lex.o \
	$(DESTDIR)$(bindir)/MetricTreeManager.o \
	$(DESTDIR)$(bindir)/DataDictionaryAccess.o \
	$(DESTDIR)$(bindir)/Parser.o

## Create the static library
ARCPP = $(AR) $(ARFLAGS) $@ 

SOCPP = $(CCC) $(SOFLAGS) $@

# Static library option
$(DESTDIR)$(libdir)/libsiren.a: $(DESTDIR) $(OBJECTS)
	$(ARCPP) $(OBJECTS)
	
# Shared library optio
$(DESTDIR)$(libdir)/libsiren.so: $(DESTDIR) $(OBJECTS)
	$(SOCPP) $(OBJECTS)
	
## Compile source files into .o files
$(DESTDIR)$(bindir)/imageUtils.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)$(utilsdir)/imageUtils.cpp 
$(DESTDIR)$(bindir)/stDynamicZernike.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)$(dynamictypesdir)/stDynamicZernike.cpp 	
$(DESTDIR)$(bindir)/stDynamicTexture.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)$(dynamictypesdir)/stDynamicTexture.cpp 	
$(DESTDIR)$(bindir)/Conn.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)$(conndir)/Conn.cpp 
$(DESTDIR)$(bindir)/ConnOracle.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)$(conndir)/ConnOracle.cpp 	
$(DESTDIR)$(bindir)/ConnPostgres.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)$(conndir)/ConnPostgres.cpp 	
$(DESTDIR)$(bindir)/Query.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)$(queriesdir)/Query.cpp 	
$(DESTDIR)$(bindir)/QueryOracle.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)$(queriesdir)/QueryOracle.cpp 	
$(DESTDIR)$(bindir)/QueryPostgres.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)$(queriesdir)/QueryPostgres.cpp 	
$(DESTDIR)$(bindir)/SirenQueries.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)$(sirenqueriesdir)/SirenQueries.cpp 	
$(DESTDIR)$(bindir)/Lex.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)/Lex.cpp 	
$(DESTDIR)$(bindir)/MetricTreeManager.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)/MetricTreeManager.cpp 	
$(DESTDIR)$(bindir)/DataDictionaryAccess.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)/DataDictionaryAccess.cpp 	
$(DESTDIR)$(bindir)/Parser.o : 
	$(CCC) $(CCFLAGS) $(INCLUDES) -o $@ $(DESTDIR)$(srcdir)/Parser.cpp 	

siren: 
	$(DESTDIR)$(libdir)/libsiren.so
	
teste: 
	$(CCC) $(CFLAGS) $(INCLUDES) -o Teste Testes.cpp -L$(DESTDIR)$(libdir) -lsiren $(LIBS)

all:
	make clean
	make 
	make teste

## Remove the object files, the static library, and the test program file from the destination directory
clean:
	rm -f $(DESTDIR)$(libdir)/*.a	
	rm -f $(DESTDIR)$(libdir)/*.a	
	rm -f $(DESTDIR)$(bindir)/*.o
	rm -f Teste

				

## Install the library in the system(requires root permission)
install:
	sudo cp src/*.h /usr/local/include/siren/
	sudo cp $(DESTDIR)/libsiren.a /usr/local/lib/
