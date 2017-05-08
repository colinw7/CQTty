#include <CQPageTextError.h>
#include <CQPageText.h>
#include <CQPageTextWidget.h>

#include <QTextEdit>
#include <QVBoxLayout>

CQPageTextError::
CQPageTextError(CQPageText *text) :
 QDialog(text->widget()), text_(text)
{
  setObjectName("error");

  QVBoxLayout *layout = new QVBoxLayout(this);

  edit_ = new QTextEdit;

  layout->addWidget(edit_);
}

void
CQPageTextError::
addError(const std::string &str)
{
  QTextCursor cursor = edit_->textCursor();

  cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

  cursor.insertText(str.c_str());
}
