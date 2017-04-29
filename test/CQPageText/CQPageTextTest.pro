APPNAME = CQPageTextTest

QT += x11extras

include($$(MAKE_DIR)/qt_app.mk)

SOURCES += \
CQPageTextTest.cpp

HEADERS += \
CQPageTextTest.h

CONFIG += debug

INCLUDEPATH += \
$(INC_DIR)/CQPageText \
$(INC_DIR)/CPageText \
$(INC_DIR)/CQImagePreview \
$(INC_DIR)/CQDirView \
$(INC_DIR)/CDirView \
$(INC_DIR)/CQDirList \
$(INC_DIR)/CQWinWidget \
$(INC_DIR)/CQWindow \
$(INC_DIR)/CQApp \
$(INC_DIR)/CQEdit \
$(INC_DIR)/CVEdit \
$(INC_DIR)/CEdit \
$(INC_DIR)/CTextFile \
$(INC_DIR)/CTty \
$(INC_DIR)/CEscape \
$(INC_DIR)/CUndo \
$(INC_DIR)/CEvent \
$(INC_DIR)/CWindow \
$(INC_DIR)/CCellStyle \
$(INC_DIR)/CTextPos \
$(INC_DIR)/CLogFile \
$(INC_DIR)/CImageLib \
$(INC_DIR)/CFont \
$(INC_DIR)/CConfig \
$(INC_DIR)/CFile \
$(INC_DIR)/CFileType \
$(INC_DIR)/CStrUtil \
$(INC_DIR)/CAlignType \
$(INC_DIR)/CMath \
$(INC_DIR)/COptVal \
$(INC_DIR)/CThrow \
$(INC_DIR)/CRegExp \
$(INC_DIR)/CRGBA \
$(INC_DIR)/CRefPtr \
$(INC_DIR)/CFlags \
$(INC_DIR)/CKeyType \
$(INC_DIR)/CAutoPtr \
$(INC_DIR)/CUtil \

PRE_TARGETDEPS = \
$(LIB_DIR)/libCQPageText.a \
$(LIB_DIR)/libCPageText.a \
$(LIB_DIR)/libCEscape.a \
$(LIB_DIR)/libCQWindow.a

unix:LIBS += \
-lCQPageText -lCPageText -lCQWindow -lCQColorChooser -lCQIntegerEdit \
$$QT_APP_LIBS \
-lCTty -lCLogFile -lCEscape -lCWindow -lCFreeType -lCTimer -lCArgs -lCVFont \
-lCEvent -lCRGBUtil -lCFont -lCConfig -lCImageLib -lCFileParse -lCReadLine \
-lCStrParse -lCStrUtil -lCUtil -lCFileUtil -lCFile -lCMath -lCHistory \
-lCOSFile -lCOSUser -lCOSRead -lCTempFile \
-lCAssert -lCGlob -lCRegExp -lCEnv -lCPrintF -lCStrNCase \
$$IMAGE_LIBS -ltre -lfreetype -lreadline -lcurses
