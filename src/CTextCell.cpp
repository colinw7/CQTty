#include <CTextCell.h>
#include <CPageText.h>
#include <CPageTextLine.h>

CTextCell::
CTextCell(CPageTextLine *line, Type type) :
 line_(line), type_(type)
{
}

const CCellStyle &
CTextCell::
getStyle() const
{
  return line_->getStyle();
}

CTextCell *
CTextCell::
convertTo(Type type)
{
  if (type_ == type)
    return this;

  CTextCell *cell1 = nullptr;

  switch (type) {
    case Type::CHAR:
      cell1 = new CTextCharCell(line_);
      break;
    default:
      break;
  }

  return cell1;
}

CIBBox2D
CTextCell::
getBBox(int x, int y, int cw, int ch, bool doubleHeight) const
{
  CIBBox2D bbox(CIPoint2D(x, y), CISize2D(cw - 1, ch - 1));

  if (getLine()->widthStyle() == CCellLineWidthStyle::DOUBLE)
    bbox.setWidth(bbox.getWidth()*2);

  if (doubleHeight) {
    if (getLine()->heightStyle() == CCellLineHeightStyle::DOUBLE_BOTTOM ||
        getLine()->heightStyle() == CCellLineHeightStyle::DOUBLE_TOP) {
      int h = bbox.getHeight();

      if (getLine()->heightStyle() == CCellLineHeightStyle::DOUBLE_BOTTOM) {
        bbox.setYMin(bbox.getYMin() - h);
        bbox.setYMax(bbox.getYMax() - h);
      }

      bbox.setHeight(h*2);
    }
  }

  return bbox;
}

//---------

char
CTextCharCell::
getChar() const
{
  return c_;
}

void
CTextCharCell::
setChar(char c)
{
  c_ = c;
}

void
CTextCharCell::
setStyle(const CCellStyle &style)
{
  style_ = style;
}

void
CTextCharCell::
resetStyle()
{
  style_.reset();
}

//---------

void
CTextImageCell::
setFileName(const std::string &fileName)
{
  fileName_ = fileName;
}

void
CTextImageCell::
setImage(CImagePtr image)
{
  image_ = image;
}

//---------

void
CTextLinkCell::
setLinkDest(const std::string &linkDest)
{
  linkDest_ = linkDest;
}

void
CTextLinkCell::
setLinkName(const std::string &linkName)
{
  linkName_ = linkName;
}
