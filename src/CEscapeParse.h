#ifndef CTERM_PARSE_H
#define CTERM_PARSE_H

#include <CCharBuf.h>
#include <CImageLib.h>
#include <CEvent.h>
#include <CCellStyle.h>
#include <CAutoPtr.h>

class CImageFile;

enum CEscapeType {
  CESCAPE_TYPE_NONE,
  CESCAPE_TYPE_NUL,
  CESCAPE_TYPE_SOH,
  CESCAPE_TYPE_STX,
  CESCAPE_TYPE_ETX,
  CESCAPE_TYPE_EOT,
  CESCAPE_TYPE_ENQ,
  CESCAPE_TYPE_ACK,
  CESCAPE_TYPE_BEL,
  CESCAPE_TYPE_BS,
  CESCAPE_TYPE_HT,
  CESCAPE_TYPE_LF,
  CESCAPE_TYPE_VT,
  CESCAPE_TYPE_FF,
  CESCAPE_TYPE_CR,
  CESCAPE_TYPE_SO,
  CESCAPE_TYPE_SI,
  CESCAPE_TYPE_DLE,
  CESCAPE_TYPE_DC1,
  CESCAPE_TYPE_DC2,
  CESCAPE_TYPE_DC3,
  CESCAPE_TYPE_DC4,
  CESCAPE_TYPE_NAK,
  CESCAPE_TYPE_SYN,
  CESCAPE_TYPE_ETB,
  CESCAPE_TYPE_CAN,
  CESCAPE_TYPE_EM,
  CESCAPE_TYPE_SUB,
  CESCAPE_TYPE_FS,
  CESCAPE_TYPE_GS,
  CESCAPE_TYPE_RS,
  CESCAPE_TYPE_US,
  CESCAPE_TYPE_DEL,
  CESCAPE_TYPE_DECPAM,
  CESCAPE_TYPE_DECPNM,
  CESCAPE_TYPE_LS3R,
  CESCAPE_TYPE_LS2R,
  CESCAPE_TYPE_LS1R,
  CESCAPE_TYPE_S7C1T,
  CESCAPE_TYPE_S8C1T,
  CESCAPE_TYPE_ANSI1,
  CESCAPE_TYPE_ANSI2,
  CESCAPE_TYPE_ANSI3,
  CESCAPE_TYPE_DECDHLTop,
  CESCAPE_TYPE_DECDHLBottom,
  CESCAPE_TYPE_DECSWL,
  CESCAPE_TYPE_DECDWL,
  CESCAPE_TYPE_DECALN,
  CESCAPE_TYPE_ISO8859_1,
  CESCAPE_TYPE_UTF_8,
  CESCAPE_TYPE_G0,
  CESCAPE_TYPE_G1,
  CESCAPE_TYPE_G2,
  CESCAPE_TYPE_G3,
  CESCAPE_TYPE_IND,
  CESCAPE_TYPE_NEL,
  CESCAPE_TYPE_ESA,
  CESCAPE_TYPE_HTS,
  CESCAPE_TYPE_RI,
  CESCAPE_TYPE_SS2,
  CESCAPE_TYPE_SS3,
  CESCAPE_TYPE_CCH,
  CESCAPE_TYPE_SPA,
  CESCAPE_TYPE_EPA,
  CESCAPE_TYPE_SOS,
  CESCAPE_TYPE_DECID,
  CESCAPE_TYPE_RIS,
  CESCAPE_TYPE_MemoryLock,
  CESCAPE_TYPE_MemoryUnlock,
  CESCAPE_TYPE_LS2,
  CESCAPE_TYPE_LS3,
  CESCAPE_TYPE_DECSC,
  CESCAPE_TYPE_DECRC,
  CESCAPE_TYPE_DECSED,
  CESCAPE_TYPE_DECSEL,
  CESCAPE_TYPE_DA2,
  CESCAPE_TYPE_DECSET,
  CESCAPE_TYPE_DECMC,
  CESCAPE_TYPE_DECRST,
  CESCAPE_TYPE_DECDSR,
  CESCAPE_TYPE_DECSTR,
  CESCAPE_TYPE_DECRestorePriv,
  CESCAPE_TYPE_DECSavePriv,
  CESCAPE_TYPE_HPA,
  CESCAPE_TYPE_DECSCL,
  CESCAPE_TYPE_DECSCA,
  CESCAPE_TYPE_DECCARA,
  CESCAPE_TYPE_DECRARA,
  CESCAPE_TYPE_DECCRA,
  CESCAPE_TYPE_DECEFR,
  CESCAPE_TYPE_DECLRP,
  CESCAPE_TYPE_DECFRA,
  CESCAPE_TYPE_DECSACE,
  CESCAPE_TYPE_DECELR,
  CESCAPE_TYPE_DECERA,
  CESCAPE_TYPE_DECSLE,
  CESCAPE_TYPE_DECSERA,
  CESCAPE_TYPE_DECRQLP,
  CESCAPE_TYPE_ICH,
  CESCAPE_TYPE_CUU,
  CESCAPE_TYPE_CUD,
  CESCAPE_TYPE_CUF,
  CESCAPE_TYPE_CUB,
  CESCAPE_TYPE_CNL,
  CESCAPE_TYPE_CPL,
  CESCAPE_TYPE_CHA,
  CESCAPE_TYPE_CUP,
  CESCAPE_TYPE_CHT,
  CESCAPE_TYPE_ED,
  CESCAPE_TYPE_EL,
  CESCAPE_TYPE_IL,
  CESCAPE_TYPE_DL,
  CESCAPE_TYPE_DCH,
  CESCAPE_TYPE_SU,
  CESCAPE_TYPE_SD,
  CESCAPE_TYPE_StartMouseTrack,
  CESCAPE_TYPE_ECH,
  CESCAPE_TYPE_CBT,
  CESCAPE_TYPE_REP,
  CESCAPE_TYPE_DA1,
  CESCAPE_TYPE_VPA,
  CESCAPE_TYPE_HVP,
  CESCAPE_TYPE_TBC,
  CESCAPE_TYPE_SM,
  CESCAPE_TYPE_MC,
  CESCAPE_TYPE_RM,
  CESCAPE_TYPE_SGR,
  CESCAPE_TYPE_DSR,
  CESCAPE_TYPE_DECLL,
  CESCAPE_TYPE_DECSTBM,
  CESCAPE_TYPE_SC,
  CESCAPE_TYPE_WindowManip,
  CESCAPE_TYPE_DECREQTPARM,
  CESCAPE_TYPE_DECTST,
  CESCAPE_TYPE_OSC,
  CESCAPE_TYPE_SET_DIR,
  CESCAPE_TYPE_SIZED_IMAGE,
  CESCAPE_TYPE_IMAGE,
  CESCAPE_TYPE_PIXEL,
  CESCAPE_TYPE_LINE,
  CESCAPE_TYPE_LINK,
  CESCAPE_TYPE_COMMAND,
  CESCAPE_TYPE_PREVIEW_FILES,
  CESCAPE_TYPE_PASTE,
  CESCAPE_TYPE_TRACE,
  CESCAPE_TYPE_DEBUG,
  CESCAPE_TYPE_DCS,
  CESCAPE_TYPE_END_
};

struct CEscapeData {
  CEscapeType type;

  CEscapeData(CEscapeType type1=CESCAPE_TYPE_NONE) :
   type(type1) {
  }

  virtual ~CEscapeData() { }

  virtual void print(std::ostream &os) const;

  static const char *typeToName(CEscapeType type);
};

struct CEscapeDataC : public CEscapeData {
  char c;

  CEscapeDataC(CEscapeType type) :
   CEscapeData(type), c('\0') {
  }

  void print(std::ostream &os) const;
};

struct CEscapeDataBool : public CEscapeData {
  bool b;

  CEscapeDataBool(CEscapeType type) :
   CEscapeData(type), b(false) {
  }

  void print(std::ostream &os) const;
};

struct CEscapeDataStr : public CEscapeData {
  std::string str;

  CEscapeDataStr(CEscapeType type) :
   CEscapeData(type) {
  }

  void print(std::ostream &os) const;
};

struct CEscapeDataDCS : public CEscapeData {
  CKeyType    key;
  std::string value;

  CEscapeDataDCS(CEscapeType type) :
   CEscapeData(type) {
  }

  void print(std::ostream &os) const;
};

struct CEscapeDataNums : public CEscapeData {
  int *num;
  int  nn;

  CEscapeDataNums(CEscapeType type) :
   CEscapeData(type), num(NULL), nn(0) {
  }

  void print(std::ostream &os) const;
};

struct CEscapeDataSGR : public CEscapeDataNums {
  CEscapeDataSGR(CEscapeType type) :
   CEscapeDataNums(type) {
  }
};

struct CEscapeDataOSC : public CEscapeData {
  int         num;
  std::string str;

  CEscapeDataOSC() :
   CEscapeData(CESCAPE_TYPE_OSC), num(0) {
  }

  void print(std::ostream &os) const;
};

struct CEscapeDataImage : public CEscapeData {
  std::string  name;
  CImageFile * image;
  int          x1, y1, x2, y2;

  CEscapeDataImage(CEscapeType type) :
   CEscapeData(type), image(NULL), x1(0), y1(0), x2(0), y2(0) {
  }

  void print(std::ostream &os) const;
};

struct CEscapeDataPixel : public CEscapeData {
  std::string color;
  int         x, y;

  CEscapeDataPixel(CEscapeType type) :
   CEscapeData(type), color("black"), x(0), y(0) {
  }

  void print(std::ostream &os) const;
};

struct CEscapeDataLine : public CEscapeData {
  std::string color;
  int         x1, y1;
  int         x2, y2;

  CEscapeDataLine(CEscapeType type) :
   CEscapeData(type), color("black"), x1(0), y1(0), x2(1), y2(1) {
  }

  void print(std::ostream &os) const;
};

struct CEscapeDataLink : public CEscapeData {
  std::string name;
  std::string path;
  std::string type;

  CEscapeDataLink() :
   CEscapeData(CESCAPE_TYPE_LINK) {
  }

  void print(std::ostream &os) const;
};

struct CEscapeDataCmd : public CEscapeData {
  std::string name;
  std::string path;
  bool        start;

  CEscapeDataCmd() :
   CEscapeData(CESCAPE_TYPE_COMMAND), start(false) {
  }

  void print(std::ostream &os) const;
};

class CEscapeParseIFace {
  // virtual interface
  virtual void handleChar   (char c) = 0;
  virtual void handleGraphic(char c) = 0;
  virtual void handleEscape (const CEscapeData *esc) = 0;

  virtual void log(const std::string &str) const = 0;
};

// class to handle characters with embedded escape codes and translate then into :
//  . normal printable characters
//  . graphic characters
//  . escape codes
class CEscapeParse : public CEscapeParseIFace {
 public:
  CEscapeParse();

  virtual ~CEscapeParse();

  bool getInEscape() const { return in_escape_; }

  void setInEscape(bool in_escape);

  void reset();

  bool        getInAPCType   () const { return in_apc_type_; }
  std::string getAPCTypeType () const { return apc_type_type_; }
  std::string getAPCTypeValue() const { return apc_type_value_; }

  void addOutputChar(char c);
  void addOutputChars(const std::string &str);
  void addOutputChars(const char *str, uint len);

  void addNormalChar(char c);
  void addControlChar(char c);
  void addEscapeChars(const char *str, uint len, uint *pos);
  void addEscapeChar(char c);
  void addGraphicChar(char c);

  bool isEscape(const char *str, uint *pos);
  bool isSpaceEscape(const char *str, uint *pos);
  bool isHashEscape(const char *str, uint *pos);
  bool isPercentEscape(const char *str, uint *pos);
  bool isLParenEscape(const char *str, uint *pos);
  bool isRParenEscape(const char *str, uint *pos);
  bool isAsteriskEscape(const char *str, uint *pos);
  bool isPlusEscape(const char *str, uint *pos);
  bool isAlphaEscape(const char *str, uint *pos);
  bool isDigitEscape(const char *str, uint *pos);
  bool isCSIEscape(const char *str, uint *pos);
  bool isOSCEscape(const char *str, uint *pos);
  bool isAPCEscape(const char *str, uint *pos);
  bool isDCSEscape(const char *str, uint *pos);

  bool isOSCEscape(const char *str, uint *pos, int *len);
  bool isAPCEscape(const char *str, uint *pos, int *len);
  bool isDCSTerm(const char *str, uint *pos, int *len);

  bool parseNumList(const char *str, uint *pos, int **values, int *num_values);

  CImageFile *lookupFileImage(const std::string &filename);
  CImageFile *lookupFileImage(const std::string &filename, int width,
                              int height, bool keep_aspect);

  bool updateStyleFromSGR(const CEscapeDataSGR *sgr, CCellStyle &style);

  // virtual interface
  virtual void handleChar   (char c) = 0;
  virtual void handleGraphic(char c) = 0;
  virtual void handleEscape (const CEscapeData *esc) = 0;

  virtual void log(const std::string &str) const = 0;

 private:
  // private data
  bool     in_escape_;
  CCharBuf escape_buf_;

  bool        in_apc_type_;
  std::string apc_type_type_;
  std::string apc_type_value_;

  CEscapeData      NUL_escape_;
  CEscapeData      SOH_escape_;
  CEscapeData      STX_escape_;
  CEscapeData      ETX_escape_;
  CEscapeData      EOT_escape_;
  CEscapeData      ENQ_escape_;
  CEscapeData      ACK_escape_;
  CEscapeData      BEL_escape_;
  CEscapeData      BS_escape_;
  CEscapeData      HT_escape_;
  CEscapeData      LF_escape_;
  CEscapeData      VT_escape_;
  CEscapeData      FF_escape_;
  CEscapeData      CR_escape_;
  CEscapeData      SO_escape_;
  CEscapeData      SI_escape_;
  CEscapeData      DLE_escape_;
  CEscapeData      DC1_escape_;
  CEscapeData      DC2_escape_;
  CEscapeData      DC3_escape_;
  CEscapeData      DC4_escape_;
  CEscapeData      NAK_escape_;
  CEscapeData      SYN_escape_;
  CEscapeData      ETB_escape_;
  CEscapeData      CAN_escape_;
  CEscapeData      EM_escape_;
  CEscapeData      SUB_escape_;
  CEscapeData      FS_escape_;
  CEscapeData      GS_escape_;
  CEscapeData      RS_escape_;
  CEscapeData      US_escape_;
  CEscapeData      DEL_escape_;
  CEscapeData      DECPAM_escape_;
  CEscapeData      DECPNM_escape_;
  CEscapeData      LS3R_escape_;
  CEscapeData      LS2R_escape_;
  CEscapeData      LS1R_escape_;
  CEscapeData      S7C1T_escape_;
  CEscapeData      S8C1T_escape_;
  CEscapeData      ANSI1_escape_;
  CEscapeData      ANSI2_escape_;
  CEscapeData      ANSI3_escape_;
  CEscapeData      DECDHLTop_escape_;
  CEscapeData      DECDHLBottom_escape_;
  CEscapeData      DECSWL_escape_;
  CEscapeData      DECDWL_escape_;
  CEscapeData      DECALN_escape_;
  CEscapeData      ISO8859_1_escape_;
  CEscapeData      UTF_8_escape_;
  CEscapeDataC     G0_escape_;
  CEscapeDataC     G1_escape_;
  CEscapeDataC     G2_escape_;
  CEscapeDataC     G3_escape_;
  CEscapeData      IND_escape_;
  CEscapeData      NEL_escape_;
  CEscapeData      ESA_escape_;
  CEscapeData      HTS_escape_;
  CEscapeData      RI_escape_;
  CEscapeData      SS2_escape_;
  CEscapeData      SS3_escape_;
  CEscapeData      CCH_escape_;
  CEscapeData      SPA_escape_;
  CEscapeData      EPA_escape_;
  CEscapeData      SOS_escape_;
  CEscapeData      DECID_escape_;
  CEscapeData      RIS_escape_;
  CEscapeData      MemoryLock_escape_;
  CEscapeData      MemoryUnlock_escape_;
  CEscapeData      LS2_escape_;
  CEscapeData      LS3_escape_;
  CEscapeData      DECSC_escape_;
  CEscapeData      DECRC_escape_;
  CEscapeDataNums  DECSED_escape_;
  CEscapeDataNums  DECSEL_escape_;
  CEscapeDataNums  DA2_escape_;
  CEscapeDataNums  DECSET_escape_;
  CEscapeDataNums  DECMC_escape_;
  CEscapeDataNums  DECRST_escape_;
  CEscapeDataNums  DECDSR_escape_;
  CEscapeData      DECSTR_escape_;
  CEscapeDataNums  DECRestorePriv_escape_;
  CEscapeDataNums  DECSavePriv_escape_;
  CEscapeDataNums  HPA_escape_;
  CEscapeDataNums  DECSCL_escape_;
  CEscapeDataNums  DECSCA_escape_;
  CEscapeDataNums  DECCARA_escape_;
  CEscapeDataNums  DECRARA_escape_;
  CEscapeDataNums  DECCRA_escape_;
  CEscapeDataNums  DECEFR_escape_;
  CEscapeDataNums  DECLRP_escape_;
  CEscapeDataNums  DECFRA_escape_;
  CEscapeDataNums  DECSACE_escape_;
  CEscapeDataNums  DECELR_escape_;
  CEscapeDataNums  DECERA_escape_;
  CEscapeDataNums  DECSLE_escape_;
  CEscapeDataNums  DECSERA_escape_;
  CEscapeDataNums  DECRQLP_escape_;
  CEscapeDataNums  ICH_escape_;
  CEscapeDataNums  CUU_escape_;
  CEscapeDataNums  CUD_escape_;
  CEscapeDataNums  CUF_escape_;
  CEscapeDataNums  CUB_escape_;
  CEscapeDataNums  CNL_escape_;
  CEscapeDataNums  CPL_escape_;
  CEscapeDataNums  CHA_escape_;
  CEscapeDataNums  CUP_escape_;
  CEscapeDataNums  CHT_escape_;
  CEscapeDataNums  ED_escape_;
  CEscapeDataNums  EL_escape_;
  CEscapeDataNums  IL_escape_;
  CEscapeDataNums  DL_escape_;
  CEscapeDataNums  DCH_escape_;
  CEscapeDataNums  SU_escape_;
  CEscapeDataNums  SD_escape_;
  CEscapeDataNums  StartMouseTrack_escape_;
  CEscapeDataNums  ECH_escape_;
  CEscapeDataNums  CBT_escape_;
  CEscapeDataNums  REP_escape_;
  CEscapeDataNums  DA1_escape_;
  CEscapeDataNums  VPA_escape_;
  CEscapeDataNums  HVP_escape_;
  CEscapeDataNums  TBC_escape_;
  CEscapeDataNums  SM_escape_;
  CEscapeDataNums  MC_escape_;
  CEscapeDataNums  RM_escape_;
  CEscapeDataSGR   SGR_escape_;
  CEscapeDataNums  DSR_escape_;
  CEscapeDataNums  DECLL_escape_;
  CEscapeDataNums  DECSTBM_escape_;
  CEscapeData      SC_escape_;
  CEscapeDataNums  WindowManip_escape_;
  CEscapeDataNums  DECREQTPARM_escape_;
  CEscapeDataNums  DECTST_escape_;
  CEscapeDataOSC   OSC_escape_;
  CEscapeDataStr   set_dir_escape_;
  CEscapeDataImage sized_image_escape_;
  CEscapeDataImage image_escape_;
  CEscapeDataPixel pixel_escape_;
  CEscapeDataLine  line_escape_;
  CEscapeDataLink  link_escape_;
  CEscapeDataCmd   cmd_escape_;
  CEscapeDataStr   preview_files_escape_;
  CEscapeDataStr   paste_escape_;
  CEscapeDataBool  trace_escape_;
  CEscapeDataBool  debug_escape_;
  CEscapeDataDCS   DCS_escape_;
};

#endif
