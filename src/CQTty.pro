TEMPLATE = app

TARGET = CQTty

DEPENDPATH += .

INCLUDEPATH += . ../include

QMAKE_CXXFLAGS += -std=c++17 -DCQUTIL_IMAGE -DCQUTIL_FONT

CONFIG += debug

MOC_DIR = .moc

QT += widgets

# Input
SOURCES += \
CEscapeColors.cpp \
CEscape.cpp \
CEscapeHandler.cpp \
CEscapeParse.cpp \
CLogFile.cpp \
CPageText.cpp \
CPageTextEscapeNotifier.cpp \
CPageTextLine.cpp \
CQPageText4014.cpp \
CQPageTextButton.cpp \
CQPageTextCanvas.cpp \
CQPageText.cpp \
CQPageTextState.cpp \
CQPageTextStatus.cpp \
CQPageTextToolBar.cpp \
CQPageTextWidget.cpp \
CQPageTextError.cpp \
CQTty.cpp \
CTextCell.cpp \
CTty.cpp \
main.cpp \

HEADERS += \
CCellStyle.h \
CCharBuf.h \
CEscapeColors.h \
CEscape.h \
CEscapeHandler.h \
CEscapeParse.h \
CEscapeState.h \
CEscapeType.h \
CLogFile.h \
CPageTextEscapeNotifier.h \
CPageText.h \
CPageTextLine.h \
CQPageText4014.h \
CQPageTextButton.h \
CQPageTextCanvas.h \
CQPageText.h \
CQPageTextState.h \
CQPageTextStatus.h \
CQPageTextToolBar.h \
CQPageTextWidget.h \
CQPageTextError.h \
CQTty.h \
CQTtyPageTextWidget.h \
CTextCell.h \
CTextPos.h \
CTextTabs.h \
CTty.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
../include \
../../CQUtil/include \
../../CImageLib/include \
../../CFont/include \
../../CVFont/include \
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
-L../../CVFont/lib \
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
-lCVFont \
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
