
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@ @FORCE_32_CFLAGS@

TESTS = copy_message.sh get.sh get_data.sh get_pl.sh get_pv.sh keys_iterator.sh \
        nearest.sh precision.sh multi_write.sh multi.sh print_data.sh set.sh set_bitmap.sh set_missing.sh \
        set_pv.sh samples.sh count_messages.sh

noinst_PROGRAMS  = index copy_message get get_data get_pl get_pv keys_iterator \
                   multi_write multi nearest precision print_data set set_bitmap set_missing \
				   set_pv samples count_messages

index_SOURCES=index.f90
copy_message_SOURCES=copy_message.f90
get_SOURCES=get.f90
get_data_SOURCES=get_data.f90
get_pl_SOURCES=get_pl.f90
get_pv_SOURCES=get_pv.f90
keys_iterator_SOURCES=keys_iterator.f90
multi_write_SOURCES=multi_write.f90
multi_SOURCES=multi.f90
nearest_SOURCES=nearest.f90
precision_SOURCES=precision.f90
print_data_SOURCES=print_data.f90
set_SOURCES=set.f90
set_bitmap_SOURCES=set_bitmap.f90
set_missing_SOURCES=set_missing.f90
set_pv_SOURCES=set_pv.f90
samples_SOURCES=samples.f90
count_messages_SOURCES=count_messages.f90

INCLUDES =  -I$(top_builddir)/src 

AM_FCFLAGS = -I$(top_builddir)/fortran

LDADD = $(top_builddir)/fortran/libgrib_api_f90.a $(top_builddir)/src/libgrib_api.a
DEPENDENCIES = $(LDADD)

EXTRA_DIST =$(TESTS) include.sh

