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
AC_PROG_LN_S

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
WF_CHECK_SDL_IMAGE($SDL_VERSION)

AC_LANG_PUSH(C)

AC_CHECK_LIB(GL,glViewport, [ LIBS="$LIBS -lGL" ],
    AC_CHECK_LIB(opengl32,glViewport, [ LIBS="$LIBS -lopengl32" ],
      AC_MSG_ERROR(Could not find OpenGL library)
    )
)
AC_CHECK_LIB(GLU,gluPerspective, [ LIBS="$LIBS -lGLU" ],
    AC_CHECK_LIB(glu32,gluPerspective, [ LIBS="$LIBS -lglu32" ],
        AC_MSG_ERROR(Could not find OpenGL U library)
    )
)

AC_LANG_POP(C)

PKG_CHECK_MODULES(WF, mercator-0.2 eris-1.2 varconf-1.0 sage ,
    [
        CPPFLAGS="$CPPFLAGS $WF_CFLAGS"
        LIBS="$LIBS $WF_LIBS"
    ],
    [
        AC_MSG_ERROR(Couldn't find WF libs.)
    ])

AM_PATH_LIB3DS(1.2.0, , AC_MSG_ERROR(Couldn't find lib3ds))
CPPFLAGS="$CPPFLAGS $LIB3DS_CFLAGS"
LIBS="$LIBS $LIB3DS_LIBS"

AC_CHECK_LIB(cal3d, main,
[
    LIBS="$LIBS -lcal3d"
], AC_MSG_ERROR(Could not find cal3d library))

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

dnl add SDL flags at the end to avoid SDL_main problems
CXXFLAGS="$CXXFLAGS $WF_SDL_IMAGE_CFLAGS"
LIBS="$LIBS $WF_SDL_IMAGE_LIBS"

dnl Generate files
AC_OUTPUT([
	Makefile
	common/Makefile
	visual/Makefile
	app/Makefile
	gui/Makefile
	gui/gtkmm/Makefile
])
