TEMPLATE	= lib
#CONFIG		= qt warn_on debug
#CONFIG		= qt warn_on release
CONFIG		= staticlib warn_on release
HEADERS		= cqml.h mainwin.h
SOURCES		= cqml.cpp mainwin.cpp
INCLUDEPATH	+= $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include
LIBS            +=
INTERFACES	= 
TARGET		= cqml

# vim:ts=8
