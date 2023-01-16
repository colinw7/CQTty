#ifndef CQTtyPageTextWidget_H
#define CQTtyPageTextWidget_H

#include <CQPageTextWidget.h>

class CQTtyPageTextWidget : public CQPageTextWidget {
 public:
  CQTtyPageTextWidget(CQTty *qtty) :
   CQPageTextWidget(nullptr), tty_(qtty) {
  }

  void showFileBrowser() override {
    tty_->showFileBrowser();
  }

  void showImage() override {
    tty_->showImage();
  }

  CTty *getTty() const override { return tty_->getTty(); }

 private:
  CQTty *tty_ { nullptr };
};

#endif
