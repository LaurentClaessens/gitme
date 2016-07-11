/*
Copyright 2015,2016 Laurent Claessens
contact : laurent@claessens-donadello.eu

This program is free software: you can redistribute it and/or modify
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

#include <QtGui>
#include "gitme.h"
#include "GitListWindow.h"
#include "Configuration.h"

using namespace boost::filesystem;
using namespace std;

void launchLoop(Configuration* config_ptr)
{
    MainLoop* main_loop=new MainLoop(config_ptr);
    main_loop->run();
    cout<<endl<<"Main loop launched."<<endl;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    try
    {    
        Configuration* config_ptr=arguments_to_configuration(argc,argv);          // There is the file 'example.cfg' as example.

        GitListWindow* git_list_window=new GitListWindow(config_ptr);
        config_ptr->setGitListWindow(git_list_window);
        git_list_window->show();

        launchLoop(config_ptr);
        
        git_list_window->join();
    }
    catch (string err) { cout<<endl<<string("I got a bad news : ")<<err<<endl; }
    catch (std::length_error &err) 
    { 
        cerr<<"Caught : "<<err.what()<<endl;
        cerr<<"Type : "<<typeid(err).name()<<endl;
    }
    return EXIT_SUCCESS;
}

// segfault on what():  boost::filesystem::copy_file: Permesso negato:
