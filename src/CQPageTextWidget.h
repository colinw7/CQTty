#ifndef CQPageTextWidget_H
#define CQPageTextWidget_H

#include <CTextPos.h>
#include <QWidget>

class CQPageTextToolBar;
class CQPageTextStatus;
class CQPageText;
class CQPageTextCanvas;
class CTty;

class QScrollBar;

class CQPageTextWidget : public QWidget {
  Q_OBJECT

  Q_PROPERTY(uint uiWidth  READ uiWidth)
  Q_PROPERTY(uint uiHeight READ uiHeight)

 public:
  CQPageTextWidget(QWidget *parent=nullptr);

  virtual ~CQPageTextWidget() { }

  void setDebug(bool debug);
  void setTrace(bool trace);

  virtual CQPageTextToolBar *createToolBar();
  virtual CQPageTextStatus  *createStatusBar();

  CQPageTextToolBar* getToolBar  () const { return toolBar_  ; }
  CQPageTextStatus*  getStatusBar() const { return statusBar_; }
  CQPageText*        getArea     () const { return area_     ; }
  QScrollBar*        getHScroll  () const { return hscroll_  ; }
  QScrollBar*        getVScroll  () const { return vscroll_  ; }

  int uiWidth () const { return ui_width_ ; }
  int uiHeight() const { return ui_height_; }

  void loadCmd(const std::string &fileName);
  void loadEsc(const std::string &fileName);

  void addEscapeChars(const char *str, uint len);

  void emitDisplayImage(const std::string &path);
  void emitDisplayFile (const std::string &path);

  void scrollBottom();

  virtual void showToolBar  (bool show);
  virtual void showScrollBar(bool show);
  virtual void showStatusBar(bool show);

  virtual void showFileBrowser() { }
  virtual void showImage() { }

  virtual CTty *getTty() const { return nullptr; }

  void processString(const char *str);

  void pasteString(const char *str);

  void flush();

  void notifyPageSize(int rows, int cols);
  void notifyPosition(const CTextPos &pos);
  void notifyStyle(const CTextPos &pos);
  void notifyCharSize(int width, int height);

  void notifyStateChange();

  void notifyDirChanged(const std::string &dirName);

  void updatePlacement();

  void updateHints();

  void sizeToRowCols(int w, int h, int *rows, int *cols) const;
  void rowColsToSize(int rows, int cols, int *w, int *h) const;

  void repaint();

 private:
  void resizeEvent(QResizeEvent *);
  void showEvent(QShowEvent *);

 private slots:
  void hscrollSlot();
  void vscrollSlot();

  void blinkTimeout();

 signals:
  void pageSizeSignal(int rows, int cols);
  void dirChangeSignal(const QString &path);

  void displayImage(const QString &path);
  void displayFile (const QString &path);

 private:
  CQPageTextToolBar* toolBar_    { nullptr };
  CQPageTextStatus*  statusBar_  { nullptr };
  CQPageText*        area_       { nullptr };
  CQPageTextCanvas*  text_       { nullptr };
  QScrollBar*        hscroll_    { nullptr };
  QScrollBar*        vscroll_    { nullptr };
  QTimer*            blinkTimer_ { nullptr };
  int                ui_width_   { 0 };
  int                ui_height_  { 0 };
};

#endif
