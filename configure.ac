dnl Initialise autoconf

AC_INIT()
AM_CONFIG_HEADER(config.h)

dnl Detect the canonical host and target environment

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Initialise automake

VERSION=0.0.2

AM_INIT_AUTOMAKE(equator, $VERSION)

dnl check if user wishes maintainer-mode ("--enable-maintainer-mode = yes")
AM_MAINTAINER_MODE

dnl Need libtool

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

dnl Test for C++ compiler

AC_PROG_CXX

AC_ARG_ENABLE(debug,
    [  --enable-debug          enable debug information [default=no]],
    [
        if test "$enableval" = "yes"; then
            CXXFLAGS="$CXXFLAGS -Wall -DDEBUG"
        else
            CXXFLAGS="$CXXFLAGS -Wall -DNDEBUG"
        fi
    ],[
        CXXFLAGS="$CXXFLAGS -Wall -DNDEBUG"
    ]
)

AC_CHECK_FUNC(rint,,[AC_CHECK_LIB(m,rint)])
AC_CHECK_FUNCS([rintf lrintf lrint])

AC_LANG_CPLUSPLUS
AC_REQUIRE_CPP

SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION)
CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

AC_CHECK_LIB(SDL_image,IMG_Load)
AC_CHECK_LIB(GL,glViewport)
AC_CHECK_LIB(GLU,gluPerspective)

PKG_CHECK_MODULES(WF, mercator-0.2 eris-discordia-1.2 coal-blade-0.4 coal-iso-0.4 ,
    [
        CPPFLAGS="$CPPFLAGS $WF_CFLAGS"
        LIBS="$LIBS $WF_LIBS"
    ],
    [
        AC_MSG_ERROR(Couldn't find WF libs.)
    ])

dnl Versions required

PKG_CHECK_MODULES(GTK, gtkglextmm-1.0 ,
    [
        CPPFLAGS="$CPPFLAGS $GTK_CFLAGS"
        LIBS="$LIBS $GTK_LIBS"
    ],
    [
        AC_MSG_ERROR(Couldn't find GTK libs.)
    ])

AC_MSG_CHECKING([for round in libm])
AC_TRY_COMPILE([
  #include <cmath>
],[
  round(0.1);
],[
  AC_MSG_RESULT(yes)
],[
    AC_MSG_RESULT(no)
    AC_MSG_CHECKING([for round in libm with _ISOC99_SOURCE])

    CXXFLAGS="$CXXFLAGS -D_ISOC99_SOURCE"

    AC_TRY_COMPILE([
      #include <math.h>
    ],[
      round(0.1);
    ],[
      AC_MSG_RESULT(yes)
    ],[
      AC_MSG_RESULT(no)
    ])

])


dnl Generate files
AC_OUTPUT([
	Makefile
	app/Makefile
])
