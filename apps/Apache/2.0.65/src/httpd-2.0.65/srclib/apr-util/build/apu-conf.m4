dnl
dnl custom autoconf rules for APRUTIL
dnl

dnl
dnl APU_FIND_APR: figure out where APR is located
dnl
AC_DEFUN(APU_FIND_APR,[

  dnl use the find_apr.m4 script to locate APR. sets apr_found and apr_config
  APR_FIND_APR
  if test "$apr_found" = "no"; then
    AC_MSG_ERROR(APR could not be located. Please use the --with-apr option.)
  fi

  APR_BUILD_DIR="`$apr_config --installbuilddir`"

  dnl make APR_BUILD_DIR an absolute directory (we'll need it in the
  dnl sub-projects in some cases)
  APR_BUILD_DIR="`cd $APR_BUILD_DIR && pwd`"

  APR_INCLUDES="`$apr_config --includes`"
  APR_LIBS="`$apr_config --link-libtool --libs`"
  APR_SO_EXT="`$apr_config --apr-so-ext`"
  APR_LIB_TARGET="`$apr_config --apr-lib-target`"

  AC_SUBST(APR_INCLUDES)
  AC_SUBST(APR_LIBS)
  AC_SUBST(APR_BUILD_DIR)
])


dnl
dnl APU_TEST_EXPAT(directory): test if Expat is located in the specified dir
dnl
dnl if present: sets expat_include_dir, expat_libs, possibly expat_old
dnl
AC_DEFUN(APU_TEST_EXPAT,[
  AC_MSG_CHECKING(for Expat in ifelse($2,,$1,$2))

  expat_libtool=""

  if test -r "$1/lib/expat.h"; then
    dnl Expat 1.95.* distribution
    expat_include_dir="$1/lib"
    expat_ldflags="-L$1"
    expat_libs="-lexpat"
    expat_libtool="$1/libexpat.la"
  elif test -r "$1/include/expat.h" -a \
    -r "$1/lib/libexpat.la"; then
    dnl Expat 1.95.* installation (with libtool)
    expat_include_dir="$1/include"
    expat_ldflags="-L$1/lib"
    expat_libs="-lexpat"
    expat_libtool="$1/lib/libexpat.la"
  elif test -r "$1/include/expat.h" -a \
    -r "$1/lib64/libexpat.la"; then
    dnl Expat 1.95.* installation on certain 64-bit platforms (with libtool)
    expat_include_dir="$1/include"
    expat_ldflags="-L$1/lib64"
    expat_libs="-lexpat"
    expat_libtool="$1/lib64/libexpat.la"
  elif test -r "$1/include/expat.h" -a \
    -r "$1/lib/libexpat.a"; then
    dnl Expat 1.95.* installation (without libtool)
    dnl FreeBSD textproc/expat2
    expat_include_dir="$1/include"
    expat_ldflags="-L$1/lib"
    expat_libs="-lexpat"
  elif test -r "$1/xmlparse.h"; then
    dnl maybe an expat-lite. use this dir for both includes and libs
    expat_include_dir="$1"
    expat_ldflags="-L$1"
    expat_libs="-lexpat"
    expat_libtool="$1/libexpat.la"
    expat_old=yes
  elif test -r "$1/include/xmlparse.h" -a \
       -r "$1/lib/libexpat.a"; then
    dnl previously installed expat
    expat_include_dir="$1/include"
    expat_ldflags="-L$1/lib"
    expat_libs="-lexpat"
    expat_old=yes
  elif test -r "$1/include/xml/xmlparse.h" -a \
       -r "$1/lib/xml/libexpat.a"; then
    dnl previously installed expat
    expat_include_dir="$1/include/xml"
    expat_ldflags="-L$1/lib"
    expat_libs="-lexpat"
    expat_old=yes
  elif test -r "$1/include/xmltok/xmlparse.h"; then
    dnl Debian distribution
    expat_include_dir="$1/include/xmltok"
    expat_ldflags="-L$1/lib"
    expat_libs="-lxmlparse -lxmltok"
    expat_old=yes
  elif test -r "$1/include/xml/xmlparse.h" -a \
       -r "$1/lib/libexpat.a"; then
    dnl FreeBSD textproc/expat package
    expat_include_dir="$1/include/xml"
    expat_ldflags="-L$1/lib"
    expat_libs="-lexpat"
    expat_old=yes
  elif test -r "$1/xmlparse/xmlparse.h"; then
    dnl Expat 1.0 or 1.1 source directory
    expat_include_dir="$1/xmlparse"
    expat_ldflags="-L$1"
    expat_libs="-lexpat"
    expat_old=yes
  fi
  dnl ### test for installed Expat 1.95.* distros

  if test -n "$expat_include_dir"; then
    dnl ### more info about what we found there? version? using .la?
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
])


dnl
dnl APU_FIND_EXPAT: figure out where EXPAT is located (or use bundled)
dnl
AC_DEFUN(APU_FIND_EXPAT,[

AC_ARG_WITH([expat],
[  --with-expat=DIR        specify Expat location or 'builtin'], [
  if test "$withval" = "yes"; then
    AC_MSG_ERROR([a directory must be specified for --with-expat])
  elif test "$withval" = "no"; then
    AC_MSG_ERROR([Expat cannot be disabled (at this time)])
  elif test "$withval" = "builtin"; then
    abs_expatdir="`cd $srcdir/xml/expat && pwd`"
    if test -d $abs_expatdir/. -a ! -d xml/expat/.; then
      $mkdir_p xml/expat
    fi
    APU_TEST_EXPAT($abs_expatdir, xml/expat)
  else
    abs_expatdir="`cd $withval && pwd`"
    APU_TEST_EXPAT($abs_expatdir, $withval)
    if test -z "$expat_include_dir"; then
      AC_MSG_ERROR([Expat was not found (or recognized) in \"$withval\"])
    fi
  fi
])

if test -z "$expat_include_dir"; then
  for d in /usr /usr/local xml/expat-cvs xml/expat $srcdir/xml/expat ; do
    APU_TEST_EXPAT($d)
    if test -n "$expat_include_dir"; then
      dnl For /usr installs of expat, we can't specify -L/usr/lib
      if test "$d" = "/usr"; then
        expat_ldflags=""
      fi
      break
    fi
  done
fi
if test -z "$expat_include_dir"; then
  AC_MSG_ERROR([could not locate Expat. use --with-expat])
fi

dnl If this expat doesn't use libtool natively, we'll mimic it for our
dnl dependency library generation.
if test -z "$expat_libtool"; then
  expat_libtool="$expat_ldflags $expat_libs" 
fi

if test -n "$expat_old"; then
  AC_DEFINE(APR_HAVE_OLD_EXPAT, 1, [define if Expat 1.0 or 1.1 was found])
fi

dnl special-case the bundled distribution (use absolute dirs)
if test "$expat_include_dir" = "xml/expat/lib" -o "$expat_include_dir" = "xml/expat-cvs/lib"; then
  bundled_subdir="`echo $expat_include_dir | sed -e 's%/lib%%'`"
  APR_SUBDIR_CONFIG($bundled_subdir, [--prefix=$prefix --exec-prefix=$exec_prefix --libdir=$libdir --includedir=$includedir --bindir=$bindir])
  expat_include_dir=$top_builddir/$bundled_subdir/lib
  expat_ldflags="-L$top_builddir/$bundled_subdir/lib"
  expat_libs="-lexpat"
  expat_libtool=$top_builddir/$bundled_subdir/libexpat.la
  APR_XML_SUBDIRS="`echo $bundled_subdir | sed -e 's%xml/%%'`"
  APR_ADDTO(APRUTIL_EXPORT_LIBS, [$expat_libtool])
else
if test "$expat_include_dir" = "$abs_srcdir/xml/expat/include" -o "$expat_include_dir" = "$abs_srcdir/xml/expat/lib"; then
  dnl This is a bit of a hack.  This only works because we know that
  dnl we are working with the bundled version of the software.
  bundled_subdir="xml/expat"
  APR_SUBDIR_CONFIG($bundled_subdir, [--prefix=$prefix --exec-prefix=$exec_prefix --libdir=$libdir --includedir=$includedir --bindir=$bindir])
  expat_include_dir=$abs_srcdir/$bundled_subdir/lib
  expat_ldflags="-L$top_builddir/$bundled_subdir/lib"
  expat_libs="-lexpat"
  expat_libtool=$top_builddir/$bundled_subdir/libexpat.la
  APR_XML_SUBDIRS="`echo $bundled_subdir | sed -e 's%xml/%%'`"
  APR_ADDTO(APRUTIL_EXPORT_LIBS, [$expat_libtool])
else
  APR_ADDTO(APRUTIL_EXPORT_LIBS, [$expat_libs])
fi
fi
APR_XML_DIR=$bundled_subdir
APR_XML_EXPAT_OLD=$expat_old
AC_SUBST(APR_XML_SUBDIRS)
AC_SUBST(APR_XML_DIR)
AC_SUBST(APR_XML_EXPAT_OLD)

if test "$expat_include_dir" != "/usr/include"; then
  APR_ADDTO(APRUTIL_INCLUDES, [-I$expat_include_dir])
fi
APR_ADDTO(APRUTIL_LDFLAGS, [$expat_ldflags])
APR_ADDTO(APRUTIL_LIBS, [$expat_libtool])
])


dnl 
dnl Find a particular LDAP library
dnl
AC_DEFUN(APU_FIND_LDAPLIB,[
  if test ${apu_has_ldap} != "1"; then
    ldaplib=$1
    extralib=$2
    unset ac_cv_lib_${ldaplib}_ldap_init
    unset ac_cv_lib_${ldaplib}___ldap_init
    AC_CHECK_LIB(${ldaplib}, ldap_init, 
      [
        APR_ADDTO(APRUTIL_EXPORT_LIBS,[-l${ldaplib} ${extralib}])
        APR_ADDTO(APRUTIL_LIBS,[-l${ldaplib} ${extralib}])
        AC_CHECK_LIB(${ldaplib}, ldapssl_install_routines, apu_has_ldap_netscape_ssl="1", , ${extralib})
        AC_CHECK_LIB(${ldaplib}, ldap_start_tls_s, apu_has_ldap_starttls="1", , ${extralib})
        apu_has_ldap="1";
      ], , ${extralib})
  fi
])


dnl
dnl APU_FIND_LDAP: figure out where LDAP is located
dnl
AC_DEFUN(APU_FIND_LDAP,[

echo $ac_n "${nl}checking for ldap support..."

apu_has_ldap="0";
apu_has_ldap_netscape_ssl="0"
apu_has_ldap_starttls="0"

AC_ARG_WITH(ldap-include,[  --with-ldap-include=path  path to ldap include files with trailing slash])
AC_ARG_WITH(ldap-lib,[  --with-ldap-lib=path    path to ldap lib file])
AC_ARG_WITH(ldap,[  --with-ldap=library     ldap library to use],
  [
    save_cppflags="$CPPFLAGS"
    save_ldflags="$LDFLAGS"
    save_libs="$LIBS"
    if test -n "$with_ldap_include"; then
      CPPFLAGS="$CPPFLAGS -I$with_ldap_include"
      APR_ADDTO(APRUTIL_INCLUDES, [-I$with_ldap_include])
    fi
    if test -n "$with_ldap_lib"; then
      LDFLAGS="$LDFLAGS -L$with_ldap_lib"
      APR_ADDTO(APRUTIL_LDFLAGS, [-L$with_ldap_lib])
    fi

    LIBLDAP="$withval"
    if test "$LIBLDAP" = "yes"; then
dnl The iPlanet C SDK 5.0 is as yet untested... 
      APU_FIND_LDAPLIB("ldap50", "-lnspr4 -lplc4 -lplds4 -liutil50 -llber50 -lldif50 -lnss3 -lprldap50 -lssl3 -lssldap50")
      APU_FIND_LDAPLIB("ldapssl41", "-lnspr3 -lplc3 -lplds3")
      APU_FIND_LDAPLIB("ldapssl40")
      APU_FIND_LDAPLIB("ldapssl30")
      APU_FIND_LDAPLIB("ldapssl20")
      APU_FIND_LDAPLIB("ldap", "-llber")
      APU_FIND_LDAPLIB("ldap", "-llber -lresolv")
      APU_FIND_LDAPLIB("ldap", "-llber -lresolv -lsocket -lnsl")
      APU_FIND_LDAPLIB("ldap", "-ldl -lpthread")
    else
      APU_FIND_LDAPLIB($LIBLDAP)
      APU_FIND_LDAPLIB($LIBLDAP, "-lresolv")
      APU_FIND_LDAPLIB($LIBLDAP, "-lresolv -lsocket -lnsl")
      APU_FIND_LDAPLIB($LIBLDAP, "-ldl -lpthread")
    fi

    test ${apu_has_ldap} != "1" && AC_MSG_ERROR(could not find an LDAP library)
    AC_CHECK_LIB(lber, ber_init)

    AC_CHECK_HEADERS(lber.h, lber_h=["#include <lber.h>"])
    AC_CHECK_HEADERS(ldap.h, ldap_h=["#include <ldap.h>"], [],
[#if HAVE_LBER_H
# include <lber.h>
# endif
])
    AC_CHECK_HEADERS(ldap_ssl.h, ldap_ssl_h=["#include <ldap_ssl.h>"])

    CPPFLAGS=$save_cppflags
    LDFLAGS=$save_ldflags
    LIBS=$save_libs
  ])

AC_SUBST(ldap_h)
AC_SUBST(lber_h)
AC_SUBST(ldap_ssl_h)
AC_SUBST(apu_has_ldap_netscape_ssl)
AC_SUBST(apu_has_ldap_starttls)
AC_SUBST(apu_has_ldap)

])
