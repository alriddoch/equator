dnl Initialise autoconf

AC_INIT([equator], [0.1.1], [alriddoch@googlemail.com])
AC_CONFIG_SRCDIR([app/equator_plugin.cpp])
AC_CONFIG_HEADERS([config.h])

dnl Detect the canonical host environment

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Initialise automake

AM_INIT_AUTOMAKE([dist-bzip2 check-news])

dnl Test for C++ compiler

AC_PROG_CXX
AC_PROG_CPP
AC_PROG_RANLIB
AC_PROG_LN_S

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

STATIC_LIBSTDCPP=
STATIC_LIBGCC=

AC_ARG_ENABLE(static_link,
  [  --enable-static-link    enable static link of some libs [default=no]],
  [
    if test "$enableval" = "yes"; then
      link_static="true"
      STATIC_LIBSTDCPP=`$CXX -print-file-name=libstdc++.a`
      STATIC_LIBGCC=`$CXX -print-file-name=libgcc.a`
    else
      link_static="false"
    fi
  ],[
    link_static="false"
  ]
)
AM_CONDITIONAL(BUILD_STATIC, test x"$link_static" = xtrue)

AC_LANG(C++)

AC_CHECK_FUNC(rint,,[AC_CHECK_LIB(m,rint)])
AC_CHECK_FUNCS([rintf lrintf lrint])

dnl Check SDL
SDL_VERSION=1.2.2
AM_PATH_SDL($SDL_VERSION, [
  CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
  EQUATOR_LIBS="$EQUATOR_LIBS $SDL_LIBS"
],
[
  AC_MSG_ERROR(Couldn't find SDL or insufficent version found. Please goto http://www.libsdl.org/ and get at least version $SDL_VERSION)
])
AC_SUBST(SDL_VERSION)

if test $cross_compiling == no; then
  AC_CHECK_LIB(SDL_image,IMG_ReadXPMFromArray, [
    EQUATOR_LIBS="$EQUATOR_LIBS -lSDL_image"
  ], [
    AC_MSG_ERROR(Couldn't find SDL_image or wrong version found. Please goto http://www.libsdl.org/projects/SDL_image and get at least version 1.2.1)
])
  AC_CHECK_LIB(SDL_mixer,Mix_OpenAudio, [
    EQUATOR_LIBS="$EQUATOR_LIBS -lSDL_mixer"
  ], [
    AC_MSG_ERROR(Couldn't find SDL_mixer. Please goto http://www.libsdl.org/projects/SDL_mixer/)
])
else
   AC_CHECK_HEADER(SDL/SDL_image.h,
       [found_sdl_image=yes break],
       [found_sdl_image=no])

   if test $found_sdl_image == yes; then
       EQUATOR_LIBS="$EQUATOR_LIBS -lSDL_image"
   else
       AC_MSG_ERROR(Error could not find SDL_image)
   fi

   AC_CHECK_HEADER(SDL/SDL_mixer.h,
       [found_sdl_mixer=yes break],
       [found_sdl_mixer=no])

   if test $found_sdl_mixer == yes; then
       EQUATOR_LIBS="$EQUATOR_LIBS -lSDL_mixer"
   else
       AC_MSG_ERROR(Error could not find SDL_mixer)
   fi
fi

if test $cross_compiling == no; then
  AC_CHECK_LIB(GL,glViewport, [
    EQUATOR_LIBS="$EQUATOR_LIBS -lGL"
  ],[
    AC_MSG_ERROR(Couldn't find OpenGL)
])

  AC_CHECK_LIB(GLU,gluPerspective, [
    EQUATOR_LIBS="$EQUATOR_LIBS -lGLU"
  ],[
    AC_MSG_ERROR(Couldn't find GLU)
])

 else
   AC_CHECK_HEADER(GL/gl.h,
       [found_gl=yes break],
       [found_gl=no])

 if test $found_gl == yes; then
       EQUATOR_LIBS="$EQUATOR_LIBS -lopengl32"
   else
       AC_MSG_ERROR(Error could not find OpenGL)
   fi

   AC_CHECK_HEADER(GL/glu.h,
       [found_glu=yes break],
       [found_glu=no])

   if test $found_glu == yes; then
       EQUATOR_LIBS="$EQUATOR_LIBS -lglu32"
   else
       AC_MSG_ERROR(Error could not find GLU)
   fi
fi
LIBS="$LIBS $EQUATOR_LIBS"

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES(WF, eris-1.3 >= 1.3.5 mercator-0.2 >= 0.2.2 varconf-1.0 sage ,
    [
        CPPFLAGS="$CPPFLAGS $WF_CFLAGS"
        EQUATOR_LIBS="$EQUATOR_LIBS $WF_LIBS"
    ],
    [
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([
Cannot find valid versions of required WorldForge libraries:
$WF_PKG_ERRORS
Perhaps you should add the directory containing the missing libraries to the
PKG_CONFIG_PATH environment variable.
Please see http://worldforge.org/dev/eng/clients/equator for details of the
libraries required and where to obtain them.])
    ])

AM_PATH_LIB3DS(1.2.0, , AC_MSG_ERROR(Couldn't find lib3ds))
CPPFLAGS="$CPPFLAGS $LIB3DS_CFLAGS"
EQUATOR_LIBS="$EQUATOR_LIBS $LIB3DS_LIBS"

PKG_CHECK_MODULES(LIBMD3, libmd3-0.2,
    [
        CPPFLAGS="$CPPFLAGS $LIBMD3_CFLAGS"
        EQUATOR_LIBS="$EQUATOR_LIBS $LIBMD3_LIBS"
    ],
    [
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([
Cannot find valid version of required library, libmodelfile:
$LIBMD3_PKG_ERRORS
Perhaps you should add the directory containing the missing libraries to the
PKG_CONFIG_PATH environment variable.
Please see http://worldforge.org/dev/eng/clients/equator for details of the
libraries required and where to obtain them.])
    ])

PKG_CHECK_MODULES(CAL3D, cal3d >= 0.10.0,
    [
        CPPFLAGS="$CPPFLAGS $CAL3D_CFLAGS"
        EQUATOR_LIBS="$EQUATOR_LIBS $CAL3D_LIBS"
    ],
    [
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([
Cannot find valid version of required library, cal3d:
$CAL3D_PKG_ERRORS
Perhaps you should add the directory containing the missing libraries to the
PKG_CONFIG_PATH environment variable.
Please see http://worldforge.org/dev/eng/clients/equator for details of the
libraries required and where to obtain them.])
    ])

dnl Versions required

PKG_CHECK_MODULES(GTK, gtkglextmm-1.2 ,
    [
        CPPFLAGS="$CPPFLAGS $GTK_CFLAGS"
        EQUATOR_LIBS="$EQUATOR_LIBS $GTK_LIBS"
    ],
    [
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([
Cannot find valid version of required library, gtkglextmm:
$GTK_PKG_ERRORS
Perhaps you should add the directory containing the missing libraries to the
PKG_CONFIG_PATH environment variable.
Please see http://worldforge.org/dev/eng/clients/equator for details of the
libraries required and where to obtain them.])
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

dnl SDL adds itself to LIBS
LIBS=

AC_SUBST(EQUATOR_LIBS)
AC_SUBST(STATIC_LIBSTDCPP)
AC_SUBST(STATIC_LIBGCC)

dnl add SDL flags at the end to avoid SDL_main problems
dnl CXXFLAGS="$CXXFLAGS $WF_SDL_IMAGE_CFLAGS"
dnl LIBS="$LIBS $WF_SDL_IMAGE_LIBS"

dnl Generate files
AC_CONFIG_FILES([
	equator.spec
	Makefile
	common/Makefile
	visual/Makefile
	app/Makefile
	gui/Makefile
	gui/gtkmm/Makefile
	autopackage/default.apspec
])
AC_OUTPUT
