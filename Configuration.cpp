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

#include <iostream>
#include <string>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "GitListWindow.h"
#include "Configuration.h"
#include "Logging.h"

using std::string;
using std::vector;

// CONFIGURATION -- general

Configuration::Configuration(const path sp, const TerminalLines* const term_lines_ptr):
    starting_backup_path(sp),
    terminal_lines_ptr(term_lines_ptr)
{}

path Configuration::getStartingBackupPath() const { return starting_backup_path; }

// CONFIGURATION -- excluding paths

void Configuration::add_exclude_path(const path ex, const bool verbose) 
{ 
    if ( is_directory(ex)  )
    {
        path can_ex=canonical(ex);
        excluded_paths.push_back(can_ex); 
    }
    else if (verbose)
    {
        std::cout<< "Trying to exclude a  directory that does not exist: "+ex.string()<<std::endl;
        //throw string(  "Trying to exclude a non-directory path : "+ex.string()  );
    }
}

void Configuration::add_exclude_path(const std::vector<path> vp,const bool verbose)
{
    for (auto p:vp) { add_exclude_path(p,verbose); }
}

bool Configuration::is_excluded(const path rep_path) const
{
    if (!exists(rep_path)) {return true;}
    for ( path p:excluded_paths )
    {
        if (p==canonical(rep_path)) {return true;}
    }
    return false;
} 

// CREATING FUNCTIONS (not to be confused with the constructor)

HashTable<string,std::vector<string> > read_configuration_file(const path cfg_path)
{
    assert(is_regular_file(cfg_path));
    auto hash_table=HashTable<std::string,std::vector<std::string>>();
    std::ifstream cfg_file(cfg_path.c_str());
    std::string line;
    std::vector<std::string> parts;
    while (  std::getline(cfg_file,line) )
    {
        split( parts,line,boost::is_any_of("=") );
        switch (parts.size())
        {
         case 0 : 
             throw std::string("Malformed configuration file : "+line); 
             break;
         case 1 :       // probably empty line
             break;
         case 2 :
             std::string key=parts[0];
             std::string value=parts[1];
            if (!hash_table.isKey(key))
            {
                hash_table[key]=std::vector<std::string>();
            }
            hash_table[key].push_back(value);
            break;
        }
    }
    return hash_table;
}

std::string read_configuration_file(const path cfg_path,const std::string searched_property)
{
    auto hash_table=read_configuration_file(cfg_path);

    for (auto itr=hash_table.begin();itr!=hash_table.end();itr++)
    {
        if (itr->key==searched_property)
        {
            std::vector<std::string> v=itr->value;
            if (!v.empty()) { return v.back(); }
        }
    }
    throw std::string("Property not found in the configuration file : "+searched_property);
}


// an argument with no "=" is interpreted 
// as the starting path.  This allows to use bash completion.
HashTable<string,string> parse_arguments(int argc,char* argv[])
{
    HashTable<string,string>hash=HashTable<string,string>();
    hash["--configuration"]="gitme.cfg";
    hash["--starting"]=getenv("HOME");
    for (int i=1;i<argc;i++)
    {
        std::vector<string> parts;
        string arg=argv[i];
        split( parts,arg,boost::is_any_of("=") );

        if (parts.size()==2)
        {
        hash[parts[0]]=parts[1];
        }
        else if (parts.size()==1)  
        {
            hash["--starting"]=arg;
        }
        else 
        {
            throw string("Malformed command line argument : "+arg);
        }

        string s_path=hash["--starting"];
        if (boost::algorithm::starts_with(s_path,"~"))
        {
            s_path.replace(0,1,getenv("HOME"));
        }
        hash["--starting"]=s_path;
    }
    return hash;
}

path get_starting_backup_path(string s_path,bool verbose=true)
{
    const path starting_path=path(s_path);
    path full_path;
    if (starting_path.is_relative()) { full_path=absolute(starting_path); }
    else { full_path=starting_path; }
    full_path=canonical(full_path);
    if (verbose)
    {
        std::cout<<"We are going to backup the repertory "<<full_path<<std::endl;
    }
    return full_path;
}

Configuration* configuration_file_to_configuration(const path cfg_path,const path starting_backup_path, const bool verbose)
{
    auto hash_table=read_configuration_file(cfg_path);


    path bp,pp,sp;
    const path home_path=path(getenv("HOME"));
    std::vector<path> exclude;

    sp=hash_table["starting"][0];

    for (std::string s_path:hash_table["exclude"])
    {
        path aux=path(s_path);
        if (aux.is_absolute()){ exclude.push_back(aux) ;}
        else { exclude.push_back(  home_path/aux ) ; }
    }
    if (verbose)
    {
        std::cout<<"starting directory is "<<sp<<std::endl;
    }

    if (starting_backup_path.string()!=""){sp=starting_backup_path;}


    string terminal=hash_table["terminal"][0];
    string in_terminal=hash_table["in_terminal"][0];
    string editor=hash_table["editor"][0];
    string log_filename=hash_table["log file"][0];


    const TerminalLines* const terminal_lines_ptr=new TerminalLines(terminal,in_terminal,editor);
    Logging* logging = new Logging();
    logging->setFile(log_filename);

    Configuration* config_ptr = new Configuration( sp,terminal_lines_ptr  );
    config_ptr->add_exclude_path(exclude,verbose);
    config_ptr->setLog(logging);

    return config_ptr;
}

Configuration* arguments_to_configuration(int argc, char* argv[],bool verbose)
{

    HashTable<string,string> hash_args=parse_arguments(argc,argv);

    path cfg_path=hash_args["--configuration"];
    path starting_backup_path=get_starting_backup_path(hash_args["--starting"],verbose);

    assert(is_regular_file(cfg_path));
    return configuration_file_to_configuration(cfg_path,starting_backup_path,verbose);
}

// GIT LIST WINDOWS

void Configuration::setGitListWindow(GitListWindow* gw)
{
    git_list_window=gw;
}

void Configuration::addGitButton(GitRepository repo)
{
    git_list_window->addGitButton(repo);
}

// TERMINAL LINES

const string Configuration::getTerminal() const {return terminal_lines_ptr->getTerminal();}
const string Configuration::getInTerminal() const {return terminal_lines_ptr->getInTerminal();}
const string Configuration::getEditor() const {return terminal_lines_ptr->getEditor();}

// OTHER UTILITIES FUNCTIONS

void Configuration::processEvents(){ git_list_window->processEvents(); }

// LOGGING

void Configuration::setLog(Logging* log) { logging=log;  }
void Configuration::setLogFile(string filename) { logging->setFile(filename);  }
void Configuration::writeLog(string message) const { logging->write(message); }
