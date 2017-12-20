#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(doublecross-cpp, 0.01, habets@google.com)
AC_CANONICAL_SYSTEM
AC_CONFIG_SRCDIR([haskell/cat.cc])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AC_CONFIG_HEADER(config.h)


# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# --enable-silent-rules
m4_ifdef([AM_SILENT_RULES],
  [AM_SILENT_RULES([yes])],
  [AC_SUBST([AM_DEFAULT_VERBOSITY], [1])])
    
CXXFLAGS="$CXXFLAGS -std=gnu++11"

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "
  $PACKAGE_NAME version $PACKAGE_VERSION
  Prefix.........: $prefix
  Debug Build....: $debug
  C++ Compiler...: $CXX $CXXFLAGS $CPPFLAGS
  Linker.........: $LD $LDFLAGS $LIBS
  "
