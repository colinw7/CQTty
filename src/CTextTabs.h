#ifndef CTEXT_TABS_H
#define CTEXT_TABS_H

#include <set>

class CTextTabs {
 private:
  typedef std::set<int> TabList;

  TabList tabs_;
  uint    length_;

 public:
  CTextTabs() :
   length_(80) {
  }

  uint getLength() const { return length_; }

  void setLength(uint length) { length_ = length; }

  void init() {
    clearAll();
  }

  void set(uint x) {
    tabs_.insert(x);
  }

  void clear(uint x) {
    tabs_.erase(x);
  }

  void clearAll() {
    tabs_.clear();
  }

  bool nextPos(int pos, int *pos1) {
    TabList::const_iterator p1, p2;

    for (p1 = tabs_.begin(), p2 = tabs_.end(); p1 != p2; ++p1) {
      if (*p1 > pos) {
        *pos1 = *p1;
        return true;
      }
    }

    if (pos >= (int) length_ - 1)
      return false;

    int d = 8 - (pos % 8);

    *pos1 = pos + d;

    if (*pos1 >= (int) length_)
      *pos1 = length_ - 1;

    return true;
  }

  bool prevPos(int pos, int *pos1) {
    TabList::const_reverse_iterator p1, p2;

    for (p1 = tabs_.rbegin(), p2 = tabs_.rend(); p1 != p2; ++p1) {
      if (*p1 < pos) {
        *pos1 = *p1;
        return true;
      }
    }

    if (pos <= 0)
      return false;

    int d = pos % 8;

    if (d == 0)
      d = 8;

    *pos1 = pos - d;

    if (*pos1 < 0)
      *pos1 = 0;

    return true;
  }
};

#endif
