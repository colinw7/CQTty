#ifndef CQPAGE_DATA_H
#define CQPAGE_DATA_H

#include <CPageText.h>
#include <CTextPos.h>
#include <QObject>

class CQPageTextWidget;
class CQPageText4014;
class CQPageTextError;
class CTty;
class CConfig;

class CQPageText : public QObject, public CPageText {
  Q_OBJECT

  Q_PROPERTY(bool debug         READ getDebug         WRITE setDebug        )
  Q_PROPERTY(bool trace         READ getTrace         WRITE setTrace        )
  Q_PROPERTY(bool isAlt         READ isAlt            WRITE setIsAlt        )
  Q_PROPERTY(uint row           READ getRow           WRITE setRow          )
  Q_PROPERTY(uint col           READ getCol           WRITE setCol          )
  Q_PROPERTY(uint pageRows      READ getPageRows      WRITE setPageRows     )
  Q_PROPERTY(uint pageCols      READ getPageCols      WRITE setPageCols     )
  Q_PROPERTY(uint charWidth     READ getCharWidth)
  Q_PROPERTY(uint charHeight    READ getCharHeight)
  Q_PROPERTY(uint charAscent    READ getCharAscent)
  Q_PROPERTY(uint pageWidth     READ getPageWidth)
  Q_PROPERTY(uint pageHeight    READ getPageHeight)
  Q_PROPERTY(bool showToolbar   READ getShowToolBar   WRITE setShowToolBar  )
  Q_PROPERTY(bool showScrollBar READ getShowScrollBar WRITE setShowScrollBar)
  Q_PROPERTY(bool showStatusBar READ getShowStatusBar WRITE setShowStatusBar)
  Q_PROPERTY(bool showLinks     READ getShowLinks     WRITE setShowLinks    )
  Q_PROPERTY(bool moveOnClick   READ isMoveOnClick    WRITE setMoveOnClick  )

 public:
  CQPageText(CQPageTextWidget *widget);

  CQPageTextWidget *widget() const { return widget_; }

  bool getShowLinks() const { return showLinks_; }
  void setShowLinks(bool b) { showLinks_ = b; }

  bool isMoveOnClick() const { return moveOnClick_; }
  void setMoveOnClick(bool b) { moveOnClick_ = b; }

  //---

  void logError(const std::string &str) override;

  //---

  void set4014(bool b) override;

  //---

  void setIsAlt(bool alt) override;

  //---

  void clear() override;
  void clearAbove() override;
  void clearBelow() override;
  void clearSaved() override;

  //---

  void notifyPageSize(int rows, int cols) override;
  void notifyPosition(const CTextPos &pos) override;
  void notifyCharSize(int width, int height) override;

  void notifyStyle(const CTextPos &pos) override;

  void notifyStateChange() override;

  CTty *getTty() const override;

  //---

  // 4014
  void exec4014BS() override;
  void exec4014TAB() override;
  void exec4014LF() override;
  void exec4014FF() override;
  void exec4014CR() override;
  void exec4014CUF() override;

  void set4014GIN(bool b) override;

  void set4014CharSet(int charSet) override;

  void clear4014() override;

  void draw4014Line(int x1, int y1, int x2, int y2, const CEscapeColor &color,
                    const CEscapeLineStyle &style) override;

  void draw4014Char(char c) override;

  CIPoint2D get4014DataPos() const override;

  int get4014NumDataRows() const override;
  int get4014NumDataCols() const override;

  //---

  void scrollBottom() override;

  void notifyToolBar  () override;
  void notifyScrollBar() override;
  void notifyStatusBar() override;

  void notifyDirChanged(const std::string &dirName) override;

  void update() override;

  //---

  void setConfig(CConfig *config) { config_ = config; }
  CConfig *getConfig() const { return config_; }

  bool loadConfig();
  bool saveConfig();

 private:
  bool loadConfigColor(const std::string &name, CEscapeColor color);
  bool saveConfigColor(const std::string &name, CEscapeColor color);

 private:
  CQPageTextWidget *widget_      { nullptr };
  CQPageText4014   *text4014_    { nullptr };
  CQPageTextError  *errorDlg_    { nullptr };
  bool              showLinks_   { false };
  bool              moveOnClick_ { false };
  CConfig          *config_      { nullptr };
};

#endif
