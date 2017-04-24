TEMPLATE = app

TARGET = CQTty

DEPENDPATH += .

INCLUDEPATH += . ../include

QMAKE_CXXFLAGS += -std=c++11

CONFIG += debug

MOC_DIR = .moc

QT += widgets

# Input
SOURCES += \
main.cpp \
CQTty.cpp \
CTty.cpp \
CQPageText.cpp \
CQPageTextState.cpp \
CPageText.cpp \
CPageTextEscapeNotifier.cpp \
CEscape.cpp \
CEscapeColors.cpp \
CEscapeHandler.cpp \
CEscapeParse.cpp \
CLogFile.cpp \

HEADERS += \
CQTty.h \
CTty.h \
CQPageText.h \
CQPageTextState.h \
CPageTextEscapeNotifier.h \
CPageText.h \
CEscapeColors.h \
CEscape.h \
CEscapeHandler.h \
CEscapeParse.h \
CEscapeType.h \
CCellStyle.h \
CTextPos.h \
CLogFile.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
../include \
../../CQUtil/include \
../../CImageLib/include \
../../CFont/include \
../../CConfig/include \
../../CFileUtil/include \
../../CFile/include \
../../CMath/include \
../../CRegExp/include \
../../CStrUtil/include \
../../CUtil/include \
../../CRGBName/include \
../../COS/include \

unix:LIBS += \
-L$$LIB_DIR \
-L../../CQUtil/lib \
-L../../CImageLib/lib \
-L../../CFont/lib \
-L../../CConfig/lib \
-L../../CFileUtil/lib \
-L../../CFile/lib \
-L../../CMath/lib \
-L../../CRegExp/lib \
-L../../CStrUtil/lib \
-L../../CRGBName/lib \
-L../../COS/lib \
-L../../CUtil/lib \
-lCQUtil \
-lCImageLib \
-lCFont \
-lCConfig \
-lCFileUtil \
-lCFile \
-lCMath \
-lCRGBName \
-lCRegExp \
-lCStrUtil \
-lCUtil \
-lCOS \
-ljpeg -lpng \
-ltre -lcurses
