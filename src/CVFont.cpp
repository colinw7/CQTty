#include <CVFont.h>
#include <CVFontDef.h>

CVFont::
CVFont()
{
}

const CVFontDef &
CVFont::
getFontDef(char c)
{
  if (c >= ' ' && c <= '~')
    return vfont_defs[c - ' '];
  else
    return vfont_defs[0];
}
