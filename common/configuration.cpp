// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "configuration.h"

#include <varconf/Config.h>

varconf::Config * global_conf;

std::string etc_directory = ETCDIR;

bool loadConfig(int argc, char ** argv)
{
    global_conf = varconf::Config::inst();

#if 0
    // See if the user has set the install directory on the command line
    char * home;
    bool home_dir_config = false, store_config = false;
    if ((home = getenv("HOME")) != NULL) {
        home_dir_config = global_conf->readFromFile(std::string(home) + "/.apogee.vconf");
    }
    // Check the command line options, and if anything
    // has been overriden, store this value in the users home directory.
    // The effect of this code is that config settings, once
    // chosen are fixed.
    global_conf->getCmdline(argc, argv);
    if (global_conf->findItem("apogee", "directory")) {
        store_config = true;
        share_directory = global_conf->getItem("apogee", "directory");
    }
    if (global_conf->findItem("apogee", "confdir")) {
        store_config = true;
        etc_directory = global_conf->getItem("apogee", "confdir");
    }
    if (store_config && (home != NULL)) {
        global_conf->writeToFile(std::string(home) + "/.apogee.vconf");
    }
#endif
    // Load up the rest of the system config file, and then ensure that
    // settings are overridden in the users config file, and the command line
    bool main_config = global_conf->readFromFile(etc_directory +
                                                 "/apogee/apogee.vconf");
    if (!main_config) {
        std::cerr << "ERROR: Unable to read main config file "
                  << etc_directory << "/apogee/apogee.vconf."
                  << std::endl;
#if 0
        if (home_dir_config) {
            std::cerr << "Try removing .apogee.vconf from your home directory as it may specify an invalid installation directory, and then restart apogee."
                      << std::endl << std::flush;
        } else {
            std::cerr << "Please ensure that apogee has been installed correctly."
                      << std::endl << std::flush;
        }
#endif
        return true;
    }
    // if (home_dir_config) {
    char * home;
    if ((home = getenv("HOME")) != NULL) {
        global_conf->readFromFile(std::string(home) + "/.apogee.vconf");
    }
    global_conf->getCmdline(argc, argv);

    // Config is now loaded. Now set the values of some globals.

    return false;
}
