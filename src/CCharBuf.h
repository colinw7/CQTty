#ifndef CCHAR_BUF_H
#define CCHAR_BUF_H

#include <sys/types.h>
#include <cstring>
#include <cassert>

class CCharBuf {
 private:
  char *chars_;
  uint  chars_len_;
  uint  chars_max_;

 public:
  CCharBuf() :
   chars_(0), chars_len_(0), chars_max_(0) {
    grow(32);

    chars_[chars_len_] = '\0';
  }

  virtual ~CCharBuf() {
    delete [] chars_;
  }

  const char *getChars() const { return chars_; }

  uint getLen() const { return chars_len_; }

  void addChar(char c) {
    addChars(&c, 1);
  }

  void addChars(const char *chars, uint len) {
    if (len + chars_len_ > chars_max_)
      grow(len);

    memcpy(&chars_[chars_len_], chars, len);

    chars_len_ += len;

    chars_[chars_len_] = '\0';
  }

  void clear() {
    chars_len_ = 0;

    chars_[chars_len_] = '\0';
  }

  void shiftTo(uint pos) {
    assert(pos <= chars_len_);

    if (pos == 0) return;

    for (uint i = pos, j = 0; i < chars_len_; ++i, ++j)
      chars_[j] = chars_[i];

    chars_len_ -= pos;

    chars_[chars_len_] = '\0';
  }

  void delEndChar() {
    if (chars_len_ > 0) {
      --chars_len_;

      chars_[chars_len_] = '\0';
    }
  }

  virtual void grow(uint size) {
    uint chars_max = 2*chars_max_ + size + 1;

    char *chars = new char [chars_max + 1];

    if (chars_ != 0) {
      memcpy(chars, chars_, (chars_max_ + 1)*sizeof(char));

      delete [] chars_;
    }

    chars_     = chars;
    chars_max_ = chars_max;
  }
};

#endif
