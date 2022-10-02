#ifndef CTEXT_TABS_H
#define CTEXT_TABS_H

#include <set>

class CTextTabs {
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

  //---

  bool nextPos(int pos, int *pos1) {
    // move to next tab pos greater than supplied pos
    for (const auto &tab : tabs_) {
      if (int(tab) > pos) {
        *pos1 = int(tab);
        return true;
      }
    }

    // fail if at right edge
    if (pos >= int(length_ - 1))
      return false;

    // move to next default tab stop (every 8 characters)
    int d = 8 - (pos % 8);

    *pos1 = pos + d;

    if (*pos1 >= int(length_))
      *pos1 = int(length_ - 1);

    return true;
  }

  //---

  bool prevPos(int pos, int *pos1) {
    // move to prev tab pos less than supplied pos
    TabList::const_reverse_iterator p1, p2;

    for (p1 = tabs_.rbegin(), p2 = tabs_.rend(); p1 != p2; ++p1) {
      if (int(*p1) < pos) {
        *pos1 = int(*p1);
        return true;
      }
    }

    // fail if at left edge
    if (pos <= 0)
      return false;

    // move to prev default tab stop (every 8 characters)
    int d = pos % 8;

    if (d == 0)
      d = 8;

    *pos1 = pos - d;

    if (*pos1 < 0)
      *pos1 = 0;

    return true;
  }

  //---

 private:
  typedef std::set<uint> TabList;

  TabList tabs_;
  uint    length_ { 80 };
};

#endif
