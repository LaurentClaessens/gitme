/*
Copyright 2015-2016 Laurent Claessens
contact : laurent@claessens-donadello.eu

This is part of 'gitme': you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
//*/


#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <iostream>
#include <boost/filesystem.hpp>
#include "HashTable.h"
#include "GitListWindow.h"
#include "GitRepository.h"
#include "Logging.h"

// The aim of this class is to answer the questions which depend on the
// configuration (excluded files, terminal command line, ...)
// It also holds a hand on the main windows in order to add buttons when
// a git need is found.
// This contains a direct sum of
// - excluded path,
// - terminal command line,
// - an handle on the git list window
// - a logging system
// and some functions to access all that.
//
// Let be clear once for all : 'config_ptr' is more or less a global variable : it has to be available almost everywhere, at least for logging purpose. Since I don't want a global variable, it is passed as argument to almost every objects in gitme.
class Configuration
{
    private:
        const path starting_backup_path;
        const TerminalLines* const terminal_lines_ptr;
        std::vector<path> excluded_paths;
        GitListWindow* git_list_window;
        Logging*  logging;
    public:
        Configuration(const path, const TerminalLines* const);
        path getStartingBackupPath() const;
        path getHomePath() const;
        path getBackupPath() const;
        path getPurgePath() const;

        // excluding a non-existing path throws an exception. 
        void add_exclude_path(const path, const bool verbose=false);
        void add_exclude_path(const std::vector<path>, const bool verbose=false); 

        bool is_excluded(const path) const;       


        void create_purge_directories() const;
        bool are_all_paths_ok() const;             

        const string getTerminal() const;
        const string getInTerminal() const;
        const string getEditor() const; 

        void setGitListWindow(GitListWindow*);
        void addGitButton(GitRepository);
        
        void setLog(Logging*);
        void setLogFile(string);
        void writeLog(string) const;

        void processEvents();
};

path get_starting_backup_path(int argc, char *argv[]);

// Read the given configuration file and return
// an HashTable whose keys are the "before the = sign" and values are vectors
// of string "after the = signe".
// example.
// if the configuration file contains
// foo=bla
// bar=blo
// foo=bli
// the returned hash table will satisfy
// ht["foo"]=(bla,bli)
// ht["bar"]=(blo)
HashTable<std::string,std::vector<std::string>> read_configuration_file(const path cfg_path);
Configuration* arguments_to_configuration(const int argc=0,char* argv[]=0, const bool verbose=true);
Configuration* configuration_file_to_configuration(const path cfg_path,const path starting_backup_path="", const bool verbose=true);


// The following returns the last found value (in the file) of the required property.
std::string read_configuration_file(const path cfg_path,const std::string searched_property);

path get_starting_backup_path(int,char*);

#endif     //__CONFIGURATION_H__
