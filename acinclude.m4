# Configure paths for GTK+
# Owen Taylor     97-11-3

dnl AM_PATH_GTK([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for GTK, and define GTK_CFLAGS and GTK_LIBS
dnl
AC_DEFUN(AM_PATH_GTK,
[dnl 
dnl Get the cflags and libraries from the gtk-config script
dnl
AC_ARG_WITH(gtk-prefix,[  --with-gtk-prefix=PFX   Prefix where GTK is installed (optional)],
            gtk_config_prefix="$withval", gtk_config_prefix="")
AC_ARG_WITH(gtk-exec-prefix,[  --with-gtk-exec-prefix=PFX Exec prefix where GTK is installed (optional)],
            gtk_config_exec_prefix="$withval", gtk_config_exec_prefix="")
AC_ARG_ENABLE(gtktest, [  --disable-gtktest       Do not try to compile and run a test GTK program],
		    , enable_gtktest=yes)

  for module in . $4
  do
      case "$module" in
         gthread) 
             gtk_config_args="$gtk_config_args gthread"
         ;;
      esac
  done

  if test x$gtk_config_exec_prefix != x ; then
     gtk_config_args="$gtk_config_args --exec-prefix=$gtk_config_exec_prefix"
     if test x${GTK_CONFIG+set} != xset ; then
        GTK_CONFIG=$gtk_config_exec_prefix/bin/gtk-config
     fi
  fi
  if test x$gtk_config_prefix != x ; then
     gtk_config_args="$gtk_config_args --prefix=$gtk_config_prefix"
     if test x${GTK_CONFIG+set} != xset ; then
        GTK_CONFIG=$gtk_config_prefix/bin/gtk-config
     fi
  fi

  AC_PATH_PROG(GTK_CONFIG, gtk-config, no)
  min_gtk_version=ifelse([$1], ,0.99.7,$1)
  AC_MSG_CHECKING(for GTK - version >= $min_gtk_version)
  no_gtk=""
  if test "$GTK_CONFIG" = "no" ; then
    no_gtk=yes
  else
    GTK_CFLAGS=`$GTK_CONFIG $gtk_config_args --cflags`
    GTK_LIBS=`$GTK_CONFIG $gtk_config_args --libs`
    gtk_config_major_version=`$GTK_CONFIG $gtk_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    gtk_config_minor_version=`$GTK_CONFIG $gtk_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    gtk_config_micro_version=`$GTK_CONFIG $gtk_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_gtktest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $GTK_CFLAGS"
      LIBS="$GTK_LIBS $LIBS"
dnl
dnl Now check if the installed GTK is sufficiently new. (Also sanity
dnl checks the results of gtk-config to some extent
dnl
      rm -f conf.gtktest
      AC_TRY_RUN([
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.gtktest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_gtk_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_gtk_version");
     exit(1);
   }

  if ((gtk_major_version != $gtk_config_major_version) ||
      (gtk_minor_version != $gtk_config_minor_version) ||
      (gtk_micro_version != $gtk_config_micro_version))
    {
      printf("\n*** 'gtk-config --version' returned %d.%d.%d, but GTK+ (%d.%d.%d)\n", 
             $gtk_config_major_version, $gtk_config_minor_version, $gtk_config_micro_version,
             gtk_major_version, gtk_minor_version, gtk_micro_version);
      printf ("*** was found! If gtk-config was correct, then it is best\n");
      printf ("*** to remove the old version of GTK+. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If gtk-config was wrong, set the environment variable GTK_CONFIG\n");
      printf("*** to point to the correct copy of gtk-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
#if defined (GTK_MAJOR_VERSION) && defined (GTK_MINOR_VERSION) && defined (GTK_MICRO_VERSION)
  else if ((gtk_major_version != GTK_MAJOR_VERSION) ||
	   (gtk_minor_version != GTK_MINOR_VERSION) ||
           (gtk_micro_version != GTK_MICRO_VERSION))
    {
      printf("*** GTK+ header files (version %d.%d.%d) do not match\n",
	     GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     gtk_major_version, gtk_minor_version, gtk_micro_version);
    }
#endif /* defined (GTK_MAJOR_VERSION) ... */
  else
    {
      if ((gtk_major_version > major) ||
        ((gtk_major_version == major) && (gtk_minor_version > minor)) ||
        ((gtk_major_version == major) && (gtk_minor_version == minor) && (gtk_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of GTK+ (%d.%d.%d) was found.\n",
               gtk_major_version, gtk_minor_version, gtk_micro_version);
        printf("*** You need a version of GTK+ newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** GTK+ is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the gtk-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of GTK+, but you can also set the GTK_CONFIG environment to point to the\n");
        printf("*** correct copy of gtk-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_gtk=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_gtk" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$GTK_CONFIG" = "no" ; then
       echo "*** The gtk-config script installed by GTK could not be found"
       echo "*** If GTK was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the GTK_CONFIG environment variable to the"
       echo "*** full path to gtk-config."
     else
       if test -f conf.gtktest ; then
        :
       else
          echo "*** Could not run GTK test program, checking why..."
          CFLAGS="$CFLAGS $GTK_CFLAGS"
          LIBS="$LIBS $GTK_LIBS"
          AC_TRY_LINK([
#include <gtk/gtk.h>
#include <stdio.h>
],      [ return ((gtk_major_version) || (gtk_minor_version) || (gtk_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding GTK or finding the wrong"
          echo "*** version of GTK. If it is not finding GTK, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the GTK package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps gtk gtk-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means GTK was incorrectly installed"
          echo "*** or that you have moved GTK since it was installed. In the latter case, you"
          echo "*** may want to edit the gtk-config script: $GTK_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     GTK_CFLAGS=""
     GTK_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(GTK_CFLAGS)
  AC_SUBST(GTK_LIBS)
  rm -f conf.gtktest
])

# Configure paths for GTK--
# Erik Andersen	30 May 1998
# Modified by Tero Pulkkinen (added the compiler checks... I hope they work..)
# Modified by Thomas Langen 16 Jan 2000 (corrected CXXFLAGS)

dnl Test for GTKMM, and define GTKMM_CFLAGS and GTKMM_LIBS
dnl   to be used as follows:
dnl AM_PATH_GTKMM([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl
AC_DEFUN(AM_PATH_GTKMM,
[dnl 
dnl Get the cflags and libraries from the gtkmm-config script
dnl
AC_ARG_WITH(gtkmm-prefix,[  --with-gtkmm-prefix=PREFIX
                          Prefix where GTK-- is installed (optional)],
            gtkmm_config_prefix="$withval", gtkmm_config_prefix="")
AC_ARG_WITH(gtkmm-exec-prefix,[  --with-gtkmm-exec-prefix=PREFIX
                          Exec prefix where GTK-- is installed (optional)],
            gtkmm_config_exec_prefix="$withval", gtkmm_config_exec_prefix="")
AC_ARG_ENABLE(gtkmmtest, [  --disable-gtkmmtest     Do not try to compile and run a test GTK-- program],
		    , enable_gtkmmtest=yes)

  if test x$gtkmm_config_exec_prefix != x ; then
     gtkmm_config_args="$gtkmm_config_args --exec-prefix=$gtkmm_config_exec_prefix"
     if test x${GTKMM_CONFIG+set} != xset ; then
        GTKMM_CONFIG=$gtkmm_config_exec_prefix/bin/gtkmm-config
     fi
  fi
  if test x$gtkmm_config_prefix != x ; then
     gtkmm_config_args="$gtkmm_config_args --prefix=$gtkmm_config_prefix"
     if test x${GTKMM_CONFIG+set} != xset ; then
        GTKMM_CONFIG=$gtkmm_config_prefix/bin/gtkmm-config
     fi
  fi

  AC_PATH_PROG(GTKMM_CONFIG, gtkmm-config, no)
  min_gtkmm_version=ifelse([$1], ,0.10.0,$1)

  AC_MSG_CHECKING(for GTK-- - version >= $min_gtkmm_version)
  no_gtkmm=""
  if test "$GTKMM_CONFIG" = "no" ; then
    no_gtkmm=yes
  else
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    GTKMM_CFLAGS=`$GTKMM_CONFIG $gtkmm_config_args --cflags`
    GTKMM_LIBS=`$GTKMM_CONFIG $gtkmm_config_args --libs`
    gtkmm_config_major_version=`$GTKMM_CONFIG $gtkmm_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    gtkmm_config_minor_version=`$GTKMM_CONFIG $gtkmm_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    gtkmm_config_micro_version=`$GTKMM_CONFIG $gtkmm_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_gtkmmtest" = "xyes" ; then
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_LIBS="$LIBS"
      CXXFLAGS="$CXXFLAGS $GTKMM_CFLAGS"
      LIBS="$LIBS $GTKMM_LIBS"
dnl
dnl Now check if the installed GTK-- is sufficiently new. (Also sanity
dnl checks the results of gtkmm-config to some extent
dnl
      rm -f conf.gtkmmtest
      AC_TRY_RUN([
#include <gtk--.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.gtkmmtest");

  /* HP/UX 0 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_gtkmm_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_gtkmm_version");
     exit(1);
   }

  if ((gtkmm_major_version != $gtkmm_config_major_version) ||
      (gtkmm_minor_version != $gtkmm_config_minor_version) ||
      (gtkmm_micro_version != $gtkmm_config_micro_version))
    {
      printf("\n*** 'gtkmm-config --version' returned %d.%d.%d, but GTK-- (%d.%d.%d)\n", 
             $gtkmm_config_major_version, $gtkmm_config_minor_version, $gtkmm_config_micro_version,
             gtkmm_major_version, gtkmm_minor_version, gtkmm_micro_version);
      printf ("*** was found! If gtkmm-config was correct, then it is best\n");
      printf ("*** to remove the old version of GTK--. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If gtkmm-config was wrong, set the environment variable GTKMM_CONFIG\n");
      printf("*** to point to the correct copy of gtkmm-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
/* GTK-- does not have the GTKMM_*_VERSION constants */
/* 
  else if ((gtkmm_major_version != GTKMM_MAJOR_VERSION) ||
	   (gtkmm_minor_version != GTKMM_MINOR_VERSION) ||
           (gtkmm_micro_version != GTKMM_MICRO_VERSION))
    {
      printf("*** GTK-- header files (version %d.%d.%d) do not match\n",
	     GTKMM_MAJOR_VERSION, GTKMM_MINOR_VERSION, GTKMM_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     gtkmm_major_version, gtkmm_minor_version, gtkmm_micro_version);
    }
*/
  else
    {
      if ((gtkmm_major_version > major) ||
        ((gtkmm_major_version == major) && (gtkmm_minor_version > minor)) ||
        ((gtkmm_major_version == major) && (gtkmm_minor_version == minor) && (gtkmm_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of GTK-- (%d.%d.%d) was found.\n",
               gtkmm_major_version, gtkmm_minor_version, gtkmm_micro_version);
        printf("*** You need a version of GTK-- newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** GTK-- is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the gtkmm-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of GTK--, but you can also set the GTKMM_CONFIG environment to point to the\n");
        printf("*** correct copy of gtkmm-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_gtkmm=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CXXFLAGS="$ac_save_CXXFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_gtkmm" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$GTKMM_CONFIG" = "no" ; then
       echo "*** The gtkmm-config script installed by GTK-- could not be found"
       echo "*** If GTK-- was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the GTKMM_CONFIG environment variable to the"
       echo "*** full path to gtkmm-config."
       echo "*** The gtkmm-config script was not available in GTK-- versions"
       echo "*** prior to 0.9.12. Perhaps you need to update your installed"
       echo "*** version to 0.9.12 or later"
     else
       if test -f conf.gtkmmtest ; then
        :
       else
          echo "*** Could not run GTK-- test program, checking why..."
          CXXFLAGS="$CXXFLAGS $GTKMM_CFLAGS"
          LIBS="$LIBS $GTKMM_LIBS"
          AC_TRY_LINK([
#include <gtk--.h>
#include <stdio.h>
],      [ return ((gtkmm_major_version) || (gtkmm_minor_version) || (gtkmm_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding GTK-- or finding the wrong"
          echo "*** version of GTK--. If it is not finding GTK--, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means GTK-- was incorrectly installed"
          echo "*** or that you have moved GTK-- since it was installed. In the latter case, you"
          echo "*** may want to edit the gtkmm-config script: $GTKMM_CONFIG" ])
          CXXFLAGS="$ac_save_CXXFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     GTKMM_CFLAGS=""
     GTKMM_LIBS=""
     ifelse([$3], , :, [$3])
     AC_LANG_RESTORE
  fi
  AC_SUBST(GTKMM_CFLAGS)
  AC_SUBST(GTKMM_LIBS)
  rm -f conf.gtkmmtest
])

dnl
dnl GNOME_XML_HOOK (script-if-xml-found, failflag)
dnl
dnl If failflag is "failure", script aborts due to lack of XML
dnl 
dnl Check for availability of the libxml library
dnl the XML parser uses libz if available too
dnl

AC_DEFUN([LIBXML_HOOK],[
	AC_PATH_PROG(LIBXML_CONFIG,xml-config,no)
	if test "$LIBXML_CONFIG" = no; then
		if test x$2 = xfailure; then
			AC_MSG_ERROR(Could not find xml-config)
		fi
	fi

	AC_CHECK_LIB(xml, xmlNewDoc, [
		$1
		LIBXML_CFLAGS=`$LIBXML_CONFIG --cflags`
		LIBXML_LIBS=`$LIBXML_CONFIG --libs`
		LIBXML_LIB_PATH=`$LIBXML_CONFIG --libs`
	], [
		if test x$2 = xfailure; then 
			AC_MSG_ERROR(Could not link sample xml program)
		fi
	], `$LIBXML_CONFIG --libs`)

	AC_SUBST(LIBXML_CFLAGS)
	AC_SUBST(LIBXML_LIBS)
	AC_SUBST(LIBXML_LIB_PATH)
])

AC_DEFUN([LIBXML_CHECK], [
	LIBXML_HOOK([],failure)
])

## Custom autoconf macros for COAL library

AC_DEFUN(WF_CHECK_COAL,
[
## Check for coal-config script (added in COAL 0.2.1)
AC_CHECK_PROGS(COAL_CONFIG, coal-config, "")
if test -n "$COAL_CONFIG"; then
  # If coal-config is installed...
  COAL_VERSION=`$COAL_CONFIG --version`

  if test -n "$1"; then
    # If a version number was passed into COAL_CHECK, we have some
    # more work to do...
    # 
    # (version-checking sed stuff stolen from gtk.m4)
dnl    echo "Found COAL version $COAL_VERSION"
    AC_MSG_CHECKING(for COAL version >= $1)

    if test "$1" = "$COAL_VERSION"; then
      AC_MSG_RESULT(yes)
    else
      # Version numbers don't match, so we'll have to pull some
      # tricks to get accurate results

      # Extract version numbers of installed COAL
      coal_major=`echo $COAL_VERSION | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
      coal_minor=`echo $COAL_VERSION | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
      coal_micro=`echo $COAL_VERSION | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

      # Extract desired version numbers
      need_major=`echo $1 | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
      need_minor=`echo $1 | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
      need_micro=`echo $1 | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

      # Test the easy cases first...
      if test $need_major -lt $coal_major; then
        AC_MSG_RESULT(yes)
      else
        if test $need_minor -lt $coal_minor; then
          AC_MSG_RESULT(yes)
        else
          if test $need_micro -lt $coal_micro; then
            AC_MSG_RESULT(yes)
          else
            AC_MSG_RESULT(no)
            AC_MSG_ERROR([

*** ERROR!!!
*** You need a version of COAL >= $1...
*** Please upgrade COAL before continuing.
])
          fi
        fi
      fi

    fi

    # Set up some other variables
    COAL_CFLAGS=`$COAL_CONFIG --cflags`
    COAL_LIBS=`$COAL_CONFIG --libs`
    AC_SUBST(COAL_CFLAGS)
    AC_SUBST(COAL_LIBS)
  fi
else
  # If coal-config is NOT installed...
  COAL_VERSION=""
fi

## Have to temporarily set the default compiler to C++
AC_LANG_CPLUSPLUS

TMP_CXXFLAGS=$CXXFLAGS
TMP_LIBS=$LIBS

CXXFLAGS="$CXXFLAGS $COAL_CFLAGS"
LIBS="$LIBS $COAL_LIBS"

AC_TRY_LINK(
[/* test for the existance of the class Coal::Component */
#include <coal/container.h>
],
[ Coal::Container* c = new Coal::Container(); ],
dnl Had to get rid of AC_CHECK_HEADER, it only checks in
dnl locations where system files are found, not locations
dnl specified by coal-config --cflags
  [],
  [ AC_MSG_ERROR(
      [
***
*** Unable to correctly link the COAL library...aborting!
***
      ]
    )
  ]
)

CXXFLAGS=$TMP_CXXFLAGS
LIBS=$TMP_LIBS

## Set default compile language back to C so we don't screw
## up the remaining configure tests
AC_LANG_C
])dnl

# Configure paths for wfmath
# Stolen by Dan Tomalesky
# Stolen by Michael Koch
# Adapted by Al Riddoch
# stolen from Sam Lantinga of SDL
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_WFMATH([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for wfmath, and define WFMATH_CFLAGS and WFMATH_LIBS
dnl
AC_DEFUN(AM_PATH_WFMATH,
[dnl 
dnl Get the cflags and libraries from the wfmath-config script
dnl
AC_LANG_CPLUSPLUS
AC_ARG_WITH(wfmath-prefix,[  --with-wfmath-prefix=PREFIX
                          Prefix where wfmath is installed (optional)],
            wfmath_prefix="$withval", wfmath_prefix="")
AC_ARG_WITH(wfmath-exec-prefix,[  --with-wfmath-exec-prefix=PREFIX
                          Exec prefix where wfmath is installed (optional)],
            wfmath_exec_prefix="$withval", wfmath_exec_prefix="")
AC_ARG_ENABLE(wfmathtest, [  --disable-wfmathtest     Do not try to compile and run a test wfmath program],
		    , enable_wfmathtest=yes)

  if test x$wfmath_exec_prefix != x ; then
     wfmath_args="$wfmath_args --exec-prefix=$wfmath_exec_prefix"
     if test x${WFMATH_CONFIG+set} != xset ; then
        WFMATH_CONFIG=$wfmath_exec_prefix/bin/wfmath-config
     fi
  fi
  if test x$wfmath_prefix != x ; then
     wfmath_args="$wfmath_args --prefix=$wfmath_prefix"
     if test x${WFMATH_CONFIG+set} != xset ; then
        WFMATH_CONFIG=$wfmath_prefix/bin/wfmath-config
     fi
  fi

  AC_PATH_PROG(WFMATH_CONFIG, wfmath-config, no)
  min_wfmath_version=ifelse([$1], ,0.3.11,$1)
  AC_MSG_CHECKING(for wfmath - version >= $min_wfmath_version)
  no_wfmath=""
  if test "$WFMATH_CONFIG" = "no" ; then
    no_wfmath=yes
  else
    WFMATH_CFLAGS=`$WFMATH_CONFIG $wfmathconf_args --cflags`
    WFMATH_LIBS=`$WFMATH_CONFIG $wfmathconf_args --libs`

    wfmath_major_version=`$WFMATH_CONFIG $wfmath_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    wfmath_minor_version=`$WFMATH_CONFIG $wfmath_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    wfmath_micro_version=`$WFMATH_CONFIG $wfmath_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_wfmathtest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $WFMATH_CFLAGS"
      LIBS="$LIBS $WFMATH_LIBS"
dnl
dnl Now check if the installed wfmath is sufficiently new. (Also sanity
dnl checks the results of wfmath-config to some extent)
dnl
      rm -f conf.wfmathtest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = (char *)malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int main (int argc, char *argv[])
{
  int major, minor, micro;
  char *tmp_version;

  /* This hangs on some systems (?)
  system ("touch conf.wfmathtest");
  */
  { FILE *fp = fopen("conf.wfmathtest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_wfmath_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_wfmath_version");
     exit(1);
   }

   if (($wfmath_major_version > major) ||
      (($wfmath_major_version == major) && ($wfmath_minor_version > minor)) ||
      (($wfmath_major_version == major) && ($wfmath_minor_version == minor) && ($wfmath_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'wfmath-config --version' returned %d.%d.%d, but the minimum version\n", $wfmath_major_version, $wfmath_minor_version, $wfmath_micro_version);
      printf("*** of wfmath required is %d.%d.%d. If wfmath-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If wfmath-config was wrong, set the environment variable WFMATH_CONFIG\n");
      printf("*** to point to the correct copy of wfmath-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_wfmath=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_wfmath" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$WFMATH_CONFIG" = "no" ; then
       echo "*** The wfmath-config script installed by wfmath could not be found"
       echo "*** If wfmath was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the WFMATH_CONFIG environment variable to the"
       echo "*** full path to wfmath-config."
     else
       if test -f conf.wfmathtest ; then
        :
       else
          echo "*** Could not run wfmath test program, checking why..."
          CFLAGS="$CFLAGS $WFMATH_CFLAGS"
          LIBS="$LIBS $WFMATH_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding wfmath or finding the wrong"
          echo "*** version of wfmath. If it is not finding wfmath, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means wfmath was incorrectly installed"
          echo "*** or that you have moved wfmath since it was installed. In the latter case, you"
          echo "*** may want to edit the wfmath-config script: $WFMATH_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     WFMATH_CFLAGS=""
     WFMATH_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(WFMATH_CFLAGS)
  AC_SUBST(WFMATH_LIBS)
  rm -f conf.wfmathtest
])
