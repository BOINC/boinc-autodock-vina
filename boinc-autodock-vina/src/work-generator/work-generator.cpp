// This file is part of BOINC.
// https://boinc.berkeley.edu
// Copyright (C) 2021 University of California
//
// BOINC is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// BOINC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BOINC.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>

#include <work-generator/input-config.h>

#ifndef BOINC_AUTODOCK_VINA_VERSION
#define BOINC_AUTODOCK_VINA_VERSION "unknown"
#endif

#ifndef BOINC_APPS_GIT_REVISION
#define BOINC_APPS_GIT_REVISION "unknown"
#endif

// Dependencies:
//
// ADFR: https://ccsb.scripps.edu/adfr/downloads/
// Open Babel: https://open-babel.readthedocs.io/en/latest/Installation/install.html#install-binaries
// For Windows additionally: https://www.lfd.uci.edu/~gohlke/pythonlibs/#openbabel
// Meeko: https://autodock-vina.readthedocs.io/en/latest/docking_requirements.html#meeko

void help() {
    std::cout << "Usage:" << std::endl;
    std::cout << "work-generator DIR" << std::endl;
}

inline void header() {
    std::cout << "Starting BOINC Autodock Vina work generator v" << BOINC_AUTODOCK_VINA_VERSION;
    std::cout << " (" << BOINC_APPS_GIT_REVISION << ")" << std::endl;
}

int main(int argc, char** argv) {
    header();

    if (argc != 2) {
        help();
        return 1;
    }

    return 0;
}
