#ifndef CQPageTextTest_H
#define CQPageTextTest_H

#include <QWidget>
#include <CConfig.h>

class CQPageTextWidget;

class CQPageTextTest : public QWidget {
  Q_OBJECT

 public:
  CQPageTextTest();

  CQPageTextWidget *getPageText() const { return area_; }

  void resizeEvent(QResizeEvent *);

  QSize sizeHint() const;

 private:
  void loadConfig();

 private slots:
  void fitAreaSlot();

 private:
  CQPageTextWidget *area_ { nullptr };
  CConfig           config_;
};

#endif
