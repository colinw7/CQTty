#ifndef CQTty_H
#define CQTty_H

#include <QWidget>
#include <CConfig.h>

class QToolBar;
class QStatusBar;
class QStackedWidget;
class CQPageTextWidget;
class CQImagePreview;
class CQDirList;
class CQEdit;
class CTty;

class CQTty : public QWidget {
  Q_OBJECT

 public:
  CQTty(QWidget *parent=nullptr);

  CQPageTextWidget *getText() const;

  CTty *getTty() const { return tty_; }

  void notifyTty();

  void showToolBar  (bool show);
  void showStatusBar(bool show);

  void showFileBrowser();

  void resizeEvent(QResizeEvent *);

  QSize sizeHint() const;

 private slots:
  void displayImage(const QString &);
  void displayFile (const QString &);

  void setTtyPageSize(int rows, int cols);
  void updateCurrentDir(const QString &dirName);
  void changeCurrentDir(const QString &dirName);

  void flush();

  void fitAreaSlot();

 public slots:
  void showText ();
  void showImage();
  void showEdit();
  void showDefault();

 private:
  CConfig           config_;
  QToolBar*         toolbar_      { nullptr };
  QStackedWidget*   stack_        { nullptr };
  CQPageTextWidget* text_         { nullptr };
  CQDirList*        dirList_      { nullptr };
  CQImagePreview*   imagePreview_ { nullptr };
  CQEdit*           edit_         { nullptr };
  QStatusBar*       status_       { nullptr };
  CTty*             tty_          { nullptr };
  QTimer*           timer_        { nullptr };
  int               ind_          { 0 };
};

#endif
