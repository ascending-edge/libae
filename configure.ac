m4_define(libae_version_triplet, m4_esyscmd_s([cat version.txt]))
m4_define(libae_version_list, m4_split(libae_version_triplet, [[.]]))
m4_define(libae_version_major, m4_argn(1, libae_version_list))
m4_define(libae_version_minor, m4_argn(2, libae_version_list))
m4_define(libae_version_age, m4_argn(3, libae_version_list))

AC_INIT([libae], [libae_version_triplet])
AC_PREREQ(2.69)
AC_COPYRIGHT([Copyright (C) 2017 Ascending Edge, LLC.])

AC_SUBST([LIBAE_MAJOR], libae_version_major)
AC_SUBST([LIBAE_MINOR], libae_version_minor)
AC_SUBST([LIBAE_AGE], libae_version_age)
AC_DEFINE([LIBAE_MAJOR], libae_version_major, [Major Version])
AC_DEFINE([LIBAE_MINOR], libae_version_minor, [Minor Version])
AC_DEFINE([LIBAE_AGE], libae_version_age, [Age])




AM_PROG_AR
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AC_CHECK_SIZEOF([void*])
AC_CHECK_HEADERS([obstack.h])
#AC_CONFIG_HEADERS([config.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CONFIG_FILES([Makefile libae/Makefile test/Makefile])
AM_SILENT_RULES([yes])
AC_OUTPUT


