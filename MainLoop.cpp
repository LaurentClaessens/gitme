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
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>     // starts_with
#include "MainLoop.h"
#include "GitRepository.h"
#include "Configuration.h"

// MAIN LOOP ----

MainLoop::MainLoop(Configuration* c_ptr) : config_ptr(c_ptr) {}

void MainLoop::run() 
{ 
    assert( is_directory(getStartingPath()) );
    std::cout<<"Launching the backup loop."<<std::endl;
    loopOverDirectory(getStartingPath()); 
    std::cout<<"Backup loop ended."<<std::endl;
}

void MainLoop::loopOverDirectory(path directory)
{
    assert(is_directory(directory));

    config_ptr->processEvents();      // a more reactive window.
    directory_iterator end_itr;
    for(  directory_iterator itr(directory); itr!=end_itr;++itr  )
    {
        path pathname=itr->path();
        if (is_symlink(pathname))
        {
            config_ptr->writeLog("I do not deal with symlink : "+ pathname.string());
        }
        else if (is_regular_file( pathname )) {}
        else if (is_directory( pathname )) { DealWithDirectory(pathname); }
        else
        {
            std::cout<<"pathname "<<pathname.string()<<std::endl;
            config_ptr->writeLog("I'm looping in the directory : "+directory.string());
            string message="I'm looping in the directory : "+directory.string();
            config_ptr->writeLog(message);
            throw std::string(message);
        }
    }
}

const path MainLoop::getStartingPath() const
{
    return config_ptr->getStartingBackupPath();
}

bool MainLoop::is_excluded(path dirname) 
{
    return config_ptr->is_excluded(dirname); 
}

void MainLoop::DealWithDirectory(path rep_path)
{
    if (rep_path.string()==".git") {}
    else 
    {
        GitRepository repo=GitRepository(rep_path);
        if (!repo.isClean())
        {
            config_ptr->addGitButton(repo);
        }
        if (!is_excluded(rep_path))
        {
            loopOverDirectory(rep_path); 
        }
    }
}
