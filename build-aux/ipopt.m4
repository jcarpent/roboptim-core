#                                                                -*-Autoconf-*-
# Copyright (C) 2009 by Thomas Moulard, AIST, CNRS, INRIA.
# This file is part of the roboptim.
#
# roboptim is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Additional permission under section 7 of the GNU General Public
# License, version 3 ("GPLv3"):
#
# If you convey this file as part of a work that contains a
# configuration script generated by Autoconf, you may do so under
# terms of your choice.
#
# roboptim is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with roboptim.  If not, see <http://www.gnu.org/licenses/>.

# serial 1

m4_pattern_forbid([^IPOPT_])

# IPOPT_LIB([action-if-found], [action-if-not-found])
# --------------------------------------------------------------
# Try to detect the library.
AC_DEFUN([IPOPT_LIB],
[
 #FIXME: insert a valid symbol.
 # Need gfortran library to work.
 AC_SEARCH_LIBS([main], [gfortran], $1, $2)

 # Detect pthread.
 ACX_PTHREAD

 #FIXME: replace symbol by CreateIpoptProblem
 AC_SEARCH_LIBS([main], [ipopt], $1, $2)
]) # LIB_IPOPT_LIB


# IPOPT_HEADERS([action-if-found], [action-if-not-found])
# ------------------------------------------------------------------
# Try to detect the library headers.
AC_DEFUN([IPOPT_HEADERS],
[AC_CHECK_HEADER([coin/IpIpoptApplication.hpp], $1, $2)
AC_CHECK_HEADER([coin/IpTNLP.hpp], $1, $2)
]) # LIB_IPOPT_HEADERS


# IPOPT_ARG_WITH
# ------------------------------
# Add an optional dependency toward Ipopt.
AC_DEFUN([IPOPT_ARG_WITH],
[
  AC_ARG_WITH([ipopt],
  [AS_HELP_STRING([--with-ipopt],
    [enable ipopt support])],
  [],
  [with_ipopt=check])

  AC_SUBST([DISTCHECK_CONFIGURE_FLAGS],
           ["$DISTCHECK_CONFIGURE_FLAGS \
	   '--with-ipopt=$with_ipopt'"])

  ipopt_fail=no
  AS_IF([test "x$with_ipopt" != xno],
    [
      # Search for Ipopt library.
      IPOPT_LIB([], [ipopt_fail=yes])

      # Search for Ipopt headers.
      IPOPT_HEADERS([], [ipopt_fail=yes])

      # If both tests are OK, add the library and define the CPP symbol.
      AS_IF([test "x$ipopt_fail" = xno],
	[AC_SUBST([IPOPT_LIBADD], ["-lipopt -lgfortran"])
	 AC_DEFINE([HAVE_IPOPT], [1],
                   [Define if you have ipopt])])

      # If the support was explicitly required, but detection has failed,
      # this is a fatal error.
      AS_IF([test "x$with_ipopt" = xyes && \
             test "x$ipopt_fail" = xyes],
        [AC_MSG_FAILURE(
        [Ipopt test failed (--without-ipopt to disable)])
        ])
    ])

  # Define a boolean indicating whether or not the package has been found.
  AM_CONDITIONAL([HAVE_IPOPT],
                 [test "x$with_ipopt" != xno && \
		  test "x$ipopt_fail" != xyes])
]) # IPOPT_ARG_WITH
