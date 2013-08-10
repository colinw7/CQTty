#include <CEscapeParse.h>
#include <CStrUtil.h>
#include <CStrParse.h>
#include <CImageLib.h>
#include <sstream>

using std::string;
using std::vector;
using std::ostream;
using std::ostringstream;
using std::pair;

#define UNHANDLED(m) { \
  ostringstream ostr; \
  ostr << "Unhandled Esc: " << m << "\n"; \
  log(ostr.str()); \
}

#define UNHANDLED1(m,a) { \
  ostringstream ostr; \
  ostr << "Unhandled Esc: " << m << " " << a << "\n"; \
  log(ostr.str()); \
}

#define UNHANDLED2(m,a,b) { \
  ostringstream ostr; \
  ostr << "Unhandled Esc: " << m << " " << a << " " << b << "\n"; \
  log(ostr.str()); \
}

#define UNHANDLED3(m,a,b,c) { \
  ostringstream ostr; \
  ostr << "Unhandled Esc: " << m << " " << a << " " << b << "" << c << "\n"; \
  log(ostr.str()); \
}

CEscapeParse::
CEscapeParse() :
 NUL_escape_            (CESCAPE_TYPE_NUL),
 SOH_escape_            (CESCAPE_TYPE_SOH),
 STX_escape_            (CESCAPE_TYPE_STX),
 ETX_escape_            (CESCAPE_TYPE_ETX),
 EOT_escape_            (CESCAPE_TYPE_EOT),
 ENQ_escape_            (CESCAPE_TYPE_ENQ),
 ACK_escape_            (CESCAPE_TYPE_ACK),
 BEL_escape_            (CESCAPE_TYPE_BEL),
 BS_escape_             (CESCAPE_TYPE_BS),
 HT_escape_             (CESCAPE_TYPE_HT),
 LF_escape_             (CESCAPE_TYPE_LF),
 VT_escape_             (CESCAPE_TYPE_VT),
 FF_escape_             (CESCAPE_TYPE_FF),
 CR_escape_             (CESCAPE_TYPE_CR),
 SO_escape_             (CESCAPE_TYPE_SO),
 SI_escape_             (CESCAPE_TYPE_SI),
 DLE_escape_            (CESCAPE_TYPE_DLE),
 DC1_escape_            (CESCAPE_TYPE_DC1),
 DC2_escape_            (CESCAPE_TYPE_DC2),
 DC3_escape_            (CESCAPE_TYPE_DC3),
 DC4_escape_            (CESCAPE_TYPE_DC4),
 NAK_escape_            (CESCAPE_TYPE_NAK),
 SYN_escape_            (CESCAPE_TYPE_SYN),
 ETB_escape_            (CESCAPE_TYPE_ETB),
 CAN_escape_            (CESCAPE_TYPE_CAN),
 EM_escape_             (CESCAPE_TYPE_EM),
 SUB_escape_            (CESCAPE_TYPE_SUB),
 FS_escape_             (CESCAPE_TYPE_FS),
 GS_escape_             (CESCAPE_TYPE_GS),
 RS_escape_             (CESCAPE_TYPE_RS),
 US_escape_             (CESCAPE_TYPE_US),
 DEL_escape_            (CESCAPE_TYPE_DEL),
 DECPAM_escape_         (CESCAPE_TYPE_DECPAM),
 DECPNM_escape_         (CESCAPE_TYPE_DECPNM),
 LS3R_escape_           (CESCAPE_TYPE_LS3R),
 LS2R_escape_           (CESCAPE_TYPE_LS2R),
 LS1R_escape_           (CESCAPE_TYPE_LS1R),
 S7C1T_escape_          (CESCAPE_TYPE_S7C1T),
 S8C1T_escape_          (CESCAPE_TYPE_S8C1T),
 ANSI1_escape_          (CESCAPE_TYPE_ANSI1),
 ANSI2_escape_          (CESCAPE_TYPE_ANSI2),
 ANSI3_escape_          (CESCAPE_TYPE_ANSI3),
 DECDHLTop_escape_      (CESCAPE_TYPE_DECDHLTop),
 DECDHLBottom_escape_   (CESCAPE_TYPE_DECDHLBottom),
 DECSWL_escape_         (CESCAPE_TYPE_DECSWL),
 DECDWL_escape_         (CESCAPE_TYPE_DECDWL),
 DECALN_escape_         (CESCAPE_TYPE_DECALN),
 ISO8859_1_escape_      (CESCAPE_TYPE_ISO8859_1),
 UTF_8_escape_          (CESCAPE_TYPE_UTF_8),
 G0_escape_             (CESCAPE_TYPE_G0),
 G1_escape_             (CESCAPE_TYPE_G1),
 G2_escape_             (CESCAPE_TYPE_G2),
 G3_escape_             (CESCAPE_TYPE_G3),
 IND_escape_            (CESCAPE_TYPE_IND),
 NEL_escape_            (CESCAPE_TYPE_NEL),
 ESA_escape_            (CESCAPE_TYPE_ESA),
 HTS_escape_            (CESCAPE_TYPE_HTS),
 RI_escape_             (CESCAPE_TYPE_RI),
 SS2_escape_            (CESCAPE_TYPE_SS2),
 SS3_escape_            (CESCAPE_TYPE_SS3),
 CCH_escape_            (CESCAPE_TYPE_CCH),
 SPA_escape_            (CESCAPE_TYPE_SPA),
 EPA_escape_            (CESCAPE_TYPE_EPA),
 SOS_escape_            (CESCAPE_TYPE_SOS),
 DECID_escape_          (CESCAPE_TYPE_DECID),
 RIS_escape_            (CESCAPE_TYPE_RIS),
 MemoryLock_escape_     (CESCAPE_TYPE_MemoryLock),
 MemoryUnlock_escape_   (CESCAPE_TYPE_MemoryUnlock),
 LS2_escape_            (CESCAPE_TYPE_LS2),
 LS3_escape_            (CESCAPE_TYPE_LS3),
 DECSC_escape_          (CESCAPE_TYPE_DECSC),
 DECRC_escape_          (CESCAPE_TYPE_DECRC),
 DECSED_escape_         (CESCAPE_TYPE_DECSED),
 DECSEL_escape_         (CESCAPE_TYPE_DECSEL),
 DA2_escape_            (CESCAPE_TYPE_DA2),
 DECSET_escape_         (CESCAPE_TYPE_DECSET),
 DECMC_escape_          (CESCAPE_TYPE_DECMC),
 DECRST_escape_         (CESCAPE_TYPE_DECRST),
 DECDSR_escape_         (CESCAPE_TYPE_DECDSR),
 DECSTR_escape_         (CESCAPE_TYPE_DECSTR),
 DECRestorePriv_escape_ (CESCAPE_TYPE_DECRestorePriv),
 DECSavePriv_escape_    (CESCAPE_TYPE_DECSavePriv),
 HPA_escape_            (CESCAPE_TYPE_HPA),
 DECSCL_escape_         (CESCAPE_TYPE_DECSCL),
 DECSCA_escape_         (CESCAPE_TYPE_DECSCA),
 DECCARA_escape_        (CESCAPE_TYPE_DECCARA),
 DECRARA_escape_        (CESCAPE_TYPE_DECRARA),
 DECCRA_escape_         (CESCAPE_TYPE_DECCRA),
 DECEFR_escape_         (CESCAPE_TYPE_DECEFR),
 DECLRP_escape_         (CESCAPE_TYPE_DECLRP),
 DECFRA_escape_         (CESCAPE_TYPE_DECFRA),
 DECSACE_escape_        (CESCAPE_TYPE_DECSACE),
 DECELR_escape_         (CESCAPE_TYPE_DECELR),
 DECERA_escape_         (CESCAPE_TYPE_DECERA),
 DECSLE_escape_         (CESCAPE_TYPE_DECSLE),
 DECSERA_escape_        (CESCAPE_TYPE_DECSERA),
 DECRQLP_escape_        (CESCAPE_TYPE_DECRQLP),
 ICH_escape_            (CESCAPE_TYPE_ICH),
 CUU_escape_            (CESCAPE_TYPE_CUU),
 CUD_escape_            (CESCAPE_TYPE_CUD),
 CUF_escape_            (CESCAPE_TYPE_CUF),
 CUB_escape_            (CESCAPE_TYPE_CUB),
 CNL_escape_            (CESCAPE_TYPE_CNL),
 CPL_escape_            (CESCAPE_TYPE_CPL),
 CHA_escape_            (CESCAPE_TYPE_CHA),
 CUP_escape_            (CESCAPE_TYPE_CUP),
 CHT_escape_            (CESCAPE_TYPE_CHT),
 ED_escape_             (CESCAPE_TYPE_ED),
 EL_escape_             (CESCAPE_TYPE_EL),
 IL_escape_             (CESCAPE_TYPE_IL),
 DL_escape_             (CESCAPE_TYPE_DL),
 DCH_escape_            (CESCAPE_TYPE_DCH),
 SU_escape_             (CESCAPE_TYPE_SU),
 SD_escape_             (CESCAPE_TYPE_SD),
 StartMouseTrack_escape_(CESCAPE_TYPE_StartMouseTrack),
 ECH_escape_            (CESCAPE_TYPE_ECH),
 CBT_escape_            (CESCAPE_TYPE_CBT),
 REP_escape_            (CESCAPE_TYPE_REP),
 DA1_escape_            (CESCAPE_TYPE_DA1),
 VPA_escape_            (CESCAPE_TYPE_VPA),
 HVP_escape_            (CESCAPE_TYPE_HVP),
 TBC_escape_            (CESCAPE_TYPE_TBC),
 SM_escape_             (CESCAPE_TYPE_SM),
 MC_escape_             (CESCAPE_TYPE_MC),
 RM_escape_             (CESCAPE_TYPE_RM),
 SGR_escape_            (CESCAPE_TYPE_SGR),
 DSR_escape_            (CESCAPE_TYPE_DSR),
 DECLL_escape_          (CESCAPE_TYPE_DECLL),
 DECSTBM_escape_        (CESCAPE_TYPE_DECSTBM),
 SC_escape_             (CESCAPE_TYPE_SC),
 WindowManip_escape_    (CESCAPE_TYPE_WindowManip),
 DECREQTPARM_escape_    (CESCAPE_TYPE_DECREQTPARM),
 DECTST_escape_         (CESCAPE_TYPE_DECTST),
 set_dir_escape_        (CESCAPE_TYPE_SET_DIR),
 sized_image_escape_    (CESCAPE_TYPE_SIZED_IMAGE),
 image_escape_          (CESCAPE_TYPE_IMAGE),
 pixel_escape_          (CESCAPE_TYPE_PIXEL),
 line_escape_           (CESCAPE_TYPE_LINE),
 preview_files_escape_  (CESCAPE_TYPE_PREVIEW_FILES),
 paste_escape_          (CESCAPE_TYPE_PASTE),
 trace_escape_          (CESCAPE_TYPE_TRACE),
 debug_escape_          (CESCAPE_TYPE_DEBUG),
 DCS_escape_            (CESCAPE_TYPE_DCS)
{
  in_escape_   = false;
  in_apc_type_ = false;
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

    //log(string("InEscape=") + (in_escape_ ? "1" : "0"));
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
  static bool   locked;
  static string buffer;

  if (locked) {
    buffer += string(str, len);
    return;
  }

  locked = true;

  //------

  uint i = 0;

  while (i < len) {
    if (! getInEscape()) {
      if      (str[i] < 32)
        addControlChar(str[i]);
      else if (str[i] < 127)
        addNormalChar(str[i]);
      else
        addGraphicChar(str[i]);

      ++i;
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
          int len1 = escape_buf_.getLen() - pos;

          assert(len1 >= 0 && len1 <= int(len));

          i = len - len1;

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

  if (! buffer.empty()) {
    string buffer1 = buffer;

    buffer.clear();

    addOutputChars(buffer1.c_str(), buffer1.size());
  }
}

void
CEscapeParse::
addNormalChar(char c)
{
  handleChar(c);
}

void
CEscapeParse::
addControlChar(char c)
{
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
      handleEscape(&FS_escape_);
      break;
    }
    case 0x1D: { // GS ,^]
      handleEscape(&GS_escape_);
      break;
    }
    case 0x1E: { // RS ,^^
      handleEscape(&RS_escape_);
      break;
    }
    case 0x1F: { // US
      handleEscape(&US_escape_);
      break;
    }
    case 0x7f: { // DEL
      handleEscape(&DEL_escape_);
      break;
    }
  }
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
      string buf(&escape_buf_.getChars()[i], escape_buf_.getLen() - i);

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
  char c;

  bool flag = false;

  uint pos1 = *pos;

  assert(str[*pos] == '');

  ++(*pos);

  if (isalpha(str[*pos])) {
    flag = isAlphaEscape(str, pos);
    goto done;
  }

  if (isdigit(str[*pos])) {
    flag = isDigitEscape(str, pos);
    goto done;
  }

  c = str[*pos];

  switch (c) {
    case ' ': {
      flag = isSpaceEscape(str, pos);
      goto done;
    }
    case '#': {
      flag = isHashEscape(str, pos);
      goto done;
    }
    case '%': {
      flag = isPercentEscape(str, pos);
      goto done;
    }
    case '(': {
      flag = isLParenEscape(str, pos);
      goto done;
    }
    case ')': {
      flag = isRParenEscape(str, pos);
      goto done;
    }
    case '*': {
      flag = isAsteriskEscape(str, pos);
      goto done;
    }
    case '+': {
      flag = isPlusEscape(str, pos);
      goto done;
    }

    case '=': { // DECPAM, Application Keypad
      ++(*pos);

      setInEscape(false);

      handleEscape(&DECPAM_escape_);

      flag = true;

      goto done;
    }
    case '>': { // DECPNM, Normal Keypad
      ++(*pos);

      setInEscape(false);

      handleEscape(&DECPNM_escape_);

      flag = true;

      goto done;
    }

    case '|': { // LS3R, Invoke G3 Character Set as GR()
      ++(*pos);

      setInEscape(false);

      handleEscape(&LS3R_escape_);

      flag = true;

      goto done;
    }
    case '}': { // LS2R, Invoke G2 Character Set as GR()
      ++(*pos);

      setInEscape(false);

      handleEscape(&LS2R_escape_);

      flag = true;

      goto done;
    }
    case '~': { // LS1R, Invoke G1 Character Set as GR()
      ++(*pos);

      setInEscape(false);

      handleEscape(&LS1R_escape_);

      flag = true;

      goto done;
    }

    //--------------

    case '[': {
      flag = isCSIEscape(str, pos);
      goto done;
    }

    case ']': {
      flag = isOSCEscape(str, pos);
      goto done;
    }

    //--------------

    case '\\': {
      setInEscape(false);

      UNHANDLED1("Escape", c)

      goto done;
    }

    case '_': {
      flag = isAPCEscape(str, pos);
      goto done;
    }

    //--------------

    case '^': { // PM
      setInEscape(false);

      UNHANDLED1("Escape", c)

      break;
    }

    //--------------

    default:
      setInEscape(false);

      UNHANDLED1("Escape", c)

      break;
  }

  //--------------

  *pos = pos1;

  ++(*pos);

  while (str[*pos] != '\0' && str[*pos] != '\n')
    ++(*pos);

  if (str[*pos] == '\n') {
    --(*pos);

    flag = true;

    goto done;
  }

  //--------------

  *pos = pos1;

 done:
  return flag;
}

bool
CEscapeParse::
isSpaceEscape(const char *str, uint *pos)
{
  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  switch (c) {
    case 'F': { // S7C1T
      setInEscape(false);

      handleEscape(&S7C1T_escape_);

      break;
    }
    case 'G': { // S8C1T
      setInEscape(false);

      handleEscape(&S8C1T_escape_);
      break;
    }
    case 'L': { // ANSI Set ANSI conformance 1
      setInEscape(false);

      handleEscape(&ANSI1_escape_);

      break;
    }
    case 'M': { // ANSI Set ANSI conformance 2
      setInEscape(false);

      handleEscape(&ANSI2_escape_);

      break;
    }
    case 'N': { // Set ANSI conformance 3
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
  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  switch (c) {
    case '3': { // DECDHL
      setInEscape(false);

      handleEscape(&DECDHLTop_escape_);

      break;
    }
    case '4': { // DECDHL
      setInEscape(false);

      handleEscape(&DECDHLBottom_escape_);

      break;
    }
    case '5': { // DECSWL
      setInEscape(false);

      handleEscape(&DECSWL_escape_);

      break;
    }
    case '6': { // DECDWL
      setInEscape(false);

      handleEscape(&DECDWL_escape_);

      break;
    }
    case '8': { // DECALN
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
  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  switch (c) {
    case '@': { // ISO 8859-1
      setInEscape(false);

      handleEscape(&ISO8859_1_escape_);

      break;
    }
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
  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  // TODO: only certain chars allowed

  setInEscape(false);

  G0_escape_.c = c;

  handleEscape(&G0_escape_);

  return true;
}

bool
CEscapeParse::
isRParenEscape(const char *str, uint *pos)
{
  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  setInEscape(false);

  G1_escape_.c = c;

  handleEscape(&G1_escape_);

  return true;
}

bool
CEscapeParse::
isAsteriskEscape(const char *str, uint *pos)
{
  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  setInEscape(false);

  G2_escape_.c = c;

  handleEscape(&G2_escape_);

  return true;
}

bool
CEscapeParse::
isPlusEscape(const char *str, uint *pos)
{
  if (str[++(*pos)] == '\0')
    return false;

  char c = str[(*pos)++];

  setInEscape(false);

  G3_escape_.c = c;

  handleEscape(&G3_escape_);

  return true;
}

bool
CEscapeParse::
isAlphaEscape(const char *str, uint *pos)
{
  char c = str[(*pos)++];

  switch (c) {
    case 'D': // IND
      setInEscape(false);

      handleEscape(&IND_escape_);

      break;
    case 'E': // NEL
      setInEscape(false);

      handleEscape(&NEL_escape_);

      break;
    case 'F': { // Cursor Lower Left (if hpLowerLeftBugCompat)
      // escapeCursorLowerLeft();
      setInEscape(false);

      UNHANDLED1("Alpha Escape", c)

      break;
    }
    case 'G': // ESA
      setInEscape(false);

      handleEscape(&ESA_escape_);

      break;
    case 'H': // HTS
      setInEscape(false);

      handleEscape(&HTS_escape_);

      break;
    case 'M': // RI
      setInEscape(false);

      handleEscape(&RI_escape_);

      break;
    case 'N': // SS2
      setInEscape(false);

      handleEscape(&SS2_escape_);

      break;
    case 'O': // SS3
      setInEscape(false);

      handleEscape(&SS3_escape_);

      break;
    case 'P': { // DCS
      return isDCSEscape(str, pos);
    }
    case 'T': // CCH
      setInEscape(false);

      handleEscape(&CCH_escape_);

      break;
    case 'V': // SPA
      setInEscape(false);

      handleEscape(&SPA_escape_);

      break;
    case 'W': // EPA
      setInEscape(false);

      handleEscape(&EPA_escape_);

      break;
    case 'X': // SOS
      setInEscape(false);

      handleEscape(&SOS_escape_);

      break;
    case 'Z': // DECID
      setInEscape(false);

      handleEscape(&DECID_escape_);

      break;
    case 'c': // RIS, Full reset
      setInEscape(false);

      handleEscape(&RIS_escape_);

      break;
    case 'l': // Memory Lock
      setInEscape(false);

      handleEscape(&MemoryLock_escape_);

      break;
    case 'm': // Memory Unlock
      setInEscape(false);

      handleEscape(&MemoryUnlock_escape_);

      break;
    case 'n': // LS2, Invoke G2 Character Set
      setInEscape(false);

      handleEscape(&LS2_escape_);

      break;
    case 'o': // LS3, Invoke G3 Character Set
      setInEscape(false);

      handleEscape(&LS3_escape_);

      break;
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
isDigitEscape(const char *str, uint *pos)
{
  char c = str[(*pos)++];

  switch (c) {
    case '7': { // DECSC, Save Cursor
      setInEscape(false);

      handleEscape(&DECSC_escape_);

      break;
    }
    case '8': { // DECRC, Restore Cursor
      setInEscape(false);

      handleEscape(&DECRC_escape_);

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

  if      (str[*pos] == '!' || str[*pos] == '?' ||
           str[*pos] == '>' || str[*pos] == '=') {
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

  if (cc1 > 0 && cc1 < 32)
    addControlChar(cc1);

  if (cc2 > 0 && cc2 < 32)
    addControlChar(cc2);

  if      (c1 != '\0') {
    switch (c3) {
      case 'J': { // DECSED
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          DECSED_escape_.num = num;
          DECSED_escape_.nn  = nn;

          handleEscape(&DECSED_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED1("CSI Escape", c3)
        }

        break;
      }
      case 'K': { // DECSEL
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          DECSEL_escape_.num = num;
          DECSEL_escape_.nn  = nn;

          handleEscape(&DECSEL_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED1("CSI Escape", c3)
        }

        break;
      }
      case 'c': { // DA2
        if (c1 == '>' && c2 == '\0') {
          setInEscape(false);

          DA2_escape_.num = num;
          DA2_escape_.nn  = nn;

          handleEscape(&DA2_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED1("CSI Escape", c3)
        }

        break;
      }
      case 'h': { // DECSET
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          DECSET_escape_.num = num;
          DECSET_escape_.nn  = nn;

          handleEscape(&DECSET_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED1("CSI Escape", c3)
        }

        break;
      }
      case 'i': { // DECMC
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          DECMC_escape_.num = num;
          DECMC_escape_.nn  = nn;

          handleEscape(&DECMC_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED1("CSI Escape", c3)
        }

        break;
      }
      case 'l': { // DECRST
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          DECRST_escape_.num = num;
          DECRST_escape_.nn  = nn;

          handleEscape(&DECRST_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED1("CSI Escape", c3)
        }

        break;
      }
      case 'n': // DECDSR
        if (c1 == '?' && c2 == '\0') {
          setInEscape(false);

          DECDSR_escape_.num = num;
          DECDSR_escape_.nn  = nn;

          handleEscape(&DECDSR_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED1("CSI Escape", c3)
        }

        break;
      case 'p': { // DECSTR
        if (c1 == '!' && c2 == '\0') {
          setInEscape(false);

          handleEscape(&DECSTR_escape_);
        }
        else {
          setInEscape(false);

          UNHANDLED1("CSI Escape", c3)
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

          UNHANDLED1("CSI Escape", c3)
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

          UNHANDLED1("CSI Escape", c3)
        }

        break;
      }
      default: {
        setInEscape(false);

        UNHANDLED1("CSI Escape", c3)

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
        case 'p': { // DECSCL
          if (c2 == '\"') {
            setInEscape(false);

            DECSCL_escape_.num = num;
            DECSCL_escape_.nn  = nn;

            handleEscape(&DECSCL_escape_);
          }
          else {
            setInEscape(false);

            UNHANDLED1("CSI Escape", c3)
          }

          break;
        }
        case 'q': { // DECSCA
          if (c2 == '\"') {
            setInEscape(false);

            DECSCA_escape_.num = num;
            DECSCA_escape_.nn  = nn;

            handleEscape(&DECSCA_escape_);
          }
          else {
            setInEscape(false);

            UNHANDLED1("CSI Escape", c3)
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

            UNHANDLED1("CSI Escape", c3)
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

            UNHANDLED1("CSI Escape", c3)
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

            UNHANDLED1("CSI Escape", c3)
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

            UNHANDLED1("CSI Escape", c3)
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

            UNHANDLED1("CSI Escape", c3)
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

            UNHANDLED1("CSI Escape", c3)
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

            UNHANDLED1("CSI Escape", c3)
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

            UNHANDLED1("CSI Escape", c3)
          }

          break;
        }

        default: {
          setInEscape(false);

          UNHANDLED1("CSI Escape", c3)

          break;
        }
      }
    }
  }
  else {
    switch (c3) {
      case '@': { // ICH, Insert N spaces
        setInEscape(false);

        ICH_escape_.num = num;
        ICH_escape_.nn  = nn;

        handleEscape(&ICH_escape_);
        break;
      }
      case 'A': { // CUU, Cursor up N
        setInEscape(false);

        CUU_escape_.num = num;
        CUU_escape_.nn  = nn;

        handleEscape(&CUU_escape_);
        break;
      }
      case 'B': { // CUD, Cursor down N
        setInEscape(false);

        CUD_escape_.num = num;
        CUD_escape_.nn  = nn;

        handleEscape(&CUD_escape_);
        break;
      }
      case 'C': { // CUF, Cursor forward N
        setInEscape(false);

        CUF_escape_.num = num;
        CUF_escape_.nn  = nn;

        handleEscape(&CUF_escape_);
        break;
      }
      case 'D': { // CUB, Cursor back N
        setInEscape(false);

        CUB_escape_.num = num;
        CUB_escape_.nn  = nn;

        handleEscape(&CUB_escape_);
        break;
      }
      case 'E': { // CNL, Cursor next line N
        setInEscape(false);

        CNL_escape_.num = num;
        CNL_escape_.nn  = nn;

        handleEscape(&CNL_escape_);
        break;
      }
      case 'F': { // CPL, Cursor previous line N
        setInEscape(false);

        CPL_escape_.num = num;
        CPL_escape_.nn  = nn;

        handleEscape(&CPL_escape_);
        break;
      }
      case 'G': { // CHA, Cursor to column N
        setInEscape(false);

        CHA_escape_.num = num;
        CHA_escape_.nn  = nn;

        handleEscape(&CHA_escape_);
        break;
      }
      case 'H': { // CUP, Cursor to row N1, column N2
        setInEscape(false);

        CUP_escape_.num = num;
        CUP_escape_.nn  = nn;

        handleEscape(&CUP_escape_);
        break;
      }
      case 'I': { // CHT
        setInEscape(false);

        CHT_escape_.num = num;
        CHT_escape_.nn  = nn;

        handleEscape(&CHT_escape_);
        break;
      }
      case 'J': { // ED, DECSED
        setInEscape(false);

        ED_escape_.num = num;
        ED_escape_.nn  = nn;

        handleEscape(&ED_escape_);
        break;
      }
      case 'K': { // EL, DECSEL
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
      case 'f': { // HVP
        setInEscape(false);

        HVP_escape_.num = num;
        HVP_escape_.nn  = nn;

        handleEscape(&HVP_escape_);
        break;
      }
      case 'g': { // TBC
        setInEscape(false);

        TBC_escape_.num = num;
        TBC_escape_.nn  = nn;

        handleEscape(&TBC_escape_);
        break;
      }
      case 'h': { // SM
        setInEscape(false);

        SM_escape_.num = num;
        SM_escape_.nn  = nn;

        handleEscape(&SM_escape_);
        break;
      }
      case 'i': { // MC
        setInEscape(false);

        MC_escape_.num = num;
        MC_escape_.nn  = nn;

        handleEscape(&MC_escape_);
        break;
      }
      case 'l': { // RM
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

  if (str[*pos] != ';')
    return false;

  ++(*pos);

  if (str[*pos] == '\0')
    return false;

  string str1;
  int    osc_len;

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

  string value;
  int    apc_len;

  value += str[(*pos)++];

  while (str[*pos] != '\0' && ! isAPCEscape(str, pos, &apc_len))
    value += str[(*pos)++];

  if (! isAPCEscape(str, pos, &apc_len))
    return false;

  (*pos) += apc_len;

  char c = value[0];

  switch (c) {
    case '<': {
      CStrParse parse(value);

      parse.skipChar();
      parse.skipSpace();

      string name;

      if (! parse.readIdentifier(name))
        name = "";

      typedef pair<string,string> NameValue;
      typedef vector<NameValue>   NameValueList;

      vector<NameValue> name_values;

      while (! parse.eof() && ! parse.isString("/>")) {
        parse.skipSpace();

        string opt_name;

        if (! parse.readIdentifier(opt_name))
          break;

        string opt_value;

        parse.skipSpace();

        if (parse.isChar('=')) {
          parse.skipChar();

          parse.skipSpace();

          if (! parse.readString(opt_value, /*strip_quotes*/true))
            opt_value = "";
        }

        name_values.push_back(NameValue(opt_name, opt_value));
      }

      NameValueList::const_iterator p1 = name_values.begin();
      NameValueList::const_iterator p2 = name_values.end  ();

      // <state dir="dirname"/>
      if      (name == "state") {
        for ( ; p1 != p2; ++p1) {
          const string &opt_name  = (*p1).first;
          const string &opt_value = (*p1).second;

          if (opt_name == "dir") {
            setInEscape(false);

            set_dir_escape_.str = opt_value;

            handleEscape(&set_dir_escape_);
          }
        }
      }

      // <image filename="name" size="size" x1="x1" y1="y1" x2="x2" y2="y2"/>
      else if (name == "image") {
        string name;
        int    size = -1;
        int    x1   = -1, y1 = -1;
        int    x2   = -1, y2 = -1;

        for ( ; p1 != p2; ++p1) {
          const string &opt_name  = (*p1).first;
          const string &opt_value = (*p1).second;

          if      (opt_name == "filename")
            name = opt_value;
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
          CImageFile *image = lookupFileImage(name, size, size, true);

          sized_image_escape_.name  = name;
          sized_image_escape_.image = image;
          sized_image_escape_.x1    = x1;
          sized_image_escape_.y1    = y1;
          sized_image_escape_.x2    = x2;
          sized_image_escape_.y2    = y2;

          handleEscape(&sized_image_escape_);
        }
        else {
          CImageFile *image = lookupFileImage(name);

          image_escape_.name  = name;
          image_escape_.image = image;
          image_escape_.x1    = x1;
          image_escape_.y1    = y1;
          image_escape_.x2    = x2;
          image_escape_.y2    = y2;

          handleEscape(&image_escape_);
        }
      }

      // <pixel x="x" y="y" color="color"/>
      else if (name == "pixel") {
        int    x = 0, y = 0;
        string color("black");

        for ( ; p1 != p2; ++p1) {
          const string &opt_name  = (*p1).first;
          const string &opt_value = (*p1).second;

          if      (opt_name == "color")
            color = opt_value;
          else if (opt_name == "x")
            x = CStrUtil::toInteger(opt_value);
          else if (opt_name == "y")
            y = CStrUtil::toInteger(opt_value);
        }

        setInEscape(false);

        pixel_escape_.x     = x;
        pixel_escape_.y     = y;
        pixel_escape_.color = color;

        handleEscape(&pixel_escape_);
      }

      // <line x1="x1" y1="y1" x2="x2" y2="y2" color="color"/>
      else if (name == "line") {
        int    x1 = 0, y1 = 0, x2 = 0, y2 = 0;
        string color("black");

        for ( ; p1 != p2; ++p1) {
          const string &opt_name  = (*p1).first;
          const string &opt_value = (*p1).second;

          if      (opt_name == "color")
            color = opt_value;
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
        line_escape_.color = color;

        handleEscape(&line_escape_);
      }

      // <link type=<type> dir=<dir> name=<name>
      else if (name == "link") {
        string type = "text";
        string dir;
        string name;

        for ( ; p1 != p2; ++p1) {
          const string &opt_name  = (*p1).first;
          const string &opt_value = (*p1).second;

          if      (opt_name == "type")
            type = opt_value;
          else if (opt_name == "dir")
            dir  = opt_value;
          else if (opt_name == "name")
            name = opt_value;
        }

        setInEscape(false);

        link_escape_.name = name;
        link_escape_.path = dir;
        link_escape_.type = type;

        handleEscape(&link_escape_);
      }

      // <command name=<name> dir=<dir>

      else if (name == "command") {
        string name;
        string dir;
        bool   start;

        for ( ; p1 != p2; ++p1) {
          const string &opt_name  = (*p1).first;
          const string &opt_value = (*p1).second;

          if      (opt_name == "name")
            name = opt_value;
          else if (opt_name == "dir")
            dir  = opt_value;
          else if (opt_name == "start")
            start = CStrUtil::toBool(opt_value);
        }

        setInEscape(false);

        cmd_escape_.name  = name;
        cmd_escape_.path  = dir;
        cmd_escape_.start = start;

        handleEscape(&cmd_escape_);
      }

      // <preview path=<path>
      else if (name == "preview") {
        setInEscape(false);

        string path;

        for ( ; p1 != p2; ++p1) {
          const string &opt_name  = (*p1).first;
          const string &opt_value = (*p1).second;

          if (opt_name == "path")
            path = opt_value;
        }

        preview_files_escape_.str = path;

        handleEscape(&preview_files_escape_);
      }

      // <paste text=<text>
      else if (name == "paste") {
        setInEscape(false);

        string text;

        for ( ; p1 != p2; ++p1) {
          const string &opt_name  = (*p1).first;
          const string &opt_value = (*p1).second;

          if (opt_name == "text")
            text = opt_value;
        }

        paste_escape_.str = text;

        handleEscape(&paste_escape_);
      }

      // <trace flag=<bool>
      else if (name == "trace") {
        setInEscape(false);

        for ( ; p1 != p2; ++p1) {
          const string &opt_name  = (*p1).first;
          const string &opt_value = (*p1).second;

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

        for ( ; p1 != p2; ++p1) {
          const string &opt_name  = (*p1).first;
          const string &opt_value = (*p1).second;

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

  string value;
  int    dcs_len;

  value += str[(*pos)++];

  while (str[*pos] != '\0' && ! isDCSTerm(str, pos, &dcs_len))
    value += str[(*pos)++];

  if (! isDCSTerm(str, pos, &dcs_len))
    return false;

  (*pos) += dcs_len;

  uint value_len = value.size();

  if (value_len >= 2 && value[0] == '+' && value[1] == 'q') {
    string value1 = value.substr(2);

    value_len -= 2;

    string key;

    bool valid = true;

    uint pos = 0;

    while (pos + 2 <= value_len) {
      uint i1, i2;

      if (! CStrUtil::decodeHexChar(value1[pos    ], &i1) ||
          ! CStrUtil::decodeHexChar(value1[pos + 1], &i2)) {
        valid = false;
        break;
      }

      if (! valid) break;

      key += char(i1 << 4 | i2);

      pos += 2;
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
    case '$': {
      UNHANDLED1("DECRQSS", value)
      break;
    }
    case '+': {
      UNHANDLED2("DCS", c, value)
      break;
    }
    default: {
      UNHANDLED2("DCS", c, value)
      break;
    }
  }
#endif

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

  if (str[*pos] == '' && str[*pos + 1] == '\\') {
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

  if (str[*pos] == '' && str[*pos + 1] == '\\') {
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

  if (str[*pos] == '' && str[*pos + 1] == '\\') {
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

CImageFile *
CEscapeParse::
lookupFileImage(const string &filename)
{
  CImageFileSrc src(filename);

  return CImageMgrInst->lookupFile(src);
}

CImageFile *
CEscapeParse::
lookupFileImage(const string &filename, int width, int height, bool keep_aspect)
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
    case 0:
      style.reset();

      break;
    case 1:
      style.setBold(true);

      break;
    case 2:
      style.setDim(true);

      break;
    case 4:
      style.setUnderscore(true);

      break;
    case 5:
      style.setBlink(true);

      break;
    case 7:
      style.setInvert(true);

      break;
    case 8:
      style.setHidden(true);

      break;
    case 21:
      style.setBold(false);

      break;
    case 22:
      style.setDim(false);

      break;
    case 24:
      style.setUnderscore(false);

      break;
    case 25:
      style.setBlink(false);

      break;
    case 27:
      style.setInvert(false);

      break;
    case 28:
      style.setHidden(false);

      break;

    case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: {
      style.setFg((CEscapeColor) (num[0] - 30));

      break;
    }
    case 38:
      style.setFg(CRGB(num[1]/255.0, num[2]/255.0, num[3]/255.0));

      break;
    case 39:
      style.setFg(CESCAPE_COLOR_FG);

      break;
    case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47: {
      style.setBg((CEscapeColor) (num[0] - 40));

      break;
    }
    case 48:
      style.setBg(CRGB(num[1]/255.0, num[2]/255.0, num[3]/255.0));

      break;
    case 49:
      style.setBg(CESCAPE_COLOR_BG);

      break;
    case 90: case 91: case 92: case 93: case 94: case 95: case 96: case 97: {
      style.setFg((CEscapeColor) (num[0] - 90));

      break;
    }
    case 100: case 101: case 102: case 103: case 104: case 105: case 106: case 107: {
      style.setBg((CEscapeColor) (num[0] - 100));

      break;
    }
    default:
      return false;
  }

  return true;
}

//-----------

void
CEscapeDataC::
print(ostream &os) const
{
  os << typeToName(type) << ";" << c;
}

void
CEscapeDataBool::
print(ostream &os) const
{
  os << typeToName(type) << ";" << b;
}

void
CEscapeDataStr::
print(ostream &os) const
{
  os << typeToName(type) << ";" << str;
}

void
CEscapeDataDCS::
print(ostream &os) const
{
  os << typeToName(type) << ";" << CEvent::keyTypeName(key);
}

void
CEscapeDataNums::
print(ostream &os) const
{
  os << typeToName(type);

  for (int i = 0; i < nn; ++i)
    os << ";" << num[i];
}

void
CEscapeDataOSC::
print(ostream &os) const
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
CEscapeDataImage::
print(ostream &os) const
{
  os << typeToName(type) << ";" << name;
}

void
CEscapeDataPixel::
print(ostream &os) const
{
  os << typeToName(type) << ";" << color << ";" << x <<  ";" << y;
}

void
CEscapeDataLine::
print(ostream &os) const
{
  os << typeToName(type) << ";" << color << ";" << x1 <<  ";" << y1 << ";" << x2 <<  ";" << y2;
}

void
CEscapeDataLink::
print(ostream &os) const
{
  os << typeToName(CEscapeData::type) << ";" << name << ";" << path <<  ";" << type;
}

void
CEscapeDataCmd::
print(ostream &os) const
{
  os << typeToName(CEscapeData::type) << ";" << name << ";" << path;
}

void
CEscapeData::
print(ostream &os) const
{
  os << typeToName(type);
}

const char *
CEscapeData::
typeToName(CEscapeType type)
{
  switch (type) {
    case CESCAPE_TYPE_NONE:            return "none";
    case CESCAPE_TYPE_NUL:             return "nul";
    case CESCAPE_TYPE_SOH:             return "soh";
    case CESCAPE_TYPE_STX:             return "stx";
    case CESCAPE_TYPE_ETX:             return "etx";
    case CESCAPE_TYPE_EOT:             return "eot";
    case CESCAPE_TYPE_ENQ:             return "enq";
    case CESCAPE_TYPE_ACK:             return "ack";
    case CESCAPE_TYPE_BEL:             return "bel";
    case CESCAPE_TYPE_BS:              return "bs";
    case CESCAPE_TYPE_HT:              return "ht";
    case CESCAPE_TYPE_LF:              return "lf";
    case CESCAPE_TYPE_VT:              return "vt";
    case CESCAPE_TYPE_FF:              return "ff";
    case CESCAPE_TYPE_CR:              return "cr";
    case CESCAPE_TYPE_SO:              return "so";
    case CESCAPE_TYPE_SI:              return "si";
    case CESCAPE_TYPE_DLE:             return "dle";
    case CESCAPE_TYPE_DC1:             return "dc1";
    case CESCAPE_TYPE_DC2:             return "dc2";
    case CESCAPE_TYPE_DC3:             return "dc3";
    case CESCAPE_TYPE_DC4:             return "dc4";
    case CESCAPE_TYPE_NAK:             return "nak";
    case CESCAPE_TYPE_SYN:             return "syn";
    case CESCAPE_TYPE_ETB:             return "etb";
    case CESCAPE_TYPE_CAN:             return "can";
    case CESCAPE_TYPE_EM:              return "em";
    case CESCAPE_TYPE_SUB:             return "sub";
    case CESCAPE_TYPE_FS:              return "fs";
    case CESCAPE_TYPE_GS:              return "gs";
    case CESCAPE_TYPE_RS:              return "rs";
    case CESCAPE_TYPE_US:              return "us";
    case CESCAPE_TYPE_DEL:             return "del";
    case CESCAPE_TYPE_DECPAM:          return "decpam";
    case CESCAPE_TYPE_DECPNM:          return "decpnm";
    case CESCAPE_TYPE_LS3R:            return "ls3r";
    case CESCAPE_TYPE_LS2R:            return "ls2r";
    case CESCAPE_TYPE_LS1R:            return "ls1r";
    case CESCAPE_TYPE_S7C1T:           return "s7c1t";
    case CESCAPE_TYPE_S8C1T:           return "s8c1t";
    case CESCAPE_TYPE_ANSI1:           return "ansi1";
    case CESCAPE_TYPE_ANSI2:           return "ansi2";
    case CESCAPE_TYPE_ANSI3:           return "ansi3";
    case CESCAPE_TYPE_DECDHLTop:       return "decdhltop";
    case CESCAPE_TYPE_DECDHLBottom:    return "decdhlbottom";
    case CESCAPE_TYPE_DECSWL:          return "decswl";
    case CESCAPE_TYPE_DECDWL:          return "decdwl";
    case CESCAPE_TYPE_DECALN:          return "decaln";
    case CESCAPE_TYPE_ISO8859_1:       return "iso8859_1";
    case CESCAPE_TYPE_UTF_8:           return "utf_8";
    case CESCAPE_TYPE_G0:              return "g0";
    case CESCAPE_TYPE_G1:              return "g1";
    case CESCAPE_TYPE_G2:              return "g2";
    case CESCAPE_TYPE_G3:              return "g3";
    case CESCAPE_TYPE_IND:             return "ind";
    case CESCAPE_TYPE_NEL:             return "nel";
    case CESCAPE_TYPE_ESA:             return "esa";
    case CESCAPE_TYPE_HTS:             return "hts";
    case CESCAPE_TYPE_RI:              return "ri";
    case CESCAPE_TYPE_SS2:             return "ss2";
    case CESCAPE_TYPE_SS3:             return "ss3";
    case CESCAPE_TYPE_CCH:             return "cch";
    case CESCAPE_TYPE_SPA:             return "spa";
    case CESCAPE_TYPE_EPA:             return "epa";
    case CESCAPE_TYPE_SOS:             return "sos";
    case CESCAPE_TYPE_DECID:           return "decid";
    case CESCAPE_TYPE_RIS:             return "ris";
    case CESCAPE_TYPE_MemoryLock:      return "memorylock";
    case CESCAPE_TYPE_MemoryUnlock:    return "memoryunlock";
    case CESCAPE_TYPE_LS2:             return "ls2";
    case CESCAPE_TYPE_LS3:             return "ls3";
    case CESCAPE_TYPE_DECSC:           return "decsc";
    case CESCAPE_TYPE_DECRC:           return "decrc";
    case CESCAPE_TYPE_DECSED:          return "decsed";
    case CESCAPE_TYPE_DECSEL:          return "decsel";
    case CESCAPE_TYPE_DA2:             return "da2";
    case CESCAPE_TYPE_DECSET:          return "decset";
    case CESCAPE_TYPE_DECMC:           return "decmc";
    case CESCAPE_TYPE_DECRST:          return "decrst";
    case CESCAPE_TYPE_DECDSR:          return "decdsr";
    case CESCAPE_TYPE_DECSTR:          return "decstr";
    case CESCAPE_TYPE_DECRestorePriv:  return "decrestorepriv";
    case CESCAPE_TYPE_DECSavePriv:     return "decsavepriv";
    case CESCAPE_TYPE_HPA:             return "hpa";
    case CESCAPE_TYPE_DECSCL:          return "decscl";
    case CESCAPE_TYPE_DECSCA:          return "decsca";
    case CESCAPE_TYPE_DECCARA:         return "deccara";
    case CESCAPE_TYPE_DECRARA:         return "decrara";
    case CESCAPE_TYPE_DECCRA:          return "deccra";
    case CESCAPE_TYPE_DECEFR:          return "decefr";
    case CESCAPE_TYPE_DECLRP:          return "declrp";
    case CESCAPE_TYPE_DECFRA:          return "decfra";
    case CESCAPE_TYPE_DECSACE:         return "decsace";
    case CESCAPE_TYPE_DECELR:          return "decelr";
    case CESCAPE_TYPE_DECERA:          return "decera";
    case CESCAPE_TYPE_DECSLE:          return "decsle";
    case CESCAPE_TYPE_DECSERA:         return "decsera";
    case CESCAPE_TYPE_DECRQLP:         return "decrqlp";
    case CESCAPE_TYPE_ICH:             return "ich";
    case CESCAPE_TYPE_CUU:             return "cuu";
    case CESCAPE_TYPE_CUD:             return "cud";
    case CESCAPE_TYPE_CUF:             return "cuf";
    case CESCAPE_TYPE_CUB:             return "cub";
    case CESCAPE_TYPE_CNL:             return "cnl";
    case CESCAPE_TYPE_CPL:             return "cpl";
    case CESCAPE_TYPE_CHA:             return "cha";
    case CESCAPE_TYPE_CUP:             return "cup";
    case CESCAPE_TYPE_CHT:             return "cht";
    case CESCAPE_TYPE_ED:              return "ed";
    case CESCAPE_TYPE_EL:              return "el";
    case CESCAPE_TYPE_IL:              return "il";
    case CESCAPE_TYPE_DL:              return "dl";
    case CESCAPE_TYPE_DCH:             return "dch";
    case CESCAPE_TYPE_SU:              return "su";
    case CESCAPE_TYPE_SD:              return "sd";
    case CESCAPE_TYPE_StartMouseTrack: return "startmousetrack";
    case CESCAPE_TYPE_ECH:             return "ech";
    case CESCAPE_TYPE_CBT:             return "cbt";
    case CESCAPE_TYPE_REP:             return "rep";
    case CESCAPE_TYPE_DA1:             return "da1";
    case CESCAPE_TYPE_VPA:             return "vpa";
    case CESCAPE_TYPE_HVP:             return "hvp";
    case CESCAPE_TYPE_TBC:             return "tbc";
    case CESCAPE_TYPE_SM:              return "sm";
    case CESCAPE_TYPE_MC:              return "mc";
    case CESCAPE_TYPE_RM:              return "rm";
    case CESCAPE_TYPE_SGR:             return "sgr";
    case CESCAPE_TYPE_DSR:             return "dsr";
    case CESCAPE_TYPE_DECLL:           return "decll";
    case CESCAPE_TYPE_DECSTBM:         return "decstbm";
    case CESCAPE_TYPE_SC:              return "sc";
    case CESCAPE_TYPE_WindowManip:     return "windowmanip";
    case CESCAPE_TYPE_DECREQTPARM:     return "decreqtparm";
    case CESCAPE_TYPE_DECTST:          return "dectst";
    case CESCAPE_TYPE_OSC:             return "osc";
    case CESCAPE_TYPE_SET_DIR:         return "set_dir";
    case CESCAPE_TYPE_SIZED_IMAGE:     return "sized_image";
    case CESCAPE_TYPE_IMAGE:           return "image";
    case CESCAPE_TYPE_PIXEL:           return "pixel";
    case CESCAPE_TYPE_LINE:            return "line";
    case CESCAPE_TYPE_LINK:            return "link";
    case CESCAPE_TYPE_COMMAND:         return "command";
    case CESCAPE_TYPE_PREVIEW_FILES:   return "preview_files";
    case CESCAPE_TYPE_PASTE:           return "paste";
    case CESCAPE_TYPE_TRACE:           return "trace";
    case CESCAPE_TYPE_DEBUG:           return "debug";
    case CESCAPE_TYPE_DCS:             return "dcs";
    default:                           return "???";
  }
}
