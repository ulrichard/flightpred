dnl Process this file with autoconf to produce a configure script.

AC_DEFUN([_AM_AUTOCONF_VERSION],[])

AC_PREREQ(2.59)
AC_INIT(src/grib_api.h,[ ], [Software.Services@ecmwf.int])

# Source file containing package/library versioning information.

 . ${srcdir}/version.sh

GRIB_API_MAIN_VERSION="${GRIB_API_MAJOR_VERSION}.${GRIB_API_MINOR_VERSION}.${GRIB_API_REVISION_VERSION}"
 echo $GRIB_API_MAIN_VERSION
PACKAGE_VERSION="${GRIB_API_MAIN_VERSION}"
AC_SUBST(GRIB_API_MAIN_VERSION)
echo "configuring ${PACKAGE_NAME} ${PACKAGE_VERSION}"

AC_CONFIG_AUX_DIR([config])

# Ensure that make can run correctly
AM_SANITY_CHECK

AC_CONFIG_SRCDIR([src/grib_api.h])
AC_CONFIG_HEADER([src/config.h])
AM_INIT_AUTOMAKE($PACKAGE_NAME,[${PACKAGE_VERSION}],[http://www.ecmwf.int])

definition_files_path=/usr/local/share
default_perl_install='${prefix}/perl'

if test "$prefix" != 'NONE' ; then
 definition_files_path=${prefix}/share
 default_perl_install=${prefix}/perl
fi

if test "$datadir" != '${datarootdir}' ; then
  definition_files_path=$datadir
fi

AC_DEFINE_UNQUOTED(GRIB_API_MAIN_VERSION,$GRIB_API_VERSION,Grib Api version)
AC_DEFINE_UNQUOTED(GRIB_API_MAJOR_VERSION,$GRIB_API_MAJOR_VERSION,Grib Api Major release)
AC_DEFINE_UNQUOTED(GRIB_API_MINOR_VERSION,$GRIB_API_MINOR_VERSION,Grib Api Minor release)
AC_DEFINE_UNQUOTED(GRIB_API_REVISION_VERSION,$GRIB_API_REVISION_VERSION,Grib Api Revision release)
AH_TEMPLATE([_LARGE_FILE_API],
			[Needs to be undefined on some AIX])

dnl Checks for programs.
AC_PROG_CC(xlc_r xlc gcc cc) 
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_F77(gfortran pgf90 pgf77 xlf gfortran f77 g77 f90 ifort) 
AC_PROG_FC(gfortran pgf90 xlf90 gfortran f90 ifort) 

dnl check availability of pthreads
AC_ARG_ENABLE([pthread],
  [AC_HELP_STRING([--enable-pthread],
                  [enable POSIX thread ])],
    GRIB_PTHREADS=1 , GRIB_PTHREADS=0
)

if test $GRIB_PTHREADS -eq 1
then
AC_GRIB_PTHREADS
AC_GRIB_LINUX_PTHREADS
else
GRIB_LINUX_PTHREADS=0
fi

AC_DEFINE_UNQUOTED(GRIB_PTHREADS,$GRIB_PTHREADS,1->pthreads enabled 0->pthreads disabled)
AC_DEFINE_UNQUOTED(GRIB_LINUX_PTHREADS,$GRIB_LINUX_PTHREADS,1->pthreads enabled 0->pthreads disabled)

dnl check on uppercase fortran modules not working to be fixed
dnl some fortran compilers change the name of the .mod file in upper case!
ac_cv_prog_f90_uppercase_mod=no
AC_PROG_FC_UPPERCASE_MOD
AM_CONDITIONAL(UPPER_CASE_MOD, [test "x$ac_cv_prog_f90_uppercase_mod" = xyes])

AC_IEEE_BE
AC_DEFINE_UNQUOTED(IEEE_BE,$IS_IEEE_BE,1-> ieee big endian float/double 0->no ieee big endian float/double)

AC_IEEE_LE
AC_DEFINE_UNQUOTED(IEEE_LE,$IS_IEEE_LE,1-> ieee little endian float/double 0->no ieee little endian float/double)

dnl disable ieee native packing
AC_ARG_ENABLE([ieee-native],
  [AC_HELP_STRING([--disable-ieee-native],
                  [disable ieee native packing])],
     without_ieee=1,without_ieee=0)

if test $without_ieee -eq 1
then
AC_DEFINE_UNQUOTED(IEEE_LE,0,1-> ieee little endian float/double 0->no ieee little endian float/double)
AC_DEFINE_UNQUOTED(IEEE_BE,0,1-> ieee big endian float/double 0->no ieee big endian float/double)
fi

AC_BIG_ENDIAN
AC_DEFINE_UNQUOTED(IS_BIG_ENDIAN,$IS_BIG_ENDIAN,1-> big endian 0->little endian)

AC_INLINE
AC_DEFINE_UNQUOTED(GRIB_INLINE,$HAS_INLINE,inline if available)

AC_ALIGN
AC_DEFINE_UNQUOTED(GRIB_MEM_ALIGN,$MEM_ALIGN,memory alignment required)

AC_ARG_ENABLE([align-memory],
  [AC_HELP_STRING([--enable-align-memory],
                  [enable memory alignement])],
AC_DEFINE_UNQUOTED(GRIB_MEM_ALIGN,1,memory alignment required) ,
	 )

dnl enable memory management
AC_ARG_ENABLE([memory-management],
  [AC_HELP_STRING([--enable-memory-management],
                  [enable memory ])],
AC_DEFINE_UNQUOTED(MANAGE_MEM,1,memory management) ,
AC_DEFINE_UNQUOTED(MANAGE_MEM,0,memory management) 
	 )

dnl enable development configuration
DEVEL_RULES='' 
AC_ARG_ENABLE([development],
  [AC_HELP_STRING([--enable-development],
                  [enable development configuration ])],
    GRIB_DEVEL=1 , GRIB_DEVEL=0
)
if test $GRIB_DEVEL -eq 1
then
   DEVEL_RULES='extrules.am'
else
   DEVEL_RULES='dummy.am'
fi
AC_SUBST(DEVEL_RULES)
AC_SUBST(GRIB_DEVEL)


dnl Large file support
AC_FUNC_FSEEKO
CREATE_H=''
if test x"$ac_cv_func_fseeko" != xyes ; then
	CREATE_H='./create_h.sh 1'
else
	CREATE_H='./create_h.sh 0'
fi 

AC_SYS_LARGEFILE

dnl What OS are we running?
AC_CANONICAL_HOST

dnl Use libtool
AM_PROG_LIBTOOL
dnl Libtool version information (we use 0 and '0d' for debian until grib_api
dnl maintainers start providing shared libraries)
GRIBAPI_LIBTOOL_VERSIONING="-version-info 0:0:0 -release 0d"
AC_SUBST(GRIBAPI_LIBTOOL_VERSIONING)

GRIB_TEMPLATES_PATH=${definition_files_path}/grib_api/samples
AC_DEFINE_UNQUOTED(GRIB_TEMPLATES_PATH,"$GRIB_TEMPLATES_PATH",Directory where templates are)
AC_DEFINE_UNQUOTED(GRIB_SAMPLES_PATH,"$GRIB_TEMPLATES_PATH",Directory where samples are)

GRIB_DEFINITION_PATH=${definition_files_path}/grib_api/definitions
AC_DEFINE_UNQUOTED(GRIB_DEFINITION_PATH,"$GRIB_DEFINITION_PATH",Directory where definition files are)

AC_SUBST(GRIB_TEMPLATES_PATH)
AC_SUBST(GRIB_SAMPLES_PATH)
AC_SUBST(GRIB_DEFINITION_PATH)

dnl Forse 32 bits addressing mode on 64 bits machines
AC_ARG_ENABLE([32-bits-addressing-mode],
        [AC_HELP_STRING([--enable-32-bits-addressing-mode],
		                [Forse 32 bits addressing mode on 64 bits machines])],
        enable_32_bits_addressing_mode='yes', enable_32_bits_addressing_mode='no')

echo "configure: grib_api host - $host $host_os"

if test -z "$ARFLAGS" ; then
   ARFLAGS=cru
fi

if test "$enable_32_bits_addressing_mode" = 'yes'
then
   case "$host" in
   *-aix*)
          ARFLAGS="-rv -X32"
		  PERL_LD_OPTIONS="LD='${LD-ld} -b32'"
		  CFLAGS="-q32 $CFLAGS"
		  FFLAGS="-q32 $FFLAGS"
		  LDFLAGS="-b32 $LDFLAGS"
	      ;;
   x86_64-*linux*)
          PERL_LD_OPTIONS="LD='${LD-ld} -m elf_i386'"
	      CFLAGS="-m32 $CFLAGS"
          ;;
   ppc64-*linux*|powerpc64-*linux*)
          PERL_LD_OPTIONS="LD='${LD-ld} -m elf32ppclinux'"
	      CFLAGS="-m32 $CFLAGS"
          ;;
   s390x-*linux*)
          PERL_LD_OPTIONS="LD='${LD-ld} -m elf_s390'"
	      CFLAGS="-m32 $CFLAGS"
          ;;
   sparc64-*linux*)
          PERL_LD_OPTIONS="LD='${LD-ld} -m elf32_sparc'"
	      CFLAGS="-m32 $CFLAGS"
          ;;
   ia64-hp-hpux*)
	      CFLAGS="+DD32 $CFLAGS"
          ;;
   esac
fi
AC_SUBST(ARFLAGS)
AC_SUBST(FORCE_32_CFLAGS)
AC_SUBST(FORCE_32_LDFLAGS)
AC_SUBST(PERL_LD_OPTIONS)

dnl Fortran interface
AC_ARG_ENABLE([fortran],
  [AC_HELP_STRING([--disable-fortran],
                  [disable fortran interface])],
     without_fortran=1,without_fortran=0)

if test "x$FC" == "x"
then
  without_fortran=1
fi

AM_CONDITIONAL([F90_CHECK], [test $without_fortran -ne 1])
AM_CONDITIONAL([FORTRAN_MOD], [test $without_fortran -ne 1])

dnl EMOS
AC_ARG_WITH([emos],
        [AC_HELP_STRING([--with-emos=EMOS],
	    [use emos for tests])],
        emos=$withval, emos='none')

EMOS_LIB=""
if test "$emos" != 'none'
then
  EMOS_LIB=$emos
  AC_DEFINE(HAVE_LIBEMOS,1,Define if you have EMOS library)
fi


dnl fortran libraries
AC_ARG_WITH([fortranlibdir],
        [AC_HELP_STRING([--with-fortranlibdir=FORTRANDIR],
	    [fortran libraries directory ])],
        fortranlibdir=$withval, fortranlibdir='')

AC_ARG_WITH([fortranlibs],
        [AC_HELP_STRING([--with-fortranlibs=FORTRANLIBS],
	    [fortran libraries to link from C])],
        fortranlibs=$withval, fortranlibs='none')

if test "$fortranlibs" != 'none'
then
   EMOS_LIB="$emos -L$fortranlibdir $fortranlibs -Wl,-rpath $fortranlibdir"
fi

AC_SUBST(EMOS_LIB)

dnl multithread packing
AC_ARG_ENABLE([omp-packing],
  [AC_HELP_STRING([--enable-omp-packing],
                  [enable OpenMP multithreaded packing])],
     with_omp=1,with_omp=0)
if test ${with_omp} -eq 1
then
  AC_DEFINE(OMP_PACKING,1,1->OpenMP packing 0->single thread packing)
else
  AC_DEFINE(OMP_PACKING,0,1->OpenMP packing 0->single thread packing)
fi

dnl Check for jpeg
AC_ARG_ENABLE([jpeg],
  [AC_HELP_STRING([--disable-jpeg],
                  [disable jpeg 2000 for grib 2 decoding/encoding])],
     without_jpeg=1,without_jpeg=0)

AC_ARG_WITH([jasper],
        [AC_HELP_STRING([--with-jasper=JASPER],
	    [use specified jasper installation directory])],
        jasper_dir=$withval, jasper_dir='system')

if test $jasper_dir != 'system'
then
  CFLAGS="$CFLAGS -I${jasper_dir}/include"
  LDFLAGS="$LDFLAGS -L${jasper_dir}/lib"
fi

AC_ARG_WITH([openjpeg],
        [AC_HELP_STRING([--with-openjpeg=OPENJPEG],
	    [use specified openjpeg installation directory])],
        openjpeg_dir=$withval, openjpeg_dir='system')

if test $openjpeg_dir != 'system'
then
  CFLAGS="$CFLAGS -I${openjpeg_dir}/include"
  LDFLAGS="$LDFLAGS -L${openjpeg_dir}/lib"
fi

if test $without_jpeg -ne 1
then
    AC_DEFINE(HAVE_JPEG,1,JPEG enabled)
    AC_CHECK_LIB(jasper,jas_stream_memopen,jasper_ok=1,jasper_ok=0)
    AC_CHECK_LIB(openjpeg,opj_image_create,openjpeg_ok=1,openjpeg_ok=0)

    jpeg_ok=0

    if test $jasper_ok -eq 1
    then
	  jpeg_ok=1
      LIB_JASPER='-ljasper'
      LIBS="$LIB_JASPER $LIBS"
      AC_DEFINE(HAVE_LIBJASPER,1,Define if you have JPEG version 2 "Jasper" library)
      AC_SUBST(LIB_JASPER)
	fi

	if test $openjpeg_ok -eq 1
	then
	  jpeg_ok=1
      LIB_OPENJPEG='-lopenjpeg'
      LIBS="$LIB_OPENJPEG $LIBS"
      AC_DEFINE(HAVE_LIBOPENJPEG,1,Define if you have JPEG version 2 "Openjpeg" library)
      AC_SUBST(LIB_OPENJPEG)
	fi

	if test $jpeg_ok -eq 0
	then
      AC_MSG_NOTICE([
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  CONFIGURATION ERROR: jasper required.                                                               
  jasper installation is not working or missing.                                 

  To fix this problem you have the following options.                                  
  1) Install without the jpeg support enabled (--disable-jpeg), but you won't be able 
     to decode grib 2 data encoded in jpeg.                       
  2) Check if you have jasper installed in a path different from your system path. 
     In this case you can provide your jasper installation path to the configure
     through the option --with-jasper="your_jasper_path"
  3) Download jasper from http://www.ece.uvic.ca/~mdadams/jasper/ and install it.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  ])
	  [ 0 -eq 1 ]
	  exit 
    fi
  JPEG_TEST="jpeg.sh"
  AC_SUBST(JPEG_TEST)
fi


dnl Check for png
AC_ARG_WITH([png-support],
  [AC_HELP_STRING([--with-png-support],
                  [add support for png decoding/encoding])],
     with_png=1,with_png=0)

if test $with_png -gt 0
then
    AC_MSG_CHECKING(for PNG )
    AC_MSG_RESULT()
    AC_CHECK_HEADER(png.h,passed=1,passed=0)
     AC_CHECK_LIB(png,png_read_png,passed=1,passed=0)
    AC_MSG_CHECKING(if PNG support package is complete)

    if test $passed -gt 0
    then
      LIB_PNG='-lpng'
      LIBS="$LIB_PNG $LIBS"
      AC_DEFINE(HAVE_LIBPNG,1,Define to 1 if you have the png library (-lpng))
      AC_SUBST(LIB_PNG)
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no -- some components failed test)
    fi
fi

dnl Perl installation directory

#PERL_INSTALL_OPTIONS="PREFIX=$prefix INSTALLDIRS=perl"
PERL_INSTALL_OPTIONS="LIB=$default_perl_install"

AC_ARG_ENABLE([install-system-perl],
        [AC_HELP_STRING([--enable-install-system-perl],
		                [perl modules will install in the standard perl installation ])],
        enable_perl_install='yes', enable_perl_install='no')

if test "$enable_perl_install" = 'yes'
then
    PERL_INSTALL_OPTIONS=""
fi

AC_SUBST(PERL_INSTALL_OPTIONS)


dnl Disable build/install of Perl grib_api.
AC_ARG_WITH([perl],
        [AC_HELP_STRING([--with-perl=PERL],
	    [use specified Perl binary to configure Perl grib_api])],
        with_perl=$withval, with_perl='no')

dnl Look for PERL if Perl requested
if test "$with_perl" != 'no'
then
  if test "$with_perl" != 'yes'
  then
    AC_CACHE_CHECK(for perl,ac_cv_path_PERL,ac_cv_path_PERL="$with_perl");
    PERL=$ac_cv_path_PERL
    AC_SUBST(PERL)dnl
  else
    AC_PATH_PROGS(PERL,perl perl5,perl)dnl
  fi
fi

dnl Get full paths
builddir=`pwd`

dnl Options to pass when configuring Perl grib_api
GRIB_API_LIB="${builddir}/src/grib_api.a"
GRIB_API_INC="${builddir}/src"
AC_ARG_WITH([perl-options],
  [AC_HELP_STRING([--with-perl-options=[OPTIONS]], [options to pass on command-line when
                          generating Perl grib_api's Makefile from Makefile.PL])],
  PERL_MAKE_OPTIONS=$withval)
AC_SUBST(PERL_MAKE_OPTIONS)
AC_SUBST(GRIB_API_LIB)
AC_SUBST(GRIB_API_INC)

AM_CONDITIONAL(WITH_PERL, test $with_perl != no)

dnl AC_DISABLE_SHARED
dnl AC_PROG_LIBTOOL

dnl Checks for ar and rm
AC_CHECK_PROG(RM, rm, rm)
AC_CHECK_TOOL(AR, ar, ar)

dnl Check if -pedantic available
grib_api_PROG_CC_WARNING_PEDANTIC([-Wall])

dnl Enable -Werror despite compiler version
grib_api_ENABLE_WARNINGS_ARE_ERRORS

dnl Checks for libraries
AC_CHECK_LIB(m,pow)

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h sys/param.h sys/time.h unistd.h math.h stdarg.h assert.h ctype.h fcntl.h])


dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

dnl Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([bzero gettimeofday])

AC_OUTPUT( Makefile src/Makefile fortran/Makefile tools/Makefile definitions/Makefile samples/Makefile ifs_samples/grib1/Makefile ifs_samples/grib1_mlgrib2/Makefile tests/Makefile examples/C/Makefile examples/F90/Makefile tigge/Makefile perl/GRIB-API/Makefile.PL perl/Makefile)

AC_MSG_NOTICE([

  Configuration completed.

    You can now say 'make' to compile the grib_api package, 'make check' to
	  test it  and 'make install' to install it afterwards.
	  ])
