#ifndef CQPAGE_TEXT_ERROR_H
#define CQPAGE_TEXT_ERROR_H

#include <QDialog>

class CQPageText;
class QTextEdit;

class CQPageTextError : public QDialog {
  Q_OBJECT

 public:
  CQPageTextError(CQPageText *text);

  void addError(const std::string &str);

 private:
  CQPageText *text_ { nullptr };
  QTextEdit  *edit_ { nullptr };
};

#endif
