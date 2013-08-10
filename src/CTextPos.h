#ifndef CTEXT_POS_H
#define CTEXT_POS_H

class CTextPos {
 private:
  int row_, col_;

 public:
  CTextPos(int row=0, int col=0) :
   row_(row), col_(col) {
  }

  int getRow() const { return row_; }
  int getCol() const { return col_; }

  int getX() const { return getCol(); }
  int getY() const { return getRow(); }

  void setRowCol(int row, int col) { row_ = row; col_ = col; }

  void setRow(int row) { row_ = row; }
  void setCol(int col) { col_ = col; }

  void setX(int x) { setCol(x); }
  void setY(int y) { setRow(y); }
};

#endif
