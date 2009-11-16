
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@ @FORCE_32_CFLAGS@

TESTS = iterator.sh get.sh print_data.sh set.sh keys_iterator.sh multi.sh multi_write.sh precision.sh list.sh

noinst_PROGRAMS        =  nearest set_bitmap iterator get print_data set set_missing keys_iterator \
							multi multi_write precision set_pv list
bin_PROGRAMS = points

nearest_SOURCES               =   nearest.c
multi_write_SOURCES               =   multi_write.c
get_SOURCES               =   get.c
print_data_SOURCES        =   print_data.c
set_SOURCES               =   set.c
set_missing_SOURCES               =   set_missing.c
set_bitmap_SOURCES        =   set_bitmap.c
multi_SOURCES             =   multi.c
keys_iterator_SOURCES     =   keys_iterator.c
iterator_SOURCES          =   iterator.c
set_pv_SOURCES          =   set_pv.c
points_SOURCES          =   points.c
list_SOURCES          =   list.c

INCLUDES =  -I$(top_builddir)/src

LDADD = $(top_builddir)/src/libgrib_api.a
DEPENDENCIES = $(LDADD)

EXTRA_DIST =$(TESTS) include.sh

