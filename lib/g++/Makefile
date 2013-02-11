##
## User: ricardo
## Time: Sep 18, 2011 7:45:42 PM
## Makefile created by Oracle Solaris Studio.
##
## This file is generated automatically.
##
#### Compiler and tool definitions shared by all build targets #####

CCC = g++
CXX = g++
BASICOPTS = -g
CCFLAGS = $(BASICOPTS)
CXXFLAGS = $(BASICOPTS)
CCADMIN = 

# Define the target directories.
TARGETDIR_libsiren=GNU-amd64-Linux

all: $(TARGETDIR_libsiren)/libsiren

## Target: libsiren
CPPFLAGS_libsiren = \
	    -I../include/ \
	    -I/usr/local/include \
	    -I/usr/include/dcmtk \
	    -I/usr/include/arboretum \
	    -I/usr/include/artemis \
	    -I/usr/include/opencv \
	    -I /oracle/product/db/rdbms/public/ \
	    -I /oracle/product/rdbms/db/demo/ 	

LIBS = 	\
	-L/usr/local/lib \
	-L /oracle/product/db/lib/ \
	-L /oracle/product/db/rdbms/lib/ \
	-ldl -lm -locci \
	-ljpeg \
	-lpqxx -lpq \
	-lcv \
	-lhighgui \
	-lstdc++ \
	-lclntsh `cat /oracle/product/db/lib/sysliblist` \
	-L/usr/include/opencv				
OBJS_libsiren =  \
	$(TARGETDIR_libsiren)/QueryPostgres.o \
	$(TARGETDIR_libsiren)/DataDictionaryAccess.o \
	$(TARGETDIR_libsiren)/MetricTreeManager.o \
	$(TARGETDIR_libsiren)/Lex.o \
	$(TARGETDIR_libsiren)/ConnOracle.o \
	$(TARGETDIR_libsiren)/QueryOracle.o \
	$(TARGETDIR_libsiren)/Query.o \
	$(TARGETDIR_libsiren)/ConnPostgres.o \
	$(TARGETDIR_libsiren)/Parser.o \
	$(TARGETDIR_libsiren)/Conn.o


# Link or archive
ARCPP = $(AR) $(ARFLAGS) $@ 
$(TARGETDIR_libsiren)/libsiren: $(TARGETDIR_libsiren) $(OBJS_libsiren)
	$(ARCPP) $(OBJS_libsiren)


# Compile source files into .o files
$(TARGETDIR_libsiren)/Conn.o: $(TARGETDIR_libsiren) src/Conn.cpp
	$(COMPILE.cc) $(CCFLAGS_libsiren) $(CPPFLAGS_libsiren) $(LIBS) -o $@ src/Conn.cpp

$(TARGETDIR_libsiren)/ConnOracle.o: $(TARGETDIR_libsiren) src/ConnOracle.cpp
	$(COMPILE.cc) $(CCFLAGS_libsiren) $(CPPFLAGS_libsiren) $(LIBS) -o $@ src/ConnOracle.cpp

$(TARGETDIR_libsiren)/ConnPostgres.o: $(TARGETDIR_libsiren) src/ConnPostgres.cpp
	$(COMPILE.cc) $(CCFLAGS_libsiren) $(CPPFLAGS_libsiren) $(LIBS) -o $@ src/ConnPostgres.cpp

$(TARGETDIR_libsiren)/Query.o: $(TARGETDIR_libsiren) src/Query.cpp
	$(COMPILE.cc) $(CCFLAGS_libsiren) $(CPPFLAGS_libsiren) $(LIBS) -o $@ src/Query.cpp

$(TARGETDIR_libsiren)/QueryPostgres.o: $(TARGETDIR_libsiren) src/QueryPostgres.cpp
	$(COMPILE.cc) $(CCFLAGS_libsiren) $(CPPFLAGS_libsiren) $(LIBS) -o $@ src/QueryPostgres.cpp

$(TARGETDIR_libsiren)/QueryOracle.o: $(TARGETDIR_libsiren) src/QueryOracle.cpp
	$(COMPILE.cc) $(CCFLAGS_libsiren) $(CPPFLAGS_libsiren) $(LIBS) -o $@ src/QueryOracle.cpp

$(TARGETDIR_libsiren)/Lex.o: $(TARGETDIR_libsiren) src/Lex.cpp
	$(COMPILE.cc) $(CCFLAGS_libsiren) $(CPPFLAGS_libsiren) $(LIBS) -o $@ src/Lex.cpp

$(TARGETDIR_libsiren)/MetricTreeManager.o: $(TARGETDIR_libsiren) src/MetricTreeManager.cpp
	$(COMPILE.cc) $(CCFLAGS_libsiren) $(CPPFLAGS_libsiren) $(LIBS) -o $@ src/MetricTreeManager.cpp

$(TARGETDIR_libsiren)/DataDictionaryAccess.o: $(TARGETDIR_libsiren) src/DataDictionaryAccess.cpp
	$(COMPILE.cc) $(CCFLAGS_libsiren) $(CPPFLAGS_libsiren) $(LIBS) -o $@ src/DataDictionaryAccess.cpp

$(TARGETDIR_libsiren)/Parser.o: $(TARGETDIR_libsiren) src/Parser.cpp
	$(COMPILE.cc) $(CCFLAGS_libsiren) $(CPPFLAGS_libsiren) $(LIBS) -o $@ src/Parser.cpp

#### Clean target deletes all generated files ####
clean:
	rm -f \
		$(TARGETDIR_libsiren)/libsiren \
		$(TARGETDIR_libsiren)/QueryPostgres.o \
		$(TARGETDIR_libsiren)/DataDictionaryAccess.o \
		$(TARGETDIR_libsiren)/MetricTreeManager.o \
		$(TARGETDIR_libsiren)/Lex.o \
		$(TARGETDIR_libsiren)/ConnOracle.o \
		$(TARGETDIR_libsiren)/QueryOracle.o \
		$(TARGETDIR_libsiren)/Query.o \
		$(TARGETDIR_libsiren)/ConnPostgres.o \
		$(TARGETDIR_libsiren)/Parser.o \
		$(TARGETDIR_libsiren)/Conn.o
	$(CCADMIN)
	rm -f -r $(TARGETDIR_libsiren)


# Create the target directory (if needed)
$(TARGETDIR_libsiren):
	mkdir -p $(TARGETDIR_libsiren)


# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux

