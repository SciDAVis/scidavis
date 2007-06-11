######################################################################
# Automatically generated by qmake (2.01a) So Mai 13 22:04:04 2007
######################################################################

QMAKE_CXX = distcc
TEMPLATE = app
TARGET = 
DEPENDPATH += . ..
INCLUDEPATH += . ..
CONFIG += debug

# Input
HEADERS += AbstractColumnData.h \
           DateTimeColumnData.h \
           DoubleColumnData.h \
           StringColumnData.h \
		   AbstractDataSource.h \
		   AbstractFilter.h \
		   AbstractDateTimeDataSource.h \
		   AbstractDoubleDataSource.h \
		   AbstractStringDataSource.h \
           TableModel.h \
           TableView.h \
           CopyThroughFilter.h \
           Double2StringFilter.h \
           String2DoubleFilter.h \
           DateTime2StringFilter.h \
           String2DateTimeFilter.h \
		   TableItemDelegate.h 

SOURCES += main.cpp TableModel.cpp TableView.cpp 

SOURCES += StringColumnData.cpp \
           DateTimeColumnData.cpp \
		   DoubleColumnData.cpp \
		   AbstractFilter.cpp \
           String2DateTimeFilter.cpp \
		   TableItemDelegate.cpp 

