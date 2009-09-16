
     TESTS = definitions.sh double_cmp.sh \
			 ieee.sh ls.sh grib1to2.sh \
	         grib2to1.sh badgrib.sh convert.sh filter.sh  \
	         multi.sh budg.sh gridType.sh concept.sh decimalPrecision.sh \
			 bitsPerValue.sh get_fail.sh missing.sh \
			 step.sh set.sh iterator.sh \
			 compare.sh level.sh \
			 bitmap.sh list.sh \
			 multi_from_message.sh change_scanning.sh \
			 julian.sh statistics.sh tigge_grib2to1.sh \
			 tigge_extra_grib2to1.sh \
			 read_any.sh debug.sh $(JPEG_TEST)

     noinst_PROGRAMS = read_any double_cmp packing pack_unpack multi_from_message julian index 


	 multi_from_message_SOURCES = multi_from_message.c
	 packing_SOURCES = packing.c
	 read_any_SOURCES = read_any.c
	 double_cmp_SOURCES = double_cmp.c
	 pack_unpack_SOURCES = pack_unpack.c
	 julian_SOURCES = julian.c
	 index_SOURCES = index.c

     LDADD = $(top_builddir)/src/libgrib_api.a $(EMOS_LIB)

     INCLUDES   = -I$(top_builddir)/src

     EXTRA_DIST = $(TESTS) mf.rules filter_rules include.sh
   
