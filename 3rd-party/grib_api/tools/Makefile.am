
AM_CFLAGS =  @WARN_PEDANTIC@ @WERROR@ @FORCE_32_CFLAGS@ -DYYDEBUG

AM_YFLAGS = -d

noinst_LTLIBRARIES = libgrib_tools.la

libgrib_tools_la_SOURCES = grib_tools.c \
                        grib_options.c


bin_PROGRAMS = grib_keys grib_histogram grib_error grib_add big2gribex \
               grib_debug grib_info grib_filter grib_ls grib_dump \
			   grib2ppm grib_set grib_get grib_get_data grib_copy \
			   grib_packing grib_cmp grib_convert  grib_distance \
			   grib_compare parser grib_count grib_2_request grib1to2

noinst_PROGRAMS =  gaussian grib_diff mars_request xref all_keys #dumpload grib_compare_old

#test_SOURCES   =  test.c

#dumpload_SOURCES   =  dumpload.c loady.y loadl.l load.c dump.c load.h

grib_dump_SOURCES   =  grib_dump.c
grib_count_SOURCES   =  grib_count.c
grib_cmp_SOURCES   =  grib_cmp.c
grib_packing_SOURCES   =  grib_packing.c
grib_histogram_SOURCES   =  grib_histogram.c
grib_distance_SOURCES   =  grib_distance.c
grib_debug_SOURCES   =  grib_debug.c
parser_SOURCES      =  parser.c
all_keys_SOURCES      =  all_keys.c
grib2ppm_SOURCES    =  grib2ppm.c
grib_info_SOURCES   =  grib_info.c
grib_keys_SOURCES   =  grib_keys.c
grib_2_request_SOURCES   =  grib_2_request.c

grib_ls_SOURCES     =  grib_ls.c
grib_copy_SOURCES   =  grib_copy.c
grib_convert_SOURCES     =  grib_convert.c
grib_filter_SOURCES =  grib_filter.c
grib_set_SOURCES     =  grib_set.c
grib_get_SOURCES     =  grib_get.c
grib_get_data_SOURCES     =  grib_get_data.c

grib_add_SOURCES     =  grib_add.c
grib_error_SOURCES     =  grib_error.c
grib_diff_SOURCES     =  grib_diff.c
grib_compare_SOURCES     =  grib_compare.c 
#grib_compare_old_SOURCES     =  grib_compare_old.c md5.c md5.h 
big2gribex_SOURCES     =  big2gribex.c
mars_request_SOURCES     =  mars_request.c
xref_SOURCES     =  xref.c
gaussian_SOURCES     =  gaussian.c
grib1to2_SOURCES = grib1to2.txt

INCLUDES =  -I$(top_builddir)/src

LDADD = libgrib_tools.la $(top_builddir)/src/libgrib_api.la
DEPENDENCIES = $(LDADD)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libgrib_tools.la libgrib_tools.a

grib1to2: grib1to2.txt
	sed "s:toolsdir:$(bindir):" grib1to2.txt > grib1to2

noinst_HEADERS = grib_tools.h grib_msg_types.h
