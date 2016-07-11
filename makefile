
####### Compiler, tools and options

CXX           = LC_ALL=C g++ -std=c++11    #c++11 for  std::to_string
MOC_BIN = /usr/lib/i386-linux-gnu/qt4/bin/moc
DEFINES       = -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -I.
BOOST_SYSTEM  = -lboost_filesystem -lboost_system 
BOOST_THREAD  = -lboost_thread
BOOST_THREAD_LIB  = /usr/lib/i386-linux-gnu/libboost_thread.so 			# You should modify this line to fit your installation.
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS)  -L/usr/lib/i386-linux-gnu -lQtGui -lQtCore -lpthread 
DEL_FILE      = rm -f
DEL_DIR       = rmdir
MOVE          = mv -f

####### Implicit rules

clean: compiler_moc_source_clean
	-$(DEL_FILE) *.o
	-$(DEL_FILE) *~ core *.core

compiler_moc_source_clean:
	-$(DEL_FILE) moc_*

####### Compile
all:   gitme UnitTests 
gitme: gitme.cpp  \
		 MainLoop.o Configuration.o GitRepository.o CommandLine.o GitWindow.o GitListWindow.o  Logging.o
	$(CXX)  $(CXXFLAGS) $(INCPATH) -o gitme MainLoop.o Logging.o CommandLine.o GitRepository.o Configuration.o GitWindow.o moc_GitWindow.o GitListWindow.o moc_GitListWindow.o  $(BOOST_SYSTEM)  $(BOOST_THREAD)  gitme.cpp  $(BOOST_THREAD_LIB) $(LIBS)
UnitTests: UnitTests.cpp\
   	testing.o CommandLine.o HashTable.o GitRepository.o GitWindow.o Configuration.o
	$(CXX) $(LFLAGS)  $(CXXFLAGS) $(INCPATH) -o UnitTests GitRepository.o Logging.o Configuration.o testing.o  CommandLine.o GitWindow.o GitListWindow.o moc_GitListWindow.o  moc_GitWindow.o $(BOOST_SYSTEM) UnitTests.cpp  $(LIBS) 

GitListWindow.o: moc_GitListWindow.o GitListWindow.cpp GitListWindow.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o GitListWindow.o     GitListWindow.cpp
moc_GitListWindow.cpp: GitListWindow.h
	$(MOC_BIN) $(DEFINES) $(INCPATH) GitListWindow.h -o moc_GitListWindow.cpp
moc_GitListWindow.o: moc_GitListWindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_GitListWindow.o moc_GitListWindow.cpp

MainLoop.o: MainLoop.cpp MainLoop.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o MainLoop.o MainLoop.cpp

Configuration.o: Configuration.cpp Configuration.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Configuration.o  Configuration.cpp

Logging.o: Logging.cpp Logging.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Logging.o  Logging.cpp

CommandLine.o: CommandLine.cpp CommandLine.h \
		HashTable.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o CommandLine.o $(BOOST_SYSTEM) CommandLine.cpp


GitRepository.o: GitRepository.cpp GitRepository.h \
		CommandLine.o \
		HashTable.o
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o GitRepository.o  $(BOOST_SYSTEM)    GitRepository.cpp

GitWindow.o: GitWindow.cpp GitWindow.h moc_GitWindow.o
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o GitWindow.o  $(BOOST_SYSTEM)    GitWindow.cpp
moc_GitWindow.cpp: GitWindow.h
	$(MOC_BIN) $(DEFINES) $(INCPATH) GitWindow.h -o moc_GitWindow.cpp
moc_GitWindow.o: moc_GitWindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_GitWindow.o moc_GitWindow.cpp

testing.o: testing.cpp testing.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o testing.o testing.cpp
HashTable.o:  HashTable.h			
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o HashTable.o HashTable.h

