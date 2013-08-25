TEMPLATE = app

TARGET = 
DEPENDPATH += .
INCLUDEPATH += . ../include

CONFIG += debug

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
CEscapeHandler.cpp \
CEscapeParse.cpp \
CQColorChooser.cpp \
CQAlphaButton.cpp \
CQIntegerEdit.cpp \
CQWindow.cpp \
CWindow.cpp \
CQImage.cpp \
CQFont.cpp \
CQUtil.cpp \
CEvent.cpp \
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
CQColorChooser.h \
CQAlphaButton.h \
CQIntegerEdit.h \
CQWindow.h \
CWindow.h \
CQImage.h \
CQFont.h \
CQUtil.h \
CEvent.h \
CLogFile.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
../include \
../../CImageLib/include \
../../CFont/include \
../../CConfig/include \
../../CFile/include \
../../CMath/include \
../../CRegExp/include \
../../CStrUtil/include \
../../CUtil/include \
../../CRGBName/include \
../../COS/include \

unix:LIBS += \
-L$$LIB_DIR \
-L../../CImageLib/lib \
-L../../CFont/lib \
-L../../CConfig/lib \
-L../../CFile/lib \
-L../../CRegExp/lib \
-L../../CStrUtil/lib \
-L../../CRGBName/lib \
-L../../COS/lib \
-lCImageLib \
-lCFont \
-lCConfig \
-lCFile \
-lCRGBName \
-lCRegExp \
-lCStrUtil \
-lCOS \
-ljpeg -lpng \
-ltre -lcurses
