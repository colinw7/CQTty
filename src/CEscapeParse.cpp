#include <CEscapeParse.h>
#include <CEscapeColors.h>
#include <CStrUtil.h>
#include <CStrParse.h>
#include <CImageLib.h>
#include <CRGBName.h>
#include <sstream>

#define UNHANDLED(m) { \
  std::ostringstream ostr; \
  ostr << "Unhandled Esc: " << m << "\n"; \
  logError(ostr.str()); \
}

#define UNHANDLED1(m,a) { \
  std::ostringstream ostr; \
  ostr << "Unhandled Esc: " << m << " " << a << "\n"; \
  logError(ostr.str()); \
}

#define UNHANDLED2(m,a,b) { \
  std::ostringstream ostr; \
  ostr << "Unhandled Esc: " << m << " " << a << " " << b << "\n"; \
  logError(ostr.str()); \
}

#define UNHANDLED3(m,a,b,c) { \
  std::ostringstream ostr; \
  ostr << "Unhandled Esc: " << m << " " << a << " " << b << "" << c << "\n"; \
  logError(ostr.str()); \
}

//------

namespace {

bool roundBracketString(const std::string &str, uint &pos, std::string &str1) {
  if (str[pos] != '(')
    return false;

  ++pos;

  str1 = "";

  while (str[pos] != ')')
    str1 += str[pos++];

  if (str[pos] == ')')
    ++pos;

  return true;
}

bool squareBracketString(const std::string &str, uint &pos, std::string &str1) {
  if (str[pos] != '[')
    return false;

  ++pos;

  str1 = "";

  while (str[pos] != ']')
    str1 += str[pos++];

  if (str[pos] == ']')
    ++pos;

  return true;
}

struct RegisPoint {
  int  x     { 0 };
  int  y     { 0 };
  bool x_rel { false };
  bool y_rel { false };

  RegisPoint() { }
};

struct RegisData {
  RegisPoint  point;
  std::string parameter;

  RegisData(const std::string &str) :
   parameter(str) {
  }

  RegisData(const RegisPoint &p) :
   point(p) {
  }
};

bool regisValueToPoint(const std::string &str, RegisPoint &point) {
  int pos = 0;
  int len = str.size();
  int num = 0;

  std::string str1;

  while (pos < len) {
    if (str[pos] == ',') {
      ++num;

      point.x = std::stoi(str1);

      ++pos;

      str1 = "";
    }
    else {
      if (str1 == "" && (str[pos] == '+' || str[pos] == '-')) {
        if (num == 0)
          point.x_rel = true;
        else
          point.y_rel = true;
      }

      str1 += str[pos++];
    }
  }

  point.y = std::stoi(str1);

  return true;
}

bool getRegisData(const std::string &str, uint &pos, std::vector<RegisData> &values)
{
  uint len = str.size();

  while (pos < len) {
    if      (str[pos] == '(') {
      std::string str1;

      if (! roundBracketString(str, pos, str1))
        return false;

      values.push_back(RegisData(str1));
    }
    else if (str[pos] == '[') {
      std::string str1;

      if (! squareBracketString(str, pos, str1))
        return false;

      RegisPoint point;

      if (! regisValueToPoint(str1, point))
        return false;

      values.push_back(RegisData(point));
    }
    else if (str[pos] == ',') {
      ++pos;
    }
    else if (str[pos] == ' ') {
      while (str[pos] == ' ')
        ++pos;
    }
    else {
      return false;
    }
  }

  return true;
}

}

//------

CEscapeParse::
CEscapeParse() :
 NUL_escape_            (CEscapeType::NUL),
 SOH_escape_            (CEscapeType::SOH),
 STX_escape_            (CEscapeType::STX),
 ETX_escape_            (CEscapeType::ETX),
 EOT_escape_            (CEscapeType::EOT),
 ENQ_escape_            (CEscapeType::ENQ),
 ACK_escape_            (CEscapeType::ACK),
 BEL_escape_            (CEscapeType::BEL),
 BS_escape_             (CEscapeType::BS),
 HT_escape_             (CEscapeType::HT),
 LF_escape_             (CEscapeType::LF),
 VT_escape_             (CEscapeType::VT),
 FF_escape_             (CEscapeType::FF),
 CR_escape_             (CEscapeType::CR),
 SO_escape_             (CEscapeType::SO),
 SI_escape_             (CEscapeType::SI),
 DLE_escape_            (CEscapeType::DLE),
 DC1_escape_            (CEscapeType::DC1),
 DC2_escape_            (CEscapeType::DC2),
 DC3_escape_            (CEscapeType::DC3),
 DC4_escape_            (CEscapeType::DC4),
 NAK_escape_            (CEscapeType::NAK),
 SYN_escape_            (CEscapeType::SYN),
 ETB_escape_            (CEscapeType::ETB),
 CAN_escape_            (CEscapeType::CAN),
 EM_escape_             (CEscapeType::EM),
 SUB_escape_            (CEscapeType::SUB),
 FS_escape_             (CEscapeType::FS),
 GS_escape_             (CEscapeType::GS),
 RS_escape_             (CEscapeType::RS),
 US_escape_             (CEscapeType::US),
 DEL_escape_            (CEscapeType::DEL),
 DECPAM_escape_         (CEscapeType::DECPAM),
 DECPNM_escape_         (CEscapeType::DECPNM),
 LS3R_escape_           (CEscapeType::LS3R),
 LS2R_escape_           (CEscapeType::LS2R),
 LS1R_escape_           (CEscapeType::LS1R),
 S7C1T_escape_          (CEscapeType::S7C1T),
 S8C1T_escape_          (CEscapeType::S8C1T),
 ANSI1_escape_          (CEscapeType::ANSI1),
 ANSI2_escape_          (CEscapeType::ANSI2),
 ANSI3_escape_          (CEscapeType::ANSI3),
 DECDHL_escape_         (CEscapeType::DECDHL),
 DECSWL_escape_         (CEscapeType::DECSWL),
 DECDWL_escape_         (CEscapeType::DECDWL),
 DECALN_escape_         (CEscapeType::DECALN),
 ISO8859_1_escape_      (CEscapeType::ISO8859_1),
 UTF_8_escape_          (CEscapeType::UTF_8),
 G0_escape_             (CEscapeType::G0),
 G1_escape_             (CEscapeType::G1),
 G2_escape_             (CEscapeType::G2),
 G3_escape_             (CEscapeType::G3),
 IND_escape_            (CEscapeType::IND),
 NEL_escape_            (CEscapeType::NEL),
 SSA_escape_            (CEscapeType::SSA),
 ESA_escape_            (CEscapeType::ESA),
 HTS_escape_            (CEscapeType::HTS),
 RI_escape_             (CEscapeType::RI),
 SS2_escape_            (CEscapeType::SS2),
 SS3_escape_            (CEscapeType::SS3),
 CCH_escape_            (CEscapeType::CCH),
 SPA_escape_            (CEscapeType::SPA),
 EPA_escape_            (CEscapeType::EPA),
 SOS_escape_            (CEscapeType::SOS),
 DECID_escape_          (CEscapeType::DECID),
 RIS_escape_            (CEscapeType::RIS),
 MemoryLock_escape_     (CEscapeType::MemoryLock),
 MemoryUnlock_escape_   (CEscapeType::MemoryUnlock),
 LS2_escape_            (CEscapeType::LS2),
 LS3_escape_            (CEscapeType::LS3),
 DECSC_escape_          (CEscapeType::DECSC),
 DECRC_escape_          (CEscapeType::DECRC),
 DECSED_escape_         (CEscapeType::DECSED),
 DECSEL_escape_         (CEscapeType::DECSEL),
 DA2_escape_            (CEscapeType::DA2),
 DECSET_escape_         (CEscapeType::DECSET),
 DECMC_escape_          (CEscapeType::DECMC),
 DECRST_escape_         (CEscapeType::DECRST),
 DECDSR_escape_         (CEscapeType::DECDSR),
 DECSTR_escape_         (CEscapeType::DECSTR),
 DECRestorePriv_escape_ (CEscapeType::DECRestorePriv),
 DECSavePriv_escape_    (CEscapeType::DECSavePriv),
 HPA_escape_            (CEscapeType::HPA),
 DECSCL_escape_         (CEscapeType::DECSCL),
 DECSCA_escape_         (CEscapeType::DECSCA),
 DECCARA_escape_        (CEscapeType::DECCARA),
 DECRARA_escape_        (CEscapeType::DECRARA),
 DECCRA_escape_         (CEscapeType::DECCRA),
 DECEFR_escape_         (CEscapeType::DECEFR),
 DECLRP_escape_         (CEscapeType::DECLRP),
 DECFRA_escape_         (CEscapeType::DECFRA),
 DECSACE_escape_        (CEscapeType::DECSACE),
 DECELR_escape_         (CEscapeType::DECELR),
 DECERA_escape_         (CEscapeType::DECERA),
 DECSLE_escape_         (CEscapeType::DECSLE),
 DECSERA_escape_        (CEscapeType::DECSERA),
 DECRQLP_escape_        (CEscapeType::DECRQLP),
 ICH_escape_            (CEscapeType::ICH),
 CUU_escape_            (CEscapeType::CUU),
 CUD_escape_            (CEscapeType::CUD),
 CUF_escape_            (CEscapeType::CUF),
 CUB_escape_            (CEscapeType::CUB),
 CNL_escape_            (CEscapeType::CNL),
 CPL_escape_            (CEscapeType::CPL),
 CHA_escape_            (CEscapeType::CHA),
 CUP_escape_            (CEscapeType::CUP),
 CHT_escape_            (CEscapeType::CHT),
 ED_escape_             (CEscapeType::ED),
 EL_escape_             (CEscapeType::EL),
 IL_escape_             (CEscapeType::IL),
 DL_escape_             (CEscapeType::DL),
 DCH_escape_            (CEscapeType::DCH),
 SU_escape_             (CEscapeType::SU),
 SD_escape_             (CEscapeType::SD),
 StartMouseTrack_escape_(CEscapeType::StartMouseTrack),
 ECH_escape_            (CEscapeType::ECH),
 CBT_escape_            (CEscapeType::CBT),
 REP_escape_            (CEscapeType::REP),
 DA1_escape_            (CEscapeType::DA1),
 VPA_escape_            (CEscapeType::VPA),
 HVP_escape_            (CEscapeType::HVP),
 TBC_escape_            (CEscapeType::TBC),
 SM_escape_             (CEscapeType::SM),
 MC_escape_             (CEscapeType::MC),
 RM_escape_             (CEscapeType::RM),
 SGR_escape_            (CEscapeType::SGR),
 DSR_escape_            (CEscapeType::DSR),
 DECLL_escape_          (CEscapeType::DECLL),
 DECSTBM_escape_        (CEscapeType::DECSTBM),
 SC_escape_             (CEscapeType::SC),
 WindowManip_escape_    (CEscapeType::WindowManip),
 DECREQTPARM_escape_    (CEscapeType::DECREQTPARM),
 DECTST_escape_         (CEscapeType::DECTST),
 set_dir_escape_        (CEscapeType::SET_DIR),
 sized_image_escape_    (CEscapeType::SIZED_IMAGE),
 file_image_escape_     (CEscapeType::FILE_IMAGE),
 image_escape_          (CEscapeType::IMAGE),
 pixel_escape_          (CEscapeType::PIXEL),
 line_escape_           (CEscapeType::LINE),
 preview_files_escape_  (CEscapeType::PREVIEW_FILES),
 paste_escape_          (CEscapeType::PASTE),
 trace_escape_          (CEscapeType::TRACE),
 debug_escape_          (CEscapeType::DEBUG),
 DCS_escape_            (CEscapeType::DCS)
{
}

CEscapeParse::
~CEscapeParse()
{
}

void
CEscapeParse::
setInEscape(bool in_escape)
{
  if (in_escape_ != in_escape) {
    in_escape_ = in_escape;

    //log(std::string("InEscape=") + (in_escape_ ? "1" : "0"));
  }
}

void
CEscapeParse::
reset()
{
  setInEscape(false);

  escape_buf_.clear();
}

void
CEscapeParse::
addOutputChar(char c)
{
  static char str[2];

  str[0] = c;
  str[1] = '\0';

  addOutputChars(str, 1);
}

void
CEscapeParse::
addOutputChars(const std::string &str)
{
  addOutputChars(str.c_str(), str.size());
}

void
CEscapeParse::
addOutputChars(const char *str, uint len)
{
  static bool        locked;
  static std::string preBuffer;
  static std::string postBuffer;

  if (locked) {
    postBuffer += std::string(str, len);
    return;
  }

  //------

  if (preBuffer.size()) {
    std::string buffer1 = preBuffer + std::string(str, len);

    preBuffer = "";

    addOutputChars(buffer1.c_str(), buffer1.size());

    return;
  }

  //------

  locked = true;

  //------

  uint i = 0;

  while (i < len) {
    if (! getInEscape()) {
      if      (str[i] < 32) {
        if (! addControlChar(str, &i, len)) {
          // too many characters (4014)
          preBuffer = std::string(str, len).substr(i);
          break;
        }
      }
      else if (str[i] < 127) {
        addNormalChar(str[i]);

        ++i;
      }
      else {
        addGraphicChar(str[i]);

        ++i;
      }
    }
    else {
      escape_buf_.addChars(&str[i], len - i);

      uint pos = 0;

      if (isEscape(escape_buf_.getChars(), &pos)) {
        int len1 = escape_buf_.getLen() - pos;

        assert(len1 >= 0 && len1 <= int(len));

        i = len - len1;

        escape_buf_.shiftTo(pos);

        pos = 0;

        while (i < len && getInEscape() && isEscape(escape_buf_.getChars(), &pos)) {
          int len2 = escape_buf_.getLen() - pos;

          assert(len2 >= 0 && len2 <= int(len));

          i = len - len2;

          escape_buf_.shiftTo(pos);

          pos = 0;
        }
      }
      else
        break;
    }
  }

  //------

  locked = false;

  if (! postBuffer.empty()) {
    std::string buffer1 = postBuffer;

    postBuffer.clear();

    addOutputChars(buffer1.c_str(), buffer1.size());
  }
}

void
CEscapeParse::
addNormalChar(char c)
{
  handleChar(c);
}

bool
CEscapeParse::
addControlChar(const char *str, uint *pos, uint len)
{
  char c = str[(*pos)++];

  switch (c) {
    case 0x00: { // NUL,   ,   , Nul
      handleEscape(&NUL_escape_);
      break;
    }
    case 0x01: { // SOH,   , ^A
      handleEscape(&SOH_escape_);
      break;
    }
    case 0x02: { // STX,   , ^B
      handleEscape(&STX_escape_);
      break;
    }
    case 0x03: { // ETX,   , ^C
      handleEscape(&ETX_escape_);
      break;
    }
    case 0x04: { // EOT,   , ^D
      handleEscape(&EOT_escape_);
      break;
    }
    case 0x05: { // ENQ,   , ^E, Terminal Status
      // terminal status (default is empty)
      handleEscape(&ENQ_escape_);
      break;
    }
    case 0x06: { // ACK,   , ^F
      handleEscape(&ACK_escape_);
      break;
    }
    case 0x07: { // BEL, \a, ^G, Bell
      handleEscape(&BEL_escape_);
      break;
    }
    case 0x08: { // BS , \b, ^H, Backspace
      handleEscape(&BS_escape_);
      break;
    }
    case 0x09: { // HT , \t, ^I, Horizontal Tab
      handleEscape(&HT_escape_);
      break;
    }
    case 0x0A: { // LF , \n, ^J, Line Feed or New Line
      handleEscape(&LF_escape_);
      break;
    }
    case 0x0B: { // VT , \v, ^K, Vertical Tab
      handleEscape(&VT_escape_);
      break;
    }
    case 0x0C: { // FF , \f, ^L, Form Feed or New Page
      handleEscape(&FF_escape_);
      break;
    }
    case 0x0D: { // CR , \r, ^M, Carriage Return
      handleEscape(&CR_escape_);
      break;
    }
    case 0x0E: { // SO ,   , ^N, Shift Out
      handleEscape(&SO_escape_);
      break;
    }
    case 0x0F: { // SI ,   , ^O, Shift In
      handleEscape(&SI_escape_);
      break;
    }
    case 0x10: { // DLE,   , ^P
      handleEscape(&DLE_escape_);
      break;
    }
    case 0x11: { // DC1,   , XON, ^Q
      handleEscape(&DC1_escape_);
      break;
    }
    case 0x12: { // DC2,   , ^R
      handleEscape(&DC2_escape_);
      break;
    }
    case 0x13: { // DC3,   , XOFF, ^S
      handleEscape(&DC3_escape_);
      break;
    }
    case 0x14: { // DC4,   , ^T
      handleEscape(&DC4_escape_);
      break;
    }
    case 0x15: { // NAK,   , ^U
      handleEscape(&NAK_escape_);
      break;
    }
    case 0x16: { // SYN,   , ^V
      handleEscape(&SYN_escape_);
      break;
    }
    case 0x17: { // ETB,   , ^W
      handleEscape(&ETB_escape_);
      break;
    }
    case 0x18: { // CAN,   , ^X
      handleEscape(&CAN_escape_);
      break;
    }
    case 0x19: { // EM ,   , ^Y
      handleEscape(&EM_escape_);
      break;
    }
    case 0x1A: { // SUB,   , ^Z
      handleEscape(&SUB_escape_);
      break;
    }
    case 0x1B: { // ESC, \e, ^[, Escape
      setInEscape(true);

      escape_buf_.clear();
      escape_buf_.addChar(c);

      break;
    }
    case 0x1C: { // FS ,^\ , Quit
      if (is4014()) {
        // Point Plot Mode
        assert(false);
      }

      handleEscape(&FS_escape_);

      break;
    }
    case 0x1D: { // GS ,^]
      if (is4014()) {
        static int last_yl = 96, last_yh = 32;
        static int last_xl = 64, last_xh = 32;
        static int last_x1 = 0 , last_y1 = 0;

        // Graph Mode
        int pos1 = *pos;

        typedef std::vector<char> Chars;

        auto mode = CEscapeDataGS::Mode::MOVE_TO;

        if (*pos < len && str[*pos] == 7) {
          ++(*pos);

          mode = CEscapeDataGS::Mode::LINE_TO;
        }

        Chars chars;

        while (*pos < len && str[*pos] >= 32) {
          char c1 = str[(*pos)++];

          chars.push_back(c1);
        }

        if (*pos >= len) {
          *pos = pos1 - 1;
          return false;
        }

        //---

        // Low X : 64-95   High X : 32-63
        // Low Y : 96-127  High Y : 32-63
        uint ic = 0;
        uint nc = chars.size();

        while (true) {
          // read yl, yh
          int yl = last_yl;
          int yh = last_yh;

          if (ic < nc) {
            char c1 = chars[ic++];

            // low y
            if      (c1 >= 96) {
              yl = c1;
            }
            // low x
            else if (c1 >= 64) {
              --ic;
            }
            // high x or y
            else {
              yh = c1;

              if (ic < nc) {
                c1 = chars[ic++];

                // low y
                if      (c1 >= 96) {
                  yl = c1;
                }
                // low x
                else if (c1 >= 64) {
                  --ic;
                  --ic;
                }
                else {
                  assert(false);
                }
              }
            }
          }
          else {
            break;
          }

          //---

          // read extra bits
          int x1 = last_x1, y1 = last_y1;

          if (ic < nc && chars[ic] >= 96) {
            int xy = yl - 96;

            yl = chars[ic++];

            assert(yl >= 96 && yl <= 127);

            x1 =  xy & 0x03;
            y1 = (xy & 0x0C) >> 2;
          }
          else {
          }

          //---

          // read xl, xh
          int xl = last_xl;
          int xh = last_xh;

          if (ic < nc) {
            int c1 = chars[ic++];

            // low y
            if      (c1 >= 96) {
              --ic;
            }
            // low x
            else if (c1 >= 64) {
              xl = c1;
            }
            // high x or y
            else {
              xh = c1;

              if (ic < nc) {
                c1 = chars[ic++];

                // low y
                if      (c1 >= 96) {
                  --ic;
                  --ic;
                }
                // low x
                else if (c1 >= 64) {
                  xl = c1;
                }
                else {
                  assert(false);
                }
              }
            }
          }

          //---

          last_xl = xl; last_xh = xh;
          last_yl = yl; last_yh = yh;
          last_x1 = x1; last_y1 = y1;

          //---

          bool xvalid = false; int x = 0;
          bool yvalid = false; int y = 0;

          if (xh >= 32 && xh <= 63 && xl >= 64 && xl <= 95) {
            xvalid = true;
            x      = 128*(xh - 32) + 4*(xl - 64) + x1;
          }

          if (yh >= 32 && yh <= 63 && yl >= 96 && yl <= 127) {
            yvalid = true;
            y      = 128*(yh - 32) + 4*(yl - 96) + y1;
          }

          //---

          if (xvalid && yvalid) {
            GS_escape_.x    = x;
            GS_escape_.y    = y;
            GS_escape_.mode = mode;

            handleEscape(&GS_escape_);
          }
          else {
            assert(false);
          }

          //---

          mode = CEscapeDataGS::Mode::LINE_TO;
        }
      }
      else {
        handleEscape(&GS_escape_);
      }

      break;
    }
    case 0x1E: { // RS ,^^
      if (is4014()) {
        // Incremental Plot Mode
        assert(false);
      }

      handleEscape(&RS_escape_);

      break;
    }
    case 0x1F: { // US ,^_
      if (is4014()) {
        // Alpha Mode
        //assert(false);

        UNHANDLED1("4014 Escape", c)
      }

      handleEscape(&US_escape_);

      break;
    }
    case 0x7f: { // DEL
      handleEscape(&DEL_escape_);
      break;
    }
  }

  return true;
}

void
CEscapeParse::
addEscapeChars(const char *str, uint len, uint *pos)
{
  escape_buf_.addChars(str, len);

  *pos = 0;

  if (isEscape(escape_buf_.getChars(), pos))
    setInEscape(false);
}

void
CEscapeParse::
addEscapeChar(char c)
{
  escape_buf_.addChar(c);

  uint i = 0;

  if (isEscape(escape_buf_.getChars(), &i)) {
    setInEscape(false);

    if (i < escape_buf_.getLen()) {
      std::string buf(&escape_buf_.getChars()[i], escape_buf_.getLen() - i);

      addOutputChars(buf.c_str(), buf.size());
    }
  }
}

void
CEscapeParse::
addGraphicChar(char c)
{
  handleGraphic(c);
}

bool
CEscapeParse::
isEscape(const char *str, uint *pos)
{
  if (is4014())
    return is4014Escape(str, pos);

  //---

  bool flag = false;

  uint pos1 = *pos;

  assert(str[*pos] == '\033');

  ++(*pos);

  if (isalpha(str[*pos])) {
    flag = isAlphaEscape(str, pos);
    return flag;
  }

  if (isdigit(str[*pos])) {
    flag = isDigitEscape(str, pos);
    return flag;
  }

  char c = str[*pos];

  switch (c) {
    case ' ': {
      flag = isSpaceEscape(str, pos);
      return flag;
    }
    case '#': {
      flag = isHashEscape(str, pos);
      return flag;
    }
    case '%': {
      flag = isPercentEscape(str, pos);
      return flag;
    }
    case '(': {
      flag = isLParenEscape(str, pos);
      return flag;
    }
    case ')': {
      flag = isRParenEscape(str, pos);
      return flag;
    }
    case '*': {
      flag = isAsteriskEscape(str, pos);
      return flag;
    }
    case '+': {
      flag = isPlusEscape(str, pos);
      return flag;
    }

    // TODO:
    //   ESC - C   Designate G1 Character Set (VT300).
    //     The same character sets apply as for ESC ( C.
    //   ESC . C   Designate G2 Character Set (VT300).
    //     The same character sets apply as for ESC ( C.
    //   ESC / C   Designate G3 Character Set (VT300).
    //     These work for 96-character sets only.
    //     C = A  -> ISO Latin-1 Supplemental.

    case '-': {
      ++(*pos);

      setInEscape(false);

      // Designate G1 Character Set (VT300).
      // The same character sets apply as for ESC ( C

      UNHANDLED1("Escape", c)

      flag = true;

      return flag;
    }
    case '.': {
      ++(*pos);

      setInEscape(false);

      // Designate G2 Character Set (VT300).
      // The same character sets apply as for ESC ( C

      UNHANDLED1("Escape", c)

      flag = true;

      return flag;
    }
    case '/': {
      ++(*pos);

      setInEscape(false);

      // Designate G3 Character Set (VT300).
      // The same character sets apply as for ESC ( C

      UNHANDLED1("Escape", c)

      flag = true;

      return flag;
    }

    case '<': {
      ++(*pos);

      setInEscape(false);

      // Exit VT52 mode (Enter VT100 mode)
      if (isVT52()) {
        setVT52(false);
      }
      else {
        UNHANDLED1("Escape", c)
      }

      flag = true;

      return flag;
    }
    // DECKPAM, Application Keypad (VT100)
    // Enter alternate keypad mode (VT52)
    case '=': {
      ++(*pos);

      setInEscape(false);

      // Enter alternate keypad mode
      if (isVT52()) {
        // TODO
        UNHANDLED1("VT52: DECKPAM", '=');
      }
      // DECKPAM, Application Keypad
      else {
        handleEscape(&DECPAM_escape_);
      }

      flag = true;

      return flag;
    }
    // DECKPNM, Normal Keypad (VT100)
    // Exit alternate keypad mode (VT52)
    case '>': {
      ++(*pos);

      setInEscape(false);

      if (isVT52()) {
        // TODO
        UNHANDLED1("VT52: DECKPAM", '>');
      }
      else {
        handleEscape(&DECPNM_escape_);
      }

      flag = true;

      return flag;
    }

    // LS3R, Invoke G3 Character Set as GR()
    case '|': {
      ++(*pos);

      setInEscape(false);

      handleEscape(&LS3R_escape_);

      flag = true;

      return flag;
    }
    // LS2R, Invoke G2 Character Set as GR()
    case '}': {
      ++(*pos);

      setInEscape(false);

      handleEscape(&LS2R_escape_);

      flag = true;

      return flag;
    }
    // LS1R, Invoke G1 Character Set as GR()
    case '~': {
      ++(*pos);

      setInEscape(false);

      handleEscape(&LS1R_escape_);

      flag = true;

      return flag;
    }

    //--------------

    // CSI (0x9b) Control Sequence Introducer
    case '[': {
      flag = isCSIEscape(str, pos);
      return flag;
    }

    // OSC (0x9d) Operating System Command
    case ']': {
      flag = isOSCEscape(str, pos);
      return flag;
    }

    //--------------

    // ST (0x9c) String Terminator
    case '\\': {
      setInEscape(false);

      UNHANDLED1("Escape", c)

      return flag;
    }

    // APC (0x9f) Application Program Command
    case '_': {
      flag = isAPCEscape(str, pos);
      return flag;
    }

    //--------------

    // PM (0x9e) Privacy Message
    case '^': {
      setInEscape(false);

      UNHANDLED1("Escape", c)

      break;
    }

    //--------------

    default: {
      setInEscape(false);

      UNHANDLED1("Escape", c)

      break;
    }
  }

  //--------------

  *pos = pos1;

  ++(*pos);

  while (str[*pos] != '\0' && str[*pos] != '\n')
    ++(*pos);

  if (str[*pos] == '\n') {
    --(*pos);

    flag = true;

    return flag;
  }

  //--------------

  *pos = pos1;

  return flag;
}

bool
CEscapeParse::
is4014Escape(const char *str, uint *pos)
{
  assert(str[*pos] == '\033');

  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  switch (c) {
    // ESC ETX : Switch to VT100 Mode
    case '\003': {
      setInEscape(false);

      Tek4014_escape_.mode = CEscapeDataTek4014::Mode::VT100;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC ENQ : Return Terminal Status
    case '\005': {
      setInEscape(false);

      Tek4014_escape_.mode = CEscapeDataTek4014::Mode::STATUS;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC FF: PAGE (Clear Screen)
    case '\014': {
      setInEscape(false);

      Tek4014_escape_.mode = CEscapeDataTek4014::Mode::CLEAR;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC SO : End 4015 APL mode
    case '\015': {
      setInEscape(false);

      Tek4014_escape_.mode  = CEscapeDataTek4014::Mode::APL;
      Tek4014_escape_.value = 0;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC SI : Begin 4015 APL mode
    case '\016': {
      setInEscape(false);

      Tek4014_escape_.mode  = CEscapeDataTek4014::Mode::APL;
      Tek4014_escape_.value = 1;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC ETB : COPY (Save Tektronix Codes to file COPYyyyy-mm-dd.hh:mm:ss)
    case '\027': {
      setInEscape(false);

      Tek4014_escape_.mode = CEscapeDataTek4014::Mode::COPY;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC CAN : Bypass Condition
    case '\030': {
      setInEscape(false);

      Tek4014_escape_.mode = CEscapeDataTek4014::Mode::BYPASS;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC SUB : GIN mode
    case '\032': {
      // start crosshair cursor, clears graph mode, activates bypass condition
      setInEscape(false);

      Tek4014_escape_.mode = CEscapeDataTek4014::Mode::GIN;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC FS : Special Point Plot Mode
    case '\034': {
      setInEscape(false);

      Tek4014_escape_.mode = CEscapeDataTek4014::Mode::POINT_PLOT;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC 8 : Select Large Character Set
    case '8': {
      setInEscape(false);

      Tek4014_escape_.mode  = CEscapeDataTek4014::Mode::CHAR_SET;
      Tek4014_escape_.value = 1;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC 9 : Select #2 Character Set
    case '9': {
      setInEscape(false);

      Tek4014_escape_.mode  = CEscapeDataTek4014::Mode::CHAR_SET;
      Tek4014_escape_.value = 2;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC : : Select #3 Character Set
    case ':': {
      setInEscape(false);

      Tek4014_escape_.mode  = CEscapeDataTek4014::Mode::CHAR_SET;
      Tek4014_escape_.value = 3;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC ; : Select Small Character Set
    case ';': {
      setInEscape(false);

      Tek4014_escape_.mode  = CEscapeDataTek4014::Mode::CHAR_SET;
      Tek4014_escape_.value = 0;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC ` : Normal Z Axis and Normal (solid) Vectors
    case '`': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::SOLID;
      Tek4014_escape_.zAxis     = CEscapeDataTek4014::ZAxis::NORMAL;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC a : Normal Z Axis and Dotted Line Vectors.
    case 'a': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::DOTTED;
      Tek4014_escape_.zAxis     = CEscapeDataTek4014::ZAxis::NORMAL;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC b : Normal Z Axis and Dot-Dashed Vectors.
    case 'b': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::DOT_DASH;
      Tek4014_escape_.zAxis     = CEscapeDataTek4014::ZAxis::NORMAL;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC c : Normal Z Axis and Short-Dashed Vectors.
    case 'c': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::SHORT_DASH;
      Tek4014_escape_.zAxis     = CEscapeDataTek4014::ZAxis::NORMAL;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC d : Normal Z Axis and Long-Dashed Vectors.
    case 'd': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::LONG_DASH;
      Tek4014_escape_.zAxis     = CEscapeDataTek4014::ZAxis::NORMAL;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC h : Defocused Z Axis and Normal (solid) Vectors.
    case 'h': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::SOLID;
      Tek4014_escape_.zAxis     = CEscapeDataTek4014::ZAxis::DEFOCUSED;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC i : Defocused Z Axis and Dotted Line Vectors.
    case 'i': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::DOTTED;
      Tek4014_escape_.zAxis     = CEscapeDataTek4014::ZAxis::DEFOCUSED;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC j : Defocused Z Axis and Dot-Dashed Vectors.
    case 'j': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::DOT_DASH;
      Tek4014_escape_.zAxis     = CEscapeDataTek4014::ZAxis::DEFOCUSED;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC k : Defocused Z Axis and Short-Dashed Vectors.
    case 'k': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::SHORT_DASH;
      Tek4014_escape_.zAxis     = CEscapeDataTek4014::ZAxis::DEFOCUSED;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC l : Defocused Z Axis and Long-Dashed Vectors.
    case 'l': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::LONG_DASH;
      Tek4014_escape_.zAxis     = CEscapeDataTek4014::ZAxis::DEFOCUSED;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC p : Write-Thru Mode and Normal (solid) Vectors.
    case 'p': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::SOLID;
      Tek4014_escape_.writeMode = CEscapeDataTek4014::WriteMode::WRITETHRU;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC q : Write-Thru Mode and Dotted Line Vectors.
    case 'q': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::DOTTED;
      Tek4014_escape_.writeMode = CEscapeDataTek4014::WriteMode::WRITETHRU;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC r : Write-Thru Mode and Dot-Dashed Vectors.
    case 'r': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::DOT_DASH;
      Tek4014_escape_.writeMode = CEscapeDataTek4014::WriteMode::WRITETHRU;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC s : Write-Thru Mode and Short-Dashed Vectors.
    case 's': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::SHORT_DASH;
      Tek4014_escape_.writeMode = CEscapeDataTek4014::WriteMode::WRITETHRU;

      handleEscape(&Tek4014_escape_);

      break;
    }
    // ESC t : Write-Thru Mode and Long-Dashed Vectors.
    case 't': {
      setInEscape(false);

      Tek4014_escape_.mode      = CEscapeDataTek4014::Mode::LINE_STYLE;
      Tek4014_escape_.lineStyle = CEscapeLineStyle::LONG_DASH;
      Tek4014_escape_.writeMode = CEscapeDataTek4014::WriteMode::WRITETHRU;

      handleEscape(&Tek4014_escape_);

      break;
    }

    // OSC Ps ; Pt BEL
    case ']': {
      bool flag = isOSCEscape(str, pos);

      return flag;
    }

    default: {
      setInEscape(false);

      UNHANDLED1("Escape", c)

      break;
    }
  }

  return true;
}

bool
CEscapeParse::
isSpaceEscape(const char *str, uint *pos)
{
  assert(str[*pos] == ' ');

  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  switch (c) {
    // ESC SP F : S7C1T (7-bit controls)
    case 'F': {
      setInEscape(false);

      handleEscape(&S7C1T_escape_);

      break;
    }
    // ESC SP G : S8C1T (8-bit controls)
    case 'G': {
      setInEscape(false);

      handleEscape(&S8C1T_escape_);
      break;
    }
    // ESC SP L : Set ANSI conformance level 1 (dpANS X3.134.1)
    case 'L': {
      setInEscape(false);

      handleEscape(&ANSI1_escape_);

      break;
    }
    // ESC SP M : Set ANSI conformance level 2 (dpANS X3.134.1)
    case 'M': {
      setInEscape(false);

      handleEscape(&ANSI2_escape_);

      break;
    }
    // ESC SP N : Set ANSI conformance level 3 (dpANS X3.134.1)
    case 'N': {
      setInEscape(false);

      handleEscape(&ANSI3_escape_);

      break;
    }
    default: {
      setInEscape(false);

      UNHANDLED1("Space Escape", c)

      break;
    }
  }

  return true;
}

bool
CEscapeParse::
isHashEscape(const char *str, uint *pos)
{
  assert(str[*pos] == '#');

  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  switch (c) {
    // ESC # 3 : DEC double-height line, top half (DECDHL)
    case '3': {
      setInEscape(false);

      DECDHL_escape_.pos = CEscapeDataDECDHL::Pos::TOP;

      handleEscape(&DECDHL_escape_);

      break;
    }
    // ESC # 4 : DEC double-height line, bottom half (DECDHL)
    case '4': {
      setInEscape(false);

      DECDHL_escape_.pos = CEscapeDataDECDHL::Pos::BOTTOM;

      handleEscape(&DECDHL_escape_);

      break;
    }
    // ESC # 5 : DEC single-width line (DECSWL)
    case '5': {
      setInEscape(false);

      handleEscape(&DECSWL_escape_);

      break;
    }
    // ESC # 6 : DEC double-width line (DECDWL)
    case '6': {
      setInEscape(false);

      handleEscape(&DECDWL_escape_);

      break;
    }
    // DEC Screen Alignment Test (DECALN)
    case '8': {
      setInEscape(false);

      handleEscape(&DECALN_escape_);

      break;
    }
    default: {
      setInEscape(false);

      UNHANDLED1("Hash Escape", c)

      break;
    }
  }

  return true;
}

bool
CEscapeParse::
isPercentEscape(const char *str, uint *pos)
{
  assert(str[*pos] == '%');

  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  switch (c) {
    // Select default character set.  That is ISO 8859-1 (ISO 2022)
    case '@': {
      setInEscape(false);

      handleEscape(&ISO8859_1_escape_);

      break;
    }
    // Select UTF-8 character set (ISO 2022)
    case 'G': { // UTF-8
      setInEscape(false);

      handleEscape(&UTF_8_escape_);

      break;
    }
    default: {
      setInEscape(false);

      UNHANDLED1("Percent Escape", c)

      break;
    }
  }

  return true;
}

bool
CEscapeParse::
isLParenEscape(const char *str, uint *pos)
{
  assert(str[*pos] == '(');

  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  // TODO: only certain chars allowed

  // Designate G0 Character Set (ISO 2022, VT100).
  // Final character C for designating 94-character sets.  In this list, 0, A and B
  // apply to VT100 and up, the remainder to VT220 and up. The VT220 character sets,
  // together with the Portuguese character set are activated by the National Replacement
  // Character controls. The A is a special case, since it is also activated by the
  // VT300-control for British Latin-1 separately from the National Replacement Character
  // controls.
  //   C = 0         -> DEC Special Character and Line Drawing Set.
  //   C = <         -> DEC Supplementary (VT200).
  //   C = % 5       -> DEC Supplementary Graphics (VT300).
  //   C = >         -> DEC Technical (VT300).
  //   C = A         -> United Kingdom (UK).
  //   C = B         -> United States (USASCII).
  //   C = 4         -> Dutch.
  //   C = C  or 5   -> Finnish.
  //   C = R  or f   -> French.
  //   C = Q  or 9   -> French Canadian (VT200, VT300).
  //   C = K         -> German.
  //   C = Y         -> Italian.
  //   C = `, E or 6 -> Norwegian/Danish.
  //   C = % 6       -> Portuguese (VT300).
  //   C = Z         -> Spanish.
  //   C = H  or 7   -> Swedish.
  //   C = =         -> Swiss.

  setInEscape(false);

  G0_escape_.c = c;

  handleEscape(&G0_escape_);

  return true;
}

bool
CEscapeParse::
isRParenEscape(const char *str, uint *pos)
{
  assert(str[*pos] == ')');

  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  // TODO: only certain chars allowed

  // Designate G1 Character Set (ISO 2022, VT100)
  // The same character sets apply as for ESC ( C

  setInEscape(false);

  G1_escape_.c = c;

  handleEscape(&G1_escape_);

  return true;
}

bool
CEscapeParse::
isAsteriskEscape(const char *str, uint *pos)
{
  assert(str[*pos] == '*');

  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  // TODO: only certain chars allowed

  // Designate G2 Character Set (ISO 2022, VT220).
  // The same character sets apply as for ESC ( C

  setInEscape(false);

  G2_escape_.c = c;

  handleEscape(&G2_escape_);

  return true;
}

bool
CEscapeParse::
isPlusEscape(const char *str, uint *pos)
{
  assert(str[*pos] == '+');

  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  // TODO: only certain chars allowed

  // Designate G3 Character Set (ISO 2022, VT220).
  // The same character sets apply as for ESC ( C

  setInEscape(false);

  G3_escape_.c = c;

  handleEscape(&G3_escape_);

  return true;
}

bool
CEscapeParse::
isAlphaEscape(const char *str, uint *pos)
{
  if (isVT52()) {
    if (isVT52AlphaEscape(str, pos))
      return true;
  }

  //------

  char c = str[(*pos)++];

  switch (c) {
    // IND (0x84) Index
    case 'D': {
      setInEscape(false);

      handleEscape(&IND_escape_);

      break;
    }
    // NEL (0x85) Next Line
    case 'E': {
      setInEscape(false);

      handleEscape(&NEL_escape_);

      break;
    }
    // SSA (Start of Selected Area)
    case 'F': {
      // Cursor Lower Left (if hpLowerLeftBugCompat resource)
      // escapeCursorLowerLeft();
      setInEscape(false);

      handleEscape(&SSA_escape_);

      break;
    }
    // ESA (End of Selected Area)
    case 'G': {
      setInEscape(false);

      handleEscape(&ESA_escape_);

      break;
    }
    // HTS (0x88) Tab Set
    case 'H': {
      setInEscape(false);

      handleEscape(&HTS_escape_);

      break;
    }
    // RI (0x8d) Reverse Index
    case 'M': {
      setInEscape(false);

      handleEscape(&RI_escape_);

      break;
    }
    // SS2 (0x8e) Single Shift Select of G2 Character Set
    case 'N': {
      setInEscape(false);

      handleEscape(&SS2_escape_);

      break;
    }
    // SS3 (0x8f) Single Shift Select of G3 Character Set
    case 'O': {
      setInEscape(false);

      handleEscape(&SS3_escape_);

      break;
    }
    // DCS (0x90) Device Control String
    case 'P': {
      return isDCSEscape(str, pos);
    }
    // CCH (0x94)
    case 'T': {
      setInEscape(false);

      handleEscape(&CCH_escape_);

      break;
    }
    // SPA (0x96) Start of Guarded Area
    case 'V': {
      setInEscape(false);

      handleEscape(&SPA_escape_);

      break;
    }
    // EPA (0x97) End of Guarded Area
    case 'W': {
      setInEscape(false);

      handleEscape(&EPA_escape_);

      break;
    }
    // SOS (0x98) Start of String
    case 'X': {
      setInEscape(false);

      handleEscape(&SOS_escape_);

      break;
    }
    // DECID (0x9a) Return Terminal ID
    case 'Z': {
      setInEscape(false);

      handleEscape(&DECID_escape_);

      break;
    }
    // RIS, Full reset
    case 'c': {
      setInEscape(false);

      handleEscape(&RIS_escape_);

      break;
    }
    // Memory Lock (per HP terminals). Locks memory above the cursor
    case 'l': {
      setInEscape(false);

      handleEscape(&MemoryLock_escape_);

      break;
    }
    // Memory Unlock (per HP terminals)
    case 'm': {
      setInEscape(false);

      handleEscape(&MemoryUnlock_escape_);

      break;
    }
    // Invoke the G2 Character Set as GL (LS2)
    case 'n': {
      setInEscape(false);

      handleEscape(&LS2_escape_);

      break;
    }
    // Invoke the G3 Character Set as GL (LS3)
    case 'o': {
      setInEscape(false);

      handleEscape(&LS3_escape_);

      break;
    }
    default: {
      setInEscape(false);

      UNHANDLED1("Alpha Escape", c)

      break;
    }
  }

  return true;
}

bool
CEscapeParse::
isVT52AlphaEscape(const char *str, uint *pos)
{
  char c = str[(*pos)++];

  switch (c) {
    // Cursor up.
    case 'A': {
      setInEscape(false);

      CUU_escape_.num = 0;
      CUU_escape_.nn  = 0;

      handleEscape(&CUU_escape_);

      break;
    }
    // Cursor down.
    case 'B': {
      setInEscape(false);

      CUD_escape_.num = 0;
      CUD_escape_.nn  = 0;

      handleEscape(&CUD_escape_);

      break;
    }
    // Cursor right.
    case 'C': {
      setInEscape(false);

      CUF_escape_.num = 0;
      CUF_escape_.nn  = 0;

      handleEscape(&CUF_escape_);

      break;
    }
    // Cursor left.
    case 'D': {
      setInEscape(false);

      CUB_escape_.num = 0;
      CUB_escape_.nn  = 0;

      handleEscape(&CUB_escape_);

      break;
    }
    // Enter graphics mode.
    case 'F': {
      setInEscape(false);

      UNHANDLED1("VT52 Escape", c)

      break;
    }
    // Exit graphics mode.
    case 'G': {
      setInEscape(false);

      UNHANDLED1("VT52 Escape", c)

      break;
    }
    // Move the cursor to the home position.
    case 'H': {
      static int num[2];

      setInEscape(false);

      CUP_escape_.num = num;
      CUP_escape_.nn  = 2;

      handleEscape(&CUP_escape_);
      break;
    }
    // Reverse line feed.
    case 'I': {
      setInEscape(false);

      UNHANDLED1("VT52 Escape", c)

      break;
    }
    // Erase from the cursor to the end of the screen.
    case 'J': {
      static int num[1];

      setInEscape(false);

      EL_escape_.num = num;
      EL_escape_.nn  = 1;

      handleEscape(&EL_escape_);

      ED_escape_.num = num;
      ED_escape_.nn  = 1;

      handleEscape(&ED_escape_);

      break;
    }
    // Erase from the cursor to the end of the line.
    case 'K': {
      static int num[2];

      setInEscape(false);

      EL_escape_.num = num;
      EL_escape_.nn  = 1;

      handleEscape(&EL_escape_);

      break;
    }
    // Move the cursor to given row and column.
    case 'Y': {
      static int num[2];

      setInEscape(false);

      num[0] = str[(*pos)++] - 0x1F;
      num[1] = str[(*pos)++] - 0x1F;

      CUP_escape_.num = num;
      CUP_escape_.nn  = 2;

      handleEscape(&CUP_escape_);
      break;
    }
    // Identify.
    case 'Z': {
      setInEscape(false);

      handleEscape(&ENQ_escape_);

      break;
    }
    default: {
      UNHANDLED1("VT52 Escape", c)

      break;
    }
  }

  return true;
}

bool
CEscapeParse::
isDigitEscape(const char *str, uint *pos)
{
  char c = str[(*pos)++];

  switch (c) {
    // Back Index (DECBI), VT420 and up
    case '6': {
      // TODO
      UNHANDLED1("Digit", c);

      break;
    }
    // Save Cursor (DECSC)
    case '7': {
      setInEscape(false);

      handleEscape(&DECSC_escape_);

      break;
    }
    // Restore Cursor (DECRC)
    case '8': {
      setInEscape(false);

      handleEscape(&DECRC_escape_);

      break;
    }
    // Forward Index (DECFI), VT420 and up
    case '9': {
      // TODO
      UNHANDLED1("Digit", c);

      break;
    }
    default: {
      setInEscape(false);

      UNHANDLED1("Digit Escape", c)

      break;
    }
  }

  return true;
}

bool
CEscapeParse::
isCSIEscape(const char *str, uint *pos)
{
  // TODO: handle 8 bit chars > 0x7f

  if (str[++(*pos)] == '\0')
    return false;

  char c1  = '\0';
  char cc1 = '\0';

  if      (str[*pos] == '!' || str[*pos] == '?' || str[*pos] == '>' || str[*pos] == '=') {
    c1 = str[(*pos)++];

    if (str[*pos] == '\0')
      return false;
  }
  else if (str[*pos] < 32) {
    cc1 = str[(*pos)++];

    if (str[*pos] == '\0')
      return false;
  }

  int nn, *num;

  if (! parseNumList(str, pos, &num, &nn))
    return false;

  if (str[*pos] == '\0')
    return false;

  char c2  = '\0';
  char cc2 = '\0';

  if (str[*pos] == '\"' || str[*pos] == '`' ||
      str[*pos] == '$'  || str[*pos] == '*' || str[*pos] == '&') {
    c2 = str[(*pos)++];

    if (str[*pos] == '\0')
      return false;
  }
  else if (str[*pos] < 32) {
    cc2 = str[(*pos)++];

    if (str[*pos] == '\0')
      return false;
  }

  char c3 = str[(*pos)++];

  //-------

  if (cc1 > 0 && cc1 < 32) {
    char str1[2] = { cc1, 0 };

    uint pos1 = 0;

    addControlChar(str1, &pos1, 1);
  }

  if (cc2 > 0 && cc2 < 32) {
    char str1[2] = { cc2, 0 };

    uint pos1 = 0;

    addControlChar(str1, &pos1, 1);
  }

  if      (c1 != '\0') {
    switch (c3) {
      // Erase in Display (DECSED).
      //   Ps = 0  -> Selective Erase Below (default).
      //   Ps = 1  -> Selective Erase Above.
      //   Ps = 2  -> Selective Erase All.
      case 'J': {
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          DECSED_escape_.num = num;
          DECSED_escape_.nn  = nn;

          handleEscape(&DECSED_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED2("CSI Escape", c1, c3)
        }

        break;
      }
      // Erase in Line (DECSEL).
      //  Ps = 0  -> Selective Erase to Right (default).
      //  Ps = 1  -> Selective Erase to Left.
      //  Ps = 2  -> Selective Erase All.
      case 'K': {
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          DECSEL_escape_.num = num;
          DECSEL_escape_.nn  = nn;

          handleEscape(&DECSEL_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED2("CSI Escape", c1, c3)
        }

        break;
      }
      // DA2
      case 'c': {
        if (c1 == '>' && c2 == '\0') {
          setInEscape(false);

          DA2_escape_.num = num;
          DA2_escape_.nn  = nn;

          handleEscape(&DA2_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED2("CSI Escape", c1, c3)
        }

        break;
      }
      // DEC Set Mode (DECSET : DEC SM)
      case 'h': {
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          // update parser for escape codes that change parse
          if      (nn == 1 && num[0] == 2)
            setVT52(false);
          else if (nn == 1 && num[0] == 38)
            set4014(true);

          //---

          DECSET_escape_.num = num;
          DECSET_escape_.nn  = nn;

          handleEscape(&DECSET_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED2("CSI Escape", c1, c3)
        }

        break;
      }
      // Media Copy (DECMC)
      case 'i': {
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          DECMC_escape_.num = num;
          DECMC_escape_.nn  = nn;

          handleEscape(&DECMC_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED2("CSI Escape", c1, c3)
        }

        break;
      }
      // DEC Reset Mode (DECRST : DEC RM)
      case 'l': {
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          // update parser for escape codes that change parse
          if      (nn == 1 && num[0] == 2)
            setVT52(true);
          else if (nn == 1 && num[0] == 38)
            set4014(false);

          //---

          DECRST_escape_.num = num;
          DECRST_escape_.nn  = nn;

          handleEscape(&DECRST_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED2("CSI Escape", c1, c3)
        }

        break;
      }
      // DECDSR
      case 'n': {
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          DECDSR_escape_.num = num;
          DECDSR_escape_.nn  = nn;

          handleEscape(&DECDSR_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED2("CSI Escape", c1, c3)
        }

        break;
      }
      // DECSTR
      case 'p': {
        if (c1 == '!' && c2 == '\0') {
          setInEscape(false);

          handleEscape(&DECSTR_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED2("CSI Escape", c1, c3)
        }

        break;
      }
      case 'r': { // DEC Restore Private Mode Values
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          DECRestorePriv_escape_.num = num;
          DECRestorePriv_escape_.nn  = nn;

          handleEscape(&DECRestorePriv_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED2("CSI Escape", c1, c3)
        }

        break;
      }
      case 's': { // DEC Save Private Mode Values
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          DECSavePriv_escape_.num = num;
          DECSavePriv_escape_.nn  = nn;

          handleEscape(&DECSavePriv_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED2("CSI Escape", c1, c3)
        }

        break;
      }
      default: {
        setInEscape(false);

        UNHANDLED2("CSI Escape", c1, c3)

        break;
      }
    }
  }
  else if (c2 != '\0') {
    if (c2 == '`' && (c3 != 'w' && c3 != 'z' &&
                      c3 != '{' && c3 != '|')) { // HPA
      --(*pos);

      setInEscape(false);

      HPA_escape_.num = num;
      HPA_escape_.nn  = nn;

      handleEscape(&HPA_escape_);
    }
    else {
      switch (c3) {
        case 'p': {
          // DECSCL
          if      (c2 == '\"') {
            setInEscape(false);

            DECSCL_escape_.num = num;
            DECSCL_escape_.nn  = nn;

            handleEscape(&DECSCL_escape_);
          }
          // DECRQM
          else if (c2 == '\"') {
            setInEscape(false);

            UNHANDLED2("CSI Escape", c2, c3)
          }
          else {
            setInEscape(false);

            UNHANDLED2("CSI Escape", c2, c3)
          }

          break;
        }
        case 'q': {
          // DECSCA
          if      (c2 == '\"') {
            setInEscape(false);

            DECSCA_escape_.num = num;
            DECSCA_escape_.nn  = nn;

            handleEscape(&DECSCA_escape_);
          }
          else {
            setInEscape(false);

            UNHANDLED2("CSI Escape", c2, c3)
          }

          break;
        }
        case 'r': { // DEC Change Private Mode Values
          if (c2 == '$') {
            setInEscape(false);

            DECCARA_escape_.num = num;
            DECCARA_escape_.nn  = nn;

            handleEscape(&DECCARA_escape_);
          }
          else {
            setInEscape(false);

            UNHANDLED2("CSI Escape", c2, c3)
          }

          break;
        }
        case 't': { // DEC Restore Private Mode Values
          if (c2 == '$') {
            setInEscape(false);

            DECRARA_escape_.num = num;
            DECRARA_escape_.nn  = nn;

            handleEscape(&DECRARA_escape_);
          }
          else {
            setInEscape(false);

            UNHANDLED2("CSI Escape", c2, c3)
          }

          break;
        }
        case 'v': { // DECCRA
          if (c2 == '$') {
            setInEscape(false);

            DECCRA_escape_.num = num;
            DECCRA_escape_.nn  = nn;

            handleEscape(&DECCRA_escape_);
          }
          else {
            setInEscape(false);

            UNHANDLED2("CSI Escape", c2, c3)
          }

          break;
        }
        case 'w': { // DECEFR, DECLRP
          if      (c2 == '`') {
            setInEscape(false);

            DECEFR_escape_.num = num;
            DECEFR_escape_.nn  = nn;

            handleEscape(&DECEFR_escape_);
          }
          else if (c2 == '&') {
            setInEscape(false);

            DECLRP_escape_.num = num;
            DECLRP_escape_.nn  = nn;

            handleEscape(&DECLRP_escape_);
          }
          else {
            setInEscape(false);

            UNHANDLED2("CSI Escape", c2, c3)
          }

          break;
        }
        case 'x': { // DECREQTPARM
          if      (c2 == '$') {
            setInEscape(false);

            DECFRA_escape_.num = num;
            DECFRA_escape_.nn  = nn;

            handleEscape(&DECFRA_escape_);
          }
          else if (c2 == '*') {
            setInEscape(false);

            DECSACE_escape_.num = num;
            DECSACE_escape_.nn  = nn;

            handleEscape(&DECSACE_escape_);
          }
          else {
            setInEscape(false);

            UNHANDLED2("CSI Escape", c2, c3)
          }

          break;
        }
        case 'z': { // DECELR
          if      (c2 == '`') {
            setInEscape(false);

            DECELR_escape_.num = num;
            DECELR_escape_.nn  = nn;

            handleEscape(&DECELR_escape_);
          }
          else if (c2 == '$') {
            setInEscape(false);

            DECERA_escape_.num = num;
            DECERA_escape_.nn  = nn;

            handleEscape(&DECERA_escape_);
          }
          else {
            setInEscape(false);

            UNHANDLED2("CSI Escape", c2, c3)
          }

          break;
        }

        case '{': { // DECSLE
          if      (c2 == '`') {
            setInEscape(false);

            DECSLE_escape_.num = num;
            DECSLE_escape_.nn  = nn;

            handleEscape(&DECSLE_escape_);
          }
          else if (c2 == '$') {
            setInEscape(false);

            DECSERA_escape_.num = num;
            DECSERA_escape_.nn  = nn;

            handleEscape(&DECSERA_escape_);
          }
          else {
            setInEscape(false);

            UNHANDLED2("CSI Escape", c2, c3)
          }

          break;
        }
        case '|': { // DECRQLP
          if (c2 == '`') {
            setInEscape(false);

            DECRQLP_escape_.num = num;
            DECRQLP_escape_.nn  = nn;

            handleEscape(&DECRQLP_escape_);
          }
          else {
            setInEscape(false);

            UNHANDLED2("CSI Escape", c2, c3)
          }

          break;
        }

        default: {
          setInEscape(false);

          UNHANDLED2("CSI Escape", c2, c3)

          break;
        }
      }
    }
  }
  else {
    switch (c3) {
      // ICH, Insert N spaces
      // Insert Ps (Blank) Character(s) (default = 1)
      case '@': {
        setInEscape(false);

        ICH_escape_.num = num;
        ICH_escape_.nn  = nn;

        handleEscape(&ICH_escape_);
        break;
      }
      // CUU, Cursor up N
      // Cursor Up Ps Times (default = 1)
      case 'A': {
        setInEscape(false);

        CUU_escape_.num = num;
        CUU_escape_.nn  = nn;

        handleEscape(&CUU_escape_);
        break;
      }
      // CUD, Cursor down N
      // Cursor Down Ps Times (default = 1)
      case 'B': {
        setInEscape(false);

        CUD_escape_.num = num;
        CUD_escape_.nn  = nn;

        handleEscape(&CUD_escape_);
        break;
      }
      // CUF, Cursor forward N
      // Cursor Forward Ps Times (default = 1)
      case 'C': {
        setInEscape(false);

        CUF_escape_.num = num;
        CUF_escape_.nn  = nn;

        handleEscape(&CUF_escape_);
        break;
      }
      // CUB, Cursor back N
      // Cursor Backward Ps Times (default = 1)
      case 'D': {
        setInEscape(false);

        CUB_escape_.num = num;
        CUB_escape_.nn  = nn;

        handleEscape(&CUB_escape_);
        break;
      }
      // CNL, Cursor next line N
      // Cursor Next Line Ps Times (default = 1)
      case 'E': {
        setInEscape(false);

        CNL_escape_.num = num;
        CNL_escape_.nn  = nn;

        handleEscape(&CNL_escape_);
        break;
      }
      // CPL, Cursor previous line N
      // Cursor Preceding Line Ps Times (default = 1)
      case 'F': {
        setInEscape(false);

        CPL_escape_.num = num;
        CPL_escape_.nn  = nn;

        handleEscape(&CPL_escape_);
        break;
      }
      // CHA, Cursor to column N
      // Cursor Character Absolute  [column] (default = [row,1])
      case 'G': {
        setInEscape(false);

        CHA_escape_.num = num;
        CHA_escape_.nn  = nn;

        handleEscape(&CHA_escape_);
        break;
      }
      // CUP, Cursor to row N1, column N2
      // Cursor Position [row;column] (default = [1,1]) (CUP)
      case 'H': {
        setInEscape(false);

        CUP_escape_.num = num;
        CUP_escape_.nn  = nn;

        handleEscape(&CUP_escape_);
        break;
      }
      // CHT
      // Cursor Forward Tabulation Ps tab stops (default = 1)
      case 'I': {
        setInEscape(false);

        CHT_escape_.num = num;
        CHT_escape_.nn  = nn;

        handleEscape(&CHT_escape_);
        break;
      }
      // Erase in Display (ED).
      //   Ps = 0  -> Erase Below (default).
      //   Ps = 1  -> Erase Above.
      //   Ps = 2  -> Erase All.
      //   Ps = 3  -> Erase Saved Lines (xterm).
      case 'J': {
        setInEscape(false);

        ED_escape_.num = num;
        ED_escape_.nn  = nn;

        handleEscape(&ED_escape_);
        break;
      }
      // Erase in Line (EL).
      //   Ps = 0  -> Erase to Right (default).
      //   Ps = 1  -> Erase to Left.
      //   Ps = 2  -> Erase All.
      case 'K': {
        setInEscape(false);

        EL_escape_.num = num;
        EL_escape_.nn  = nn;

        handleEscape(&EL_escape_);
        break;
      }
      case 'L': { // IL
        setInEscape(false);

        IL_escape_.num = num;
        IL_escape_.nn  = nn;

        handleEscape(&IL_escape_);
        break;
      }
      case 'M': { // DL
        setInEscape(false);

        DL_escape_.num = num;
        DL_escape_.nn  = nn;

        handleEscape(&DL_escape_);
        break;
      }
      case 'P': { // DCH
        setInEscape(false);

        DCH_escape_.num = num;
        DCH_escape_.nn  = nn;

        handleEscape(&DCH_escape_);
        break;
      }
      case 'R': { // Output from Report Cursor Position (no op)
        break;
      }
      case 'S': { // SU
        setInEscape(false);

        SU_escape_.num = num;
        SU_escape_.nn  = nn;

        handleEscape(&SU_escape_);
        break;
      }
      case 'T': { // SD
        if (nn != 5) {
          setInEscape(false);

          SD_escape_.num = num;
          SD_escape_.nn  = nn;

          handleEscape(&SD_escape_);
        }
        else {
          setInEscape(false);

          StartMouseTrack_escape_.num = num;
          StartMouseTrack_escape_.nn  = nn;

          handleEscape(&StartMouseTrack_escape_);
        }
        break;
      }
      case 'X': { // ECH
        setInEscape(false);

        ECH_escape_.num = num;
        ECH_escape_.nn  = nn;

        handleEscape(&ECH_escape_);
        break;
      }
      case 'Z': { // CBT
        setInEscape(false);

        CBT_escape_.num = num;
        CBT_escape_.nn  = nn;

        handleEscape(&CBT_escape_);
        break;
      }
      case 'b': { // REP
        setInEscape(false);

        REP_escape_.num = num;
        REP_escape_.nn  = nn;

        handleEscape(&REP_escape_);
        break;
      }
      case 'c': { // DA1
        setInEscape(false);

        DA1_escape_.num = num;
        DA1_escape_.nn  = nn;

        handleEscape(&DA1_escape_);

        break;
      }
      case 'd': { // VPA
        setInEscape(false);

        VPA_escape_.num = num;
        VPA_escape_.nn  = nn;

        handleEscape(&VPA_escape_);
        break;
      }
      // HVP
      case 'f': {
        setInEscape(false);

        HVP_escape_.num = num;
        HVP_escape_.nn  = nn;

        handleEscape(&HVP_escape_);
        break;
      }
      // TBC
      case 'g': {
        setInEscape(false);

        TBC_escape_.num = num;
        TBC_escape_.nn  = nn;

        handleEscape(&TBC_escape_);
        break;
      }
      // SM
      case 'h': {
        setInEscape(false);

        SM_escape_.num = num;
        SM_escape_.nn  = nn;

        handleEscape(&SM_escape_);
        break;
      }
      // Media Copy (MC)
      case 'i': {
        setInEscape(false);

        MC_escape_.num = num;
        MC_escape_.nn  = nn;

        handleEscape(&MC_escape_);
        break;
      }
      // ResetMode (RM)
      case 'l': {
        setInEscape(false);

        RM_escape_.num = num;
        RM_escape_.nn  = nn;

        handleEscape(&RM_escape_);
        break;
      }
      case 'm': { // SGR
        setInEscape(false);

        if (nn == 0) {
          num[0] = 0;
          nn     = 1;
        }

        int i = 0;

        while (i < nn) {
          switch (num[i]) {
            case 38:
            case 48: {
              SGR_escape_.num = &num[i];
              SGR_escape_.nn  = 4;

              handleEscape(&SGR_escape_);

              i += 4;

              break;
            }
            default: {
              SGR_escape_.num = &num[i];
              SGR_escape_.nn  = 1;

              handleEscape(&SGR_escape_);

              ++i;

              break;
            }
          }
        }

        break;
      }
      case 'n': // DSR (CPR ?)
        setInEscape(false);

        DSR_escape_.num = num;
        DSR_escape_.nn  = nn;

        handleEscape(&DSR_escape_);
        break;
      case 'q': { // DECSCA
        setInEscape(false);

        DECLL_escape_.num = num;
        DECLL_escape_.nn  = nn;

        handleEscape(&DECLL_escape_);
        break;
      }
      case 'r': { // DECSTBM, DEC Restore Private Mode Values
        setInEscape(false);

        if (nn == 2) {
          DECSTBM_escape_.num = num;
          DECSTBM_escape_.nn  = nn;

          handleEscape(&DECSTBM_escape_);
        }
        else {
          int num1[2] = { -1, -1 };
          DECSTBM_escape_.num = num1;
          DECSTBM_escape_.nn  = 2;

          handleEscape(&DECSTBM_escape_);
        }
        break;
      }
      case 's': { // SC
        setInEscape(false);

        handleEscape(&SC_escape_);
        break;
      }
      case 't': { // Window manip
        setInEscape(false);

        WindowManip_escape_.num = num;
        WindowManip_escape_.nn  = nn ;

        handleEscape(&WindowManip_escape_);
        break;
      }
      case 'u': { // SC
        setInEscape(false);

        handleEscape(&SC_escape_);
        break;
      }
      case 'x': { // DECREQTPARM
        setInEscape(false);

        DECREQTPARM_escape_.num = num;
        DECREQTPARM_escape_.nn  = nn ;

        handleEscape(&DECREQTPARM_escape_);
        break;
      }
      case 'y': { // DECTST
        setInEscape(false);

        DECTST_escape_.num = num;
        DECTST_escape_.nn  = nn;

        handleEscape(&DECTST_escape_);
        break;
      }

      default: {
        setInEscape(false);

        UNHANDLED1("CSI Escape", c3)

        break;
      }
    }
  }

  return true;
}

bool
CEscapeParse::
isOSCEscape(const char *str, uint *pos)
{
  if (str[++(*pos)] == '\0')
    return false;

  // get parameters
  int num = 0;

  if (isdigit(str[*pos])) {
    while (isdigit(str[*pos])) {
      num = 0;

      num = str[(*pos)++] - '0';

      while (isdigit(str[*pos]))
        num = 10*num + (str[(*pos)++] - '0');
    }

    if (str[*pos] == '\0')
      return false;
  }

  //---

  if (str[*pos] != ';')
    return false;

  ++(*pos);

  if (str[*pos] == '\0')
    return false;

  //---

  // read to end of escape
  std::string str1;
  int         osc_len;

  while (str[*pos] != '\0' && ! isOSCEscape(str, pos, &osc_len))
    str1 += str[(*pos)++];

  if (! isOSCEscape(str, pos, &osc_len))
    return false;

  (*pos) += osc_len;

  //--------

  OSC_escape_.num = num;
  OSC_escape_.str = str1;

  setInEscape(false);

  handleEscape(&OSC_escape_);

  return true;
}

bool
CEscapeParse::
isAPCEscape(const char *str, uint *pos)
{
  if (str[++(*pos)] == '\0')
    return false;

  std::string value;
  int         apc_len;

  value += str[(*pos)++];

  while (str[*pos] != '\0' && ! isAPCEscape(str, pos, &apc_len))
    value += str[(*pos)++];

  if (! isAPCEscape(str, pos, &apc_len))
    return false;

  (*pos) += apc_len;

  //---

  char c = value[0];

  switch (c) {
    case '<': {
      CStrParse parse(value);

      parse.skipChar();
      parse.skipSpace();

      std::string name;

      if (! parse.readIdentifier(name))
        name = "";

      typedef std::pair<std::string,std::string> NameValue;
      typedef std::vector<NameValue>             NameValueList;

      NameValueList name_values;

      while (! parse.eof() && ! parse.isString("/>")) {
        parse.skipSpace();

        std::string opt_name;

        if (! parse.readIdentifier(opt_name))
          break;

        std::string opt_value;

        parse.skipSpace();

        if (parse.isChar('=')) {
          parse.skipChar();

          parse.skipSpace();

          if (! parse.readString(opt_value, /*strip_quotes*/true))
            opt_value = "";
        }

        name_values.push_back(NameValue(opt_name, opt_value));
      }

      // <state dir="dirname"/>
      if      (name == "state") {
        for (const auto &name_value : name_values) {
          const std::string &opt_name  = name_value.first;
          const std::string &opt_value = name_value.second;

          if (opt_name == "dir") {
            setInEscape(false);

            set_dir_escape_.str = opt_value;

            handleEscape(&set_dir_escape_);
          }
        }
      }

      // <image filename="name" size="size" x1="x1" y1="y1" x2="x2" y2="y2"/>
      else if (name == "image") {
        std::string iname;
        int         size = -1;
        int         x1   = -1, y1 = -1;
        int         x2   = -1, y2 = -1;

        for (const auto &name_value : name_values) {
          const std::string &opt_name  = name_value.first;
          const std::string &opt_value = name_value.second;

          if      (opt_name == "filename")
            iname = opt_value;
          else if (opt_name == "size")
            size = CStrUtil::toInteger(opt_value);
          else if (opt_name == "x1")
            x1 = CStrUtil::toInteger(opt_value);
          else if (opt_name == "y1")
            y1 = CStrUtil::toInteger(opt_value);
          else if (opt_name == "x2")
            x2 = CStrUtil::toInteger(opt_value);
          else if (opt_name == "y2")
            y2 = CStrUtil::toInteger(opt_value);
        }

        setInEscape(false);

        if (size > 0) {
          auto *image = lookupFileImage(iname, size, size, true);

          sized_image_escape_.name  = iname;
          sized_image_escape_.image = image;
          sized_image_escape_.x1    = x1;
          sized_image_escape_.y1    = y1;
          sized_image_escape_.x2    = x2;
          sized_image_escape_.y2    = y2;

          handleEscape(&sized_image_escape_);
        }
        else {
          auto *image = lookupFileImage(iname);

          file_image_escape_.name  = iname;
          file_image_escape_.image = image;
          file_image_escape_.x1    = x1;
          file_image_escape_.y1    = y1;
          file_image_escape_.x2    = x2;
          file_image_escape_.y2    = y2;

          handleEscape(&file_image_escape_);
        }
      }

      // <pixel x="x" y="y" color="color"/>
      else if (name == "pixel") {
        typedef std::vector<CEscapeColor> Colors;

        Colors colors;
        int    x = 0, y = 0;

        for (const auto &name_value : name_values) {
          const std::string &opt_name  = name_value.first;
          const std::string &opt_value = name_value.second;

          if      (opt_name == "color") {
            CRGBA c1;

            std::string colorNames = opt_value;

            std::string::size_type p1 = 0;
            std::string::size_type p2 = colorNames.find(',');

            while (p2 != std::string::npos) {
              std::string color = colorNames.substr(p1, p2 - p1);

              CRGBA c2;

              if (color == "#")
                c2 = c1;
              else
                c2 = CRGBName::toRGBA(color);

              colors.push_back(CEscapeColorsInst->encode(c2));

              c1 = c2;
              p1 = p2 + 1;
              p2 = colorNames.find(',', p1);
            }

            std::string color = colorNames.substr(p1, p2 - p1);

            auto ec = CRGBName::toRGBA(color);

            colors.push_back(CEscapeColorsInst->encode(ec));
          }
          else if (opt_name == "x")
            x = CStrUtil::toInteger(opt_value);
          else if (opt_name == "y")
            y = CStrUtil::toInteger(opt_value);
        }

        setInEscape(false);

        for (const auto &color : colors) {
          pixel_escape_.x     = x;
          pixel_escape_.y     = y;
          pixel_escape_.color = color;

          handleEscape(&pixel_escape_);

          ++x;
        }
      }

      // <line x1="x1" y1="y1" x2="x2" y2="y2" color="color"/>
      else if (name == "line") {
        CRGBA color;
        int   x1 = 0, y1 = 0, x2 = 0, y2 = 0;

        for (const auto &name_value : name_values) {
          const std::string &opt_name  = name_value.first;
          const std::string &opt_value = name_value.second;

          if      (opt_name == "color")
            color = CRGBName::toRGBA(opt_value);
          else if (opt_name == "x1")
            x1 = CStrUtil::toInteger(opt_value);
          else if (opt_name == "y1")
            y1 = CStrUtil::toInteger(opt_value);
          else if (opt_name == "x2")
            x2 = CStrUtil::toInteger(opt_value);
          else if (opt_name == "y2")
            y2 = CStrUtil::toInteger(opt_value);
        }

        setInEscape(false);

        line_escape_.x1    = x1;
        line_escape_.y1    = y1;
        line_escape_.x2    = x2;
        line_escape_.y2    = y2;
        line_escape_.color = CEscapeColorsInst->encode(color);

        handleEscape(&line_escape_);
      }

      // <link type=<type> dir=<dir> name=<name>
      else if (name == "link") {
        std::string type = "text";
        std::string dir;
        std::string lname;

        for (const auto &name_value : name_values) {
          const std::string &opt_name  = name_value.first;
          const std::string &opt_value = name_value.second;

          if      (opt_name == "type")
            type = opt_value;
          else if (opt_name == "dir")
            dir  = opt_value;
          else if (opt_name == "name")
            lname = opt_value;
        }

        setInEscape(false);

        link_escape_.name = lname;
        link_escape_.path = dir;
        link_escape_.type = type;

        handleEscape(&link_escape_);
      }

      // <command name=<name> dir=<dir>

      else if (name == "command") {
        std::string cname;
        std::string dir;
        bool        start;

        for (const auto &name_value : name_values) {
          const std::string &opt_name  = name_value.first;
          const std::string &opt_value = name_value.second;

          if      (opt_name == "name")
            cname = opt_value;
          else if (opt_name == "dir")
            dir  = opt_value;
          else if (opt_name == "start")
            start = CStrUtil::toBool(opt_value);
        }

        setInEscape(false);

        cmd_escape_.name  = cname;
        cmd_escape_.path  = dir;
        cmd_escape_.start = start;

        handleEscape(&cmd_escape_);
      }

      // <preview path=<path>
      else if (name == "preview") {
        setInEscape(false);

        std::string path;

        for (const auto &name_value : name_values) {
          const std::string &opt_name  = name_value.first;
          const std::string &opt_value = name_value.second;

          if (opt_name == "path")
            path = opt_value;
        }

        preview_files_escape_.str = path;

        handleEscape(&preview_files_escape_);
      }

      // <paste text=<text>
      else if (name == "paste") {
        setInEscape(false);

        std::string text;

        for (const auto &name_value : name_values) {
          const std::string &opt_name  = name_value.first;
          const std::string &opt_value = name_value.second;

          if (opt_name == "text")
            text = opt_value;
        }

        paste_escape_.str = text;

        handleEscape(&paste_escape_);
      }

      // <trace flag=<bool>
      else if (name == "trace") {
        setInEscape(false);

        for (const auto &name_value : name_values) {
          const std::string &opt_name  = name_value.first;
          const std::string &opt_value = name_value.second;

          if (opt_name == "flag") {
            setInEscape(false);

            trace_escape_.b = CStrUtil::toBool(opt_value);

            handleEscape(&trace_escape_);
          }
        }
      }

      // debug flag=<bool>
      else if (name == "debug") {
        setInEscape(false);

        for (const auto &name_value : name_values) {
          const std::string &opt_name  = name_value.first;
          const std::string &opt_value = name_value.second;

          if (opt_name == "flag") {
            setInEscape(false);

            debug_escape_.b = CStrUtil::toBool(opt_value);

            handleEscape(&debug_escape_);
          }
        }
      }

      break;
    }
    default:
      break;
  }

  return true;
}

bool
CEscapeParse::
isDCSEscape(const char *str, uint *pos)
{
  if (str[*pos] == '\0')
    return false;

  std::string value;
  int         dcs_len;

  value += str[(*pos)++];

  while (str[*pos] != '\0' && ! isDCSTerm(str, pos, &dcs_len))
    value += str[(*pos)++];

  if (! isDCSTerm(str, pos, &dcs_len))
    return false;

  (*pos) += dcs_len;

  uint value_len = value.size();

  // DCS $ q Pt ST
  //   Request Status String (DECRQSS). The string following the "q" is one of the following:
  //     " q  -> DECSCA
  //     " p  -> DECSCL
  //     r    -> DECSTBM
  //     s    -> DECSLRM
  //     m    -> SGR
  //     SP q -> DECSCUSR
  // xterm responds with DCS 1 $ r Pt ST for valid requests, replacing the Pt with the
  // corresponding CSI string, or DCS 0 $ r Pt ST for invalid requests.
  if      (value_len >= 2 && value[0] == '$' && value[1] == 'q') {
    UNHANDLED1("DECRQSS", value)
  }
  // DCS + p Pt ST
  //   Set Termcap/Terminfo Data (xterm, experimental). The string following the "p" is a
  //   name to use for retrieving data from the terminal database. The data will be used
  //   for the "tcap" keyboard configuration's function- and special-keys, as well as by
  //   the Request Termcap/Terminfo String control.
  else if (value_len >= 2 && value[0] == '+' && value[1] == 'p') {
    UNHANDLED1("DCS", value)
  }
  // DCS + q Pt ST
  //   Request Termcap/Terminfo String (xterm, experimental). The string following the "q"
  //   is a list of names encoded in hexadecimal (2 digits per character) separated by ;
  //   which correspond to termcap or terminfo key names. Two special features are also
  //   recognized, which are not key names: Co for termcap colors (or colors for terminfo
  //   colors), and TN for termcap name (or name for terminfo name).
  //   xterm responds with DCS 1 + r Pt ST for valid requests, adding to Pt an = , and the
  //   value of the corresponding string that xterm would send, or DCS 0 + r Pt ST for
  //   invalid requests. The strings are encoded in hexadecimal (2 digits per character).
  else if (value_len >= 2 && value[0] == '+' && value[1] == 'q') {
    std::string value1 = value.substr(2);

    value_len -= 2;

    std::string key;

    bool valid = true;

    uint pos1 = 0;

    while (pos1 + 2 <= value_len) {
      uint i1, i2;

      if (! CStrUtil::decodeHexChar(value1[pos1    ], &i1) ||
          ! CStrUtil::decodeHexChar(value1[pos1 + 1], &i2)) {
        valid = false;
        break;
      }

      if (! valid) break;

      key += char(i1 << 4 | i2);

      pos1 += 2;
    }

    /* term cap keys:

       !1   shifted save key
       !2   shifted suspend key
       !3   shifted undo key
       #1   shifted help key
       #2   shifted home key
       #3   shifted input key
       #4   shifted cursor left key
       %0   redo key
       %1   help key
       %2   mark key
       %3   message key
       %4   move key
       %5   next-object key
       %6   open key
       %7   options key
       %8   previous-object key
       %9   print key
       %a   shifted message key
       %b   shifted move key
       %c   shifted next key
       &7   suspend key
       &8   undo key
       &9   shifted begin key
       *0   shifted find key
       *1   shifted command key
       *2   shifted copy key
       *3   shifted create key
       *4   shifted delete character
       *5   shifted delete line
       *6   select key
       *7   shifted end key
       *8   shifted clear line key
       *9   shifted exit key
       @0   find key
       @1   begin key
       @2   cancel key
       @3   close key
       @4   command key
       @5   copy key
       @6   create key
       @7   end key
       @8   enter/send key
       @9   exit key
       al   Insert one line
       AL   Insert %1 lines
       ac   Pairs of block graphic characters to map alternate character set
       ae   End alternative character set
       as   Start alternative character set for block graphic characters
       bc   Backspace, if not ^H
       bl   Audio bell
       bt   Move to previous tab stop
       cb   Clear from beginning of line to cursor
       cc   Dummy command character
       cd   Clear to end of screen
       ce   Clear to end of line
       ch   Move cursor horizontally only to column %1
       cl   Clear screen and cursor home
       cm   Cursor move to row %1 and column %2 (on screen)
       CM   Move cursor to row %1 and column %2 (in memory)
       cr   Carriage return
       cs   Scroll region from line %1 to %2
       ct   Clear tabs
       cv   Move cursor vertically only to line %1
       dc   Delete one character
       DC   Delete %1 characters
       dl   Delete one line
       DL   Delete %1 lines
       dm   Begin delete mode
       do   Cursor down one line
       DO   Cursor down #1 lines
       ds   Disable status line
       eA   Enable alternate character set
       Fa   The string sent by function key f46
       Fb   The string sent by function key f47
       ...  ...
       Fr   The string sent by function key f63
       hd   Move cursor a half line down
       ho   Cursor home
       hu   Move cursor a half line up
       i1   Initialization string 1 at login
       i3   Initialization string 3 at login
       is   Initialization string 2 at login
       ic   Insert one character
       IC   Insert %1 characters
       if   Initialization file
       im   Begin insert mode
       ip   Insert pad time and needed special characters after insert
       iP   Initialization program
       K1   upper left key on keypad
       K2   center key on keypad
       K3   upper right key on keypad
       K4   bottom left key on keypad
       K5   bottom right key on keypad
       k0   Function key 0
       k1   Function key 1
       k2   Function key 2
       k3   Function key 3
       k4   Function key 4
       k5   Function key 5
       k6   Function key 6
       k7   Function key 7
       k8   Function key 8
       k9   Function key 9
       k;   Function key 10
       ka   Clear all tabs key
       kA   Insert line key
       kb   Backspace key
       kB   Back tab stop
       kC   Clear screen key
       kd   Cursor down key
       kD   Key for delete character under cursor
       ke   turn keypad off
       kE   Key for clear to end of line
       kF   Key for scrolling forward/down
       kh   Cursor home key
       kH   Cursor hown down key
       kI   Insert character/Insert mode key
       kl   Cursor left key
       kL   Key for delete line
       kM   Key for exit insert mode
       kN   Key for next page
       kP   Key for previous page
       kr   Cursor right key
       kR   Key for scrolling backward/up
       LO   Turn soft labels on
       mb   Start blinking
       MC   Clear soft margins
       md   Start bold mode
       me   End all mode like so, us, mb, md and mr
       mh   Start half bright mode
       mk   Dark mode (Characters invisible)
       ML   Set left soft margin
       mm   Put terminal in meta mode
       mo   Put terminal out of meta mode
       mp   Turn on protected attribute
       mr   Start reverse mode
       MR   Set right soft margin
       nd   Cursor right one character
       nw   Carriage return command
       pc   Padding character
       pf   Turn printer off
       pk   Program key %1 to send string %2 as if typed by user
       pl   Program key %1 to execute string %2 in local mode
       pn   Program soft label %1 to to show string %2
       po   Turn the printer on
       pO   Turn the printer on for %1 (<256) bytes
       ps   Print screen contents on printer
       px   Program key %1 to send string %2 to computer
       r1   Reset string 1 to set terminal to sane modes
       r2   Reset string 2 to set terminal to sane modes
       r3   Reset string 3 to set terminal to sane modes
       RA   disable automatic margins
       rc   Restore saved cursor position
       rf   Reset string filename
       RF   Request for input from terminal
       RI   Cursor right %1 characters
       rp   Repeat character %1 for %2 times
       rP   Padding after character sent in replace mode
       rs   Reset string
       RX   Turn off XON/XOFF flow control
       sa   Set %1 %2 %3 %4 %5 %6 %7 %8 %9 attributes
       SA   enable automatic margins
       sc   Save cursor position
       se   End standout mode
       sf   Normal scroll one line
       SF   Normal scroll %1 lines
       so   Start standout mode
       sr   Reverse scroll
       SR   scroll back %1 lines
       st   Set tabulator stop in all rows at current column
       SX   Turn on XON/XOFF flow control
       ta   move to next hardware tab
       tc   Read in terminal description from another entry
       te   End program that uses cursor motion
       ti   Begin program that uses cursor motion
       ts   Move cursor to column %1 of status line
    */

    CKeyType ekey = CKEY_TYPE_NUL;

    if      (key == "ku") ekey = CKEY_TYPE_Up;
    else if (key == "kd") ekey = CKEY_TYPE_Down;
    else if (key == "kr") ekey = CKEY_TYPE_Right;
    else if (key == "kl") ekey = CKEY_TYPE_Left;

    else if (key == "k0") ekey = CKEY_TYPE_F1; // F0
    else if (key == "k1") ekey = CKEY_TYPE_F1;
    else if (key == "k2") ekey = CKEY_TYPE_F2;
    else if (key == "k3") ekey = CKEY_TYPE_F3;
    else if (key == "k4") ekey = CKEY_TYPE_F4;
    else if (key == "k5") ekey = CKEY_TYPE_F5;
    else if (key == "k6") ekey = CKEY_TYPE_F6;
    else if (key == "k7") ekey = CKEY_TYPE_F7;
    else if (key == "k8") ekey = CKEY_TYPE_F8;
    else if (key == "k9") ekey = CKEY_TYPE_F9;
    else if (key == "k;") ekey = CKEY_TYPE_F10;
    else if (key == "F1") ekey = CKEY_TYPE_F11;
    else if (key == "F2") ekey = CKEY_TYPE_F12;

    else if (key == "K1") ekey = CKEY_TYPE_Home;      // upper left keypad
    else if (key == "K2") ekey = CKEY_TYPE_DEL;       // center keypad (??)
    else if (key == "K3") ekey = CKEY_TYPE_Page_Up;   // upper right key pad
    else if (key == "K4") ekey = CKEY_TYPE_End;       // bottom left key pad
    else if (key == "K5") ekey = CKEY_TYPE_Page_Down; // bottom right key pad

    else if (key == "kP") ekey = CKEY_TYPE_Prior;
    else if (key == "kN") ekey = CKEY_TYPE_Next;

    else if (key == "kb") ekey = CKEY_TYPE_BackSpace;
    else if (key == "kB") ekey = CKEY_TYPE_Backtab;

    else if (key == "kI") ekey = CKEY_TYPE_Insert;
    else if (key == "kD") ekey = CKEY_TYPE_DEL;
    else if (key == "kh") ekey = CKEY_TYPE_Home;

    else if (key == "Co") ekey = CKEY_TYPE_c; // copyright

    else if (key == "#2") ekey = CKEY_TYPE_Home;  // Shift + Home
    else if (key == "#4") ekey = CKEY_TYPE_Left;  // Shift + Cursor Left
    else if (key == "%i") ekey = CKEY_TYPE_Right; // Shift + Right
    else if (key == "*7") ekey = CKEY_TYPE_End;   // Shift + End

    else if (key == "%1") ekey = CKEY_TYPE_Help;
    else if (key == "&8") ekey = CKEY_TYPE_Undo; // Undo

    else if (key == "@7") ekey = CKEY_TYPE_End;  // End

    else
      UNHANDLED1("DCS", key)

    setInEscape(false);

    DCS_escape_.key   = ekey;
    DCS_escape_.value = value1;

    handleEscape(&DCS_escape_);
  }
  // DCS 0 p Pt ST
  else if (value_len >= 2 && value[0] == '0' && value[1] == 'p') {
    std::string value1 = value.substr(2);

    CEscapeRegisState regisState;

    parseRegisString(value1, regisState);

    setInEscape(false);
  }
  // DCS q "
  else if (value_len >= 2 && value[0] == 'q' && value[1] == '"') {
    std::vector<CRGBA> colors;

    uint pos1 = 2;

    int nn, *num;

    if (! parseNumList(&value[0], &pos1, &num, &nn))
      return false;

    if (nn != 4)
      return false;

    int w = num[2];
    int h = num[3];

    auto image = CImageMgrInst->createImage();

    image->setType(CFILE_TYPE_IMAGE_SIX);

    image->setDataSize(w, h);

    //---

    int c = 0;

    while (pos1 < value_len && value[pos1] == '#') {
      ++pos1;

      int nn1, *num1;

      if (! parseNumList(&value[0], &pos1, &num1, &nn1))
        return false;

      if      (nn1 == 5) {
        int r = num1[2];
        int g = num1[3];
        int b = num1[4];

        colors.push_back(CRGBA(r/255.0, g/255.0, b/255.0));
      }
      else if (nn1 == 1) {
        c = num1[0];
      }
      else {
        return false;
      }
    }

    //------

    for (const auto &color : colors)
      image->addColor(color);

    //---

    uint *data = new uint [w*h];

    int x = 0;
    int y = 0;

    while (pos1 < value_len) {
      // repeat data
      if      (value[pos1] == '!') {
        ++pos1;

        int num1;

        if (! parseNumber(&value[0], &pos1, &num1))
          break;

        int d;

        if (! parseDCSImageData(&value[0], &pos1, &d))
          break;

        for (int i = 0; i < num1; ++i) {
          for (int j = 0; j < 6; ++j) {
            bool b = d & (1<<j);
            if (! b) continue;

            if (x >= w || y + j >= h)
              return false;

            data[x + (y + j)*w] = c;
          }

          ++x;
        }
      }
      // choose palette color
      else if (value[pos1] == '#') {
        ++pos1;

        int num1;

        if (! parseNumber(&value[0], &pos1, &num1))
          break;

        c = num1;
      }
      // carriage return
      else if (value[pos1] == '$') {
        ++pos1;

        x = 0;
      }
      // next line
      else if (value[pos1] == '-') {
        ++pos1;

        x = 0;

        y += 6;
      }
      // add data
      else {
        int d;

        if (! parseDCSImageData(&value[0], &pos1, &d))
          break;

        for (int j = 0; j < 6; ++j) {
          bool b = d & (1<<j);
          if (! b) continue;

          if (x >= w || y + j >= h)
            return false;

          data[x + (y + j)*w] = c;
        }

        ++x;
      }
    }

    //---

    image->setColorIndexData(data);

    delete [] data;

    //---

    setInEscape(false);

    image_escape_.image = image;

    handleEscape(&image_escape_);
  }
  else {
    UNHANDLED1("DCS", value)

    setInEscape(false);
  }

#if 0
  switch (c) {
    case '|': {
      UNHANDLED3("DECUDK", num[0], num[1], value)
      break;
    }
  }
#endif

  return true;
}

bool
CEscapeParse::
parseRegisString(const std::string &str, CEscapeRegisState &regisState)
{
  static std::string chars = "PVCFTSRWL@";

  uint pos = 0;
  uint len = str.size();

  RegisPoint currentPoint;

  while (pos < len) {
    char c = toupper(str[pos]);

    auto p = chars.find(c);

    if (p == std::string::npos)
      break;

    ++pos;

    std::vector<RegisData> values;

    getRegisData(str, pos, values);

    // position
    if      (c == 'P') {
      for (const auto &value : values) {
        if (value.parameter == "") {
          currentPoint = value.point;
        }
      }
    }
    // vector
    else if (c == 'V') {
      RegisPoint rp;

      for (const auto &value : values) {
        if      (value.parameter == "B") {
          rp = currentPoint;
        }
        else if (value.parameter == "E") {
          currentPoint = rp;
        }
        else if (value.parameter == "") {
          line_escape_.x1 = currentPoint.x;
          line_escape_.y1 = currentPoint.y;

          if (value.point.x_rel)
            line_escape_.x2 = line_escape_.x1 + value.point.x;
          else
            line_escape_.x2 = value.point.x;

          if (value.point.y_rel)
            line_escape_.y2 = line_escape_.y1 + value.point.y;
          else
            line_escape_.y2 = value.point.y;

          currentPoint = value.point;

          handleEscape(&line_escape_);
        }
      }
    }
    // curve
    else if (c == 'C') {
      for (const auto &value : values) {
         if (value.parameter == "") {
           // draw circle
         }
      }
    }
    // fill
    else if (c == 'F') {
      regisState.filled = true;

      for (const auto &value : values) {
         if (value.parameter != "") {
           parseRegisString(value.parameter, regisState);
         }
      }

      regisState.filled = false;
    }
    // text
    else if (c == 'T') {
    }
    // screen
    else if (c == 'S') {
      for (const auto &value : values) {
        if      (value.parameter == "E") {
          // clear screen
        }
        else if (value.parameter == "C1") {
          // show cursor
        }
      }
    }
    // report
    else if (c == 'R') {
    }
    // write
    else if (c == 'W') {
    }
    // load
    else if (c == 'L') {
    }
    // macrograph
    else if (c == '@') {
    }
  }

  return true;
}

bool
CEscapeParse::
isOSCEscape(const char *str, uint *pos, int *len)
{
  *len = 0;

  if (str[*pos] == '') {
    *len = 1;
    return true;
  }

  if (str[*pos] == '\033' && str[*pos + 1] == '\\') {
    *len = 2;
    return true;
  }

  return false;
}

bool
CEscapeParse::
isAPCEscape(const char *str, uint *pos, int *len)
{
  *len = 0;

  if (str[*pos] == '\033' && str[*pos + 1] == '\\') {
    *len = 2;
    return true;
  }

  return false;
}

bool
CEscapeParse::
isDCSTerm(const char *str, uint *pos, int *len)
{
  *len = 0;

  if (str[*pos] == '\033' && str[*pos + 1] == '\\') {
    *len = 2;
    return true;
  }

  return false;
}
bool
CEscapeParse::
parseNumList(const char *str, uint *pos, int **values, int *num_values)
{
  static int values1[256];

  memset(values1, 0, sizeof(values1));

  int num_values1 = 0;

  while (str[*pos] != '\0' && (str[*pos] == ';' || isdigit(str[*pos]))) {
    while (str[*pos] == ';') {
      values1[num_values1++] = 0;

      ++(*pos);

      if (str[*pos] == '\0')
        return false;
    }

    if (isdigit(str[*pos])) {
      while (str[*pos] != '\0' && isdigit(str[*pos])) {
        values1[num_values1] = 0;

        values1[num_values1] = str[(*pos)++] - '0';

        while (str[*pos] != '\0' && isdigit(str[*pos]))
          values1[num_values1] =
            10*values1[num_values1] + (str[(*pos)++] - '0');
      }

      ++num_values1;

      if (str[*pos] != ';')
        break;

      ++(*pos);

      if (str[*pos] == '\0')
        return false;
    }
  }

  if (str[*pos] == '\0')
    return false;

  *values     = values1;
  *num_values = num_values1;

  return true;
}

bool
CEscapeParse::
parseNumber(const char *str, uint *pos, int *num)
{
  if (! isdigit(str[*pos]))
    return false;

  *num = 0;

  while (str[*pos] != '\0' && isdigit(str[*pos]))
    *num = 10*(*num) + str[(*pos)++] - '0';

  return true;
}

bool
CEscapeParse::
parseDCSImageData(const char *str, uint *pos, int *data)
{
  int c = str[*pos];

  if (c < 0x3f)
    return false;

  int c1 = c - 0x3f;

  if (c1 > 0x3f)
    return false;

  *data = c1;

  ++(*pos);

  return true;
}

CImageFile *
CEscapeParse::
lookupFileImage(const std::string &filename)
{
  CImageFileSrc src(filename);

  return CImageMgrInst->lookupFile(src);
}

CImageFile *
CEscapeParse::
lookupFileImage(const std::string &filename, int width, int height, bool keep_aspect)
{
  CImageSizedFileSrc src(filename, width, height, keep_aspect);

  return CImageMgrInst->lookupFile(src);
}

bool
CEscapeParse::
updateStyleFromSGR(const CEscapeDataSGR *sgr, CCellStyle &style)
{
  int *num = sgr->num;
  int  nn  = sgr->nn;

  assert(nn);

  switch (num[0]) {
    // Normal (default)
    case 0: {
      style.reset();
      break;
    }
    // Bold
    case 1: {
      style.setBold(true);
      break;
    }
    // Faint, decreased intensity (ISO 6429)
    case 2: {
      style.setDim(true);
      break;
    }
    // Italicized (ISO 6429)
    case 3: {
      style.setItalic(true);
      break;
    }
    // Underlined
    case 4: {
      style.setUnderscore(true);
      break; {
    }
    // Blink (appears as Bold)
    case 5: {
      style.setBlink(true);
      break;
    }
    // Inverse
    case 7: {
      style.setInvert(true);
      break;
    }
    // Invisible, i.e., hidden (VT300)
    case 8: {
      style.setHidden(true);
      break;
    }
    // Crossed-out characters (ISO 6429)
    case 9:
      style.setStrikeout(true);
      break;
    }
    // Doubly-underlined (ISO 6429)
    case 21: {
      style.setBold(false);
      break;
    }
    // Normal (neither bold nor faint)
    case 22: {
      style.setDim(false);
      break;
    }
    // Not italicized (ISO 6429)
    case 23: {
      style.setItalic(false);
      break;
    }
    // Not underlined
    case 24: {
      style.setUnderscore(false);
      break;
    }
    // Steady (not blinking)
    case 25: {
      style.setBlink(false);
      break;
    }
    // Positive (not inverse)
    case 27: {
      style.setInvert(false);
      break;
    }
    // Visible, i.e., not hidden (VT300)
    case 28: {
      style.setHidden(false);
      break;
    }
    // Not crossed-out (ISO 6429)
    case 29: {
      style.setStrikeout(false);
      break;
    }
    // Set foreground color :
    //  30 -> Black.
    //  31 -> Red.
    //  32 -> Green.
    //  33 -> Yellow.
    //  34 -> Blue.
    //  35 -> Magenta.
    //  36 -> Cyan.
    //  37 -> White.
    case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: {
      style.setFg((CEscapeColor) (num[0] - 30));
      break;
    }
    // Set foreground color to the closest match in xterm's palette for the given RGB Pr/Pg/Pb
    // (ISO-8613-3 controls)
    case 38: {
      style.setFg(CRGB(num[1]/255.0, num[2]/255.0, num[3]/255.0));
      break;
    }
    // Set foreground color to default (original)
    case 39: {
      style.setFg(CEscapeColor::FG);
      break;
    }
    // Set background color :
    //  40 -> Black.
    //  41 -> Red.
    //  42 -> Green.
    //  43 -> Yellow.
    //  44 -> Blue.
    //  45 -> Magenta.
    //  46 -> Cyan.
    //  47 -> White.
    case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47: {
      style.setBg((CEscapeColor) (num[0] - 40));
      break;
    }
    // Set background color to the closest match in xterm's palette for the given RGB Pr/Pg/Pb
    // (ISO-8613-3 controls)
    case 48: {
      style.setBg(CRGB(num[1]/255.0, num[2]/255.0, num[3]/255.0));
      break;
    }
    // Set background color to default (original)
    case 49: {
      style.setBg(CEscapeColor::BG);
      break;
    }
    // Set foreground color (16-color support):
    //  90 -> Black.
    //  91 -> Red.
    //  92 -> Green.
    //  93 -> Yellow.
    //  94 -> Blue.
    //  95 -> Magenta.
    //  96 -> Cyan.
    //  97 -> White.
    case 90: case 91: case 92: case 93: case 94: case 95: case 96: case 97: {
      style.setFg((CEscapeColor) (num[0] - 80));
      break;
    }
    // Set background color (16-color support):
    //  100 -> Black.
    //  101 -> Red.
    //  102 -> Green.
    //  103 -> Yellow.
    //  104 -> Blue.
    //  105 -> Magenta.
    //  106 -> Cyan.
    //  107 -> White.
    case 100: case 101: case 102: case 103: case 104: case 105: case 106: case 107: {
      style.setBg((CEscapeColor) (num[0] - 90));
      break;
    }
    default: {
      return false;
    }
  }

  return true;
}

//-----------

void
CEscapeDataC::
print(std::ostream &os) const
{
  os << typeToName(type) << ";" << c;
}

void
CEscapeDataBool::
print(std::ostream &os) const
{
  os << typeToName(type) << ";" << b;
}

void
CEscapeDataStr::
print(std::ostream &os) const
{
  os << typeToName(type) << ";" << str;
}

void
CEscapeDataGS::
print(std::ostream &os) const
{
  if      (mode == Mode::NONE)
    os << typeToName(type);
  else if (mode == Mode::MOVE_TO)
    os << typeToName(type) << ";0;" << x << ";" << y;
  else if (mode == Mode::LINE_TO)
    os << typeToName(type) << ";1;" << x << ";" << y;
}

void
CEscapeDataDECDHL::
print(std::ostream &os) const
{
  os << typeToName(type) << ";";

  os << (pos == Pos::TOP ? "TOP" : "BOTTOM");
}

void
CEscapeDataDCS::
print(std::ostream &os) const
{
  os << typeToName(type) << ";" << CEvent::keyTypeName(key);
}

void
CEscapeDataNums::
print(std::ostream &os) const
{
  os << typeToName(type);

  for (int i = 0; i < nn; ++i)
    os << ";" << num[i];
}

void
CEscapeDataOSC::
print(std::ostream &os) const
{
  os << typeToName(type) << ";";

  switch (num) {
    case 0 : os << "icon_window_title"; break;
    case 1 : os << "icon_title"; break;
    case 2 : os << "window_title"; break;
    case 3 : os << "window_prop"; break;
    case 4 : os << "color"; break;
    case 10: os << "fg"; break;
    case 11: os << "bg"; break;
    case 12: os << "cursor_color"; break;
    case 50: os << "font"; break;
    default: os << num; break;
  }

  os << ";" << str;
}

void
CEscapeDataTek4014::
print(std::ostream &os) const
{
  os << typeToName(type) << ";";

  if      (mode == Mode::STATUS) {
    os << "STATUS";
  }
  else if (mode == Mode::APL) {
    os << "APL;" << (value ? "1" : "0");
  }
  else if (mode == Mode::COPY) {
    os << "COPY";
  }
  else if (mode == Mode::BYPASS) {
    os << "BYPASS";
  }
  else if (mode == Mode::GIN) {
    os << "GIN";
  }
  else if (mode == Mode::POINT_PLOT) {
    os << "POINT_PLOT";
  }
  else if (mode == Mode::CHAR_SET) {
    os << "CHAR_SET;";

    if      (value == 0)
      os << "SMALL";
    else if (value == 1)
      os << "LARGE";
    else if (value == 2)
      os << "2";
    else if (value == 3)
      os << "3";
    else
      assert(false);
  }
  else if (mode == Mode::LINE_STYLE) {
    os << "LINE_STYLE;";

    if (lineStyle == CEscapeLineStyle::SOLID &&
        zAxis     == ZAxis::NORMAL &&
        writeMode == WriteMode::NORMAL)
      os << "NORMAL";

    if (lineStyle != CEscapeLineStyle::SOLID) {
      if      (lineStyle == CEscapeLineStyle::DOTTED)
        os << "DOTTED";
      else if (lineStyle == CEscapeLineStyle::DOT_DASH)
        os << "DOT_DASH";
      else if (lineStyle == CEscapeLineStyle::SHORT_DASH)
        os << "SHORT_DASH";
      else if (lineStyle == CEscapeLineStyle::LONG_DASH)
        os << "LONG_DASH";
    }

    if (zAxis != ZAxis::NORMAL)
      os << "DEFOCUSED";

    if (writeMode != WriteMode::NORMAL)
      os << "WRITETHRU";
  }
}

void
CEscapeDataFileImage::
print(std::ostream &os) const
{
  os << typeToName(type) << ";" << name;
}

void
CEscapeDataImage::
print(std::ostream &os) const
{
  os << typeToName(type);
}

void
CEscapeDataPixel::
print(std::ostream &os) const
{
  os << typeToName(type) << ";" << uint(color) << ";" << x <<  ";" << y;
}

void
CEscapeDataLine::
print(std::ostream &os) const
{
  os << typeToName(type) << ";" << uint(color) << ";" <<
        x1 <<  ";" << y1 << ";" << x2 <<  ";" << y2;
}

void
CEscapeDataLink::
print(std::ostream &os) const
{
  os << typeToName(CEscapeData::type) << ";" << name << ";" << path <<  ";" << type;
}

void
CEscapeDataCmd::
print(std::ostream &os) const
{
  os << typeToName(CEscapeData::type) << ";" << name << ";" << path;
}

void
CEscapeData::
print(std::ostream &os) const
{
  os << typeToName(type);
}

const char *
CEscapeData::
typeToName(CEscapeType type)
{
  switch (type) {
    case CEscapeType::NUL:             return "NUL";
    case CEscapeType::SOH:             return "SOH";
    case CEscapeType::STX:             return "STX";
    case CEscapeType::ETX:             return "ETX";
    case CEscapeType::EOT:             return "EOT";
    case CEscapeType::ENQ:             return "ENQ";
    case CEscapeType::ACK:             return "ACK";
    case CEscapeType::BEL:             return "BEL";
    case CEscapeType::BS:              return "BS";
    case CEscapeType::HT:              return "HT"; // also "TAB"
    case CEscapeType::LF:              return "LF"; // also "NL"
    case CEscapeType::VT:              return "VT";
    case CEscapeType::FF:              return "FF"; // also "NP"
    case CEscapeType::CR:              return "CR";
    case CEscapeType::SO:              return "SO";
    case CEscapeType::SI:              return "SI";
    case CEscapeType::DLE:             return "DLE";
    case CEscapeType::DC1:             return "DC1";
    case CEscapeType::DC2:             return "DC2";
    case CEscapeType::DC3:             return "DC3";
    case CEscapeType::DC4:             return "DC4";
    case CEscapeType::NAK:             return "NAK";
    case CEscapeType::SYN:             return "SYN";
    case CEscapeType::ETB:             return "ETB";
    case CEscapeType::CAN:             return "CAN";
    case CEscapeType::EM:              return "EM";
    case CEscapeType::SUB:             return "SUB";
    case CEscapeType::FS:              return "FS";
    case CEscapeType::GS:              return "GS";
    case CEscapeType::RS:              return "RS";
    case CEscapeType::US:              return "US";
    case CEscapeType::DEL:             return "DEL";

    case CEscapeType::IND:             return "IND";
    case CEscapeType::NEL:             return "NEL";
    case CEscapeType::HTS:             return "HTS";
    case CEscapeType::RI:              return "RI";
    case CEscapeType::SS2:             return "SS2";
    case CEscapeType::SS3:             return "SS3";
    case CEscapeType::DCS:             return "DCS";
    case CEscapeType::SPA:             return "SPA";
    case CEscapeType::EPA:             return "EPA";
    case CEscapeType::SOS:             return "SOS";
    case CEscapeType::DECID:           return "DECID";

    case CEscapeType::G0:              return "G0";
    case CEscapeType::G1:              return "G1";
    case CEscapeType::G2:              return "G2";
    case CEscapeType::G3:              return "G3";

#if 0
    case CEscapeType::WIN:             return "WIN";
    case CEscapeType::CSI:             return "CSI";
#endif
    case CEscapeType::OSC:             return "OSC";

    case CEscapeType::DECSC:           return "DECSC";
    case CEscapeType::DECRC:           return "DECRC";
    case CEscapeType::DECPAM:          return "DECPAM";
    case CEscapeType::DECPNM:          return "DECPNM";

#if 0
    case CEscapeType::SCS:             return "SCS";
#endif

    case CEscapeType::RIS:             return "RIS";

    case CEscapeType::DECALN:          return "DECALN";

    case CEscapeType::ICH:             return "ICH";
    case CEscapeType::CUU:             return "CUU";
    case CEscapeType::CUD:             return "CUD";
    case CEscapeType::CUF:             return "CUF";
    case CEscapeType::CUB:             return "CUB";
    case CEscapeType::CNL:             return "CNL";
    case CEscapeType::CPL:             return "CPL";
    case CEscapeType::CHA:             return "CHA";
    case CEscapeType::CUP:             return "CUP";
    case CEscapeType::CHT:             return "CHT";
    case CEscapeType::ED:              return "ED";
    case CEscapeType::DECSED:          return "DECSED";
    case CEscapeType::EL:              return "EL";
    case CEscapeType::DECSEL:          return "DECSEL";
    case CEscapeType::IL:              return "IL";
    case CEscapeType::DL:              return "DL";
    case CEscapeType::DCH:             return "DCH";
    case CEscapeType::SU:              return "SU";
    case CEscapeType::SD:              return "SD";
    case CEscapeType::ECH:             return "ECH";
    case CEscapeType::CBT:             return "CBT";
    case CEscapeType::HPA:             return "HPA";
    case CEscapeType::REP:             return "REP";
    case CEscapeType::DA1:             return "DA1";
    case CEscapeType::DA2:             return "DA2";
    case CEscapeType::VPA:             return "VPA";
    case CEscapeType::HVP:             return "HVP";
    case CEscapeType::TBC:             return "TBC";
    case CEscapeType::SM:              return "SM";
    case CEscapeType::DECSET:          return "DECSET";
    case CEscapeType::MC:              return "MC";
    case CEscapeType::DECMC:           return "DECMC";
    case CEscapeType::RM:              return "RM";
    case CEscapeType::DECRST:          return "DECRST";
    case CEscapeType::SGR:             return "SGR";
    case CEscapeType::DSR:             return "DSR";
    case CEscapeType::DECDSR:          return "DECDSR";
    case CEscapeType::DECSTR:          return "DECSTR";
    case CEscapeType::DECSCL:          return "DECSCL";
    case CEscapeType::DECSTBM:         return "DECSTBM";
    case CEscapeType::DECCARA:         return "DECCARA";
    case CEscapeType::SC:              return "SC";
    case CEscapeType::DECRARA:         return "DECRARA";
    case CEscapeType::DECREQTPARM:     return "DECREQTPARM";
    case CEscapeType::DECELR:          return "DECELR";
    case CEscapeType::DECSLE:          return "DECSLE";
    case CEscapeType::DECRQLP:         return "DECRQLP";

    case CEscapeType::LS3R:            return "LS3R";
    case CEscapeType::LS2R:            return "LS2R";
    case CEscapeType::LS1R:            return "LS1R";
    case CEscapeType::S7C1T:           return "S7C1T";
    case CEscapeType::S8C1T:           return "S8C1T";
    case CEscapeType::ANSI1:           return "ANSI1";
    case CEscapeType::ANSI2:           return "ANSI2";
    case CEscapeType::ANSI3:           return "ANSI3";
    case CEscapeType::DECDHL:          return "DECDHL";
    case CEscapeType::DECSWL:          return "DECSWL";
    case CEscapeType::DECDWL:          return "DECDWL";
    case CEscapeType::ISO8859_1:       return "ISO8859_1";
    case CEscapeType::UTF_8:           return "UTF_8";
    case CEscapeType::SSA:             return "SSA";
    case CEscapeType::ESA:             return "ESA";
    case CEscapeType::CCH:             return "CCH";
    case CEscapeType::LS2:             return "LS2";
    case CEscapeType::LS3:             return "LS3";

    case CEscapeType::DECSCA:          return "DECSCA";
    case CEscapeType::DECCRA:          return "DECCRA";
    case CEscapeType::DECEFR:          return "DECEFR";
    case CEscapeType::DECLRP:          return "DECLRP";
    case CEscapeType::DECFRA:          return "DECFRA";
    case CEscapeType::DECSACE:         return "DECSACE";
    case CEscapeType::DECERA:          return "DECERA";
    case CEscapeType::DECSERA:         return "DECSERA";
    case CEscapeType::DECLL:           return "DECLL";
    case CEscapeType::DECTST:          return "DECTST";

    case CEscapeType::DECRestorePriv:  return "DECRestorePriv";
    case CEscapeType::DECSavePriv:     return "DECSavePriv";
    case CEscapeType::MemoryLock:      return "MemoryLock";
    case CEscapeType::MemoryUnlock:    return "MemoryUnlock";
    case CEscapeType::StartMouseTrack: return "StartMouseTrack";
    case CEscapeType::WindowManip:     return "WindowManip";

    case CEscapeType::TEK4014:         return "TEK4014";

    case CEscapeType::SET_DIR:         return "set_dir";
    case CEscapeType::SIZED_IMAGE:     return "sized_image";
    case CEscapeType::IMAGE:           return "image";
    case CEscapeType::PIXEL:           return "pixel";
    case CEscapeType::LINE:            return "line";
    case CEscapeType::LINK:            return "link";
    case CEscapeType::COMMAND:         return "command";
    case CEscapeType::PREVIEW_FILES:   return "preview_files";
    case CEscapeType::PASTE:           return "paste";
    case CEscapeType::TRACE:           return "trace";
    case CEscapeType::DEBUG:           return "debug";

    case CEscapeType::NONE:            return "none";
    default:                           return "???";
  }
}
