#
# $Id: doxygen.m4,v 1.1 2003/07/09 21:56:03 aleix Exp $
#
# Author: Aleix Conchillo Flaque <aleix@member.fsf.org>
# Start date: Sun Jul 06, 2003 04:41
#
# Copyright (C) 2003 Aleix Conchillo Flaque
#

# Locate doxygen and auxiliary programs.
# SCEW_DOXYGEN declares the configure arguments
#     doxygen
#     dot
#     html-docs
#     latex-docs
# The conditional macro DOC marks whether doxygen is enabled
# (or found, if not disabled). In the doxyfile.in file,
#     @enable_dot@
#     @enable_html_docs@
#     @enable_latex_docs@
# can be used to setup the documentation's generation.

AC_DEFUN(SCEW_DOXYGEN,
[
AC_ARG_ENABLE(doxygen,
[  --enable-doxygen        enable documentation generation with doxygen (auto)])
AC_ARG_ENABLE(dot,
[  --enable-dot            use 'dot' to generate graphs in doxygen (auto)])
AC_ARG_ENABLE(html-docs,
[  --enable-html-docs      enable HTML generation with doxygen (yes)], [], [ enable_html_docs=yes])
AC_ARG_ENABLE(latex-docs,
[  --enable-latex-docs     enable LaTeX documentation generation with doxygen (no)], [], [ enable_latex_docs=no])

if test "x$enable_doxygen" = xno; then
        enable_doc=no
else
        AC_PATH_PROG(DOXYGEN, doxygen, , $PATH)
        if test x$DOXYGEN = x; then
                if test "x$enable_doxygen" = xyes; then
                        AC_MSG_ERROR([could not find doxygen])
                fi
                enable_doc=no
        else
                enable_doc=yes
                AC_PATH_PROG(DOT, dot, , $PATH)
        fi
fi
AM_CONDITIONAL(DOC, test x$enable_doc = xyes)

if test x$DOT = x; then
        if test "x$enable_dot" = xyes; then
                AC_MSG_ERROR([could not find dot])
        fi
        enable_dot=no
else
        enable_dot=yes
fi
AC_SUBST(enable_dot)
AC_SUBST(enable_html_docs)
AC_SUBST(enable_latex_docs)
])
