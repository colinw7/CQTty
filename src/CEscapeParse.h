#ifndef CTERM_PARSE_H
#define CTERM_PARSE_H

#include <CCharBuf.h>
#include <CImageLib.h>
#include <CEvent.h>
#include <CCellStyle.h>

class CImageFile;

enum class CEscapeType {
  NONE,
  NUL,
  SOH,
  STX,
  ETX,
  EOT,
  ENQ,
  ACK,
  BEL,
  BS,
  HT,
  LF,
  VT,
  FF,
  CR,
  SO,
  SI,
  DLE,
  DC1,
  DC2,
  DC3,
  DC4,
  NAK,
  SYN,
  ETB,
  CAN,
  EM,
  SUB,
  FS,
  GS,
  RS,
  US,
  DEL,
  DECPAM,
  DECPNM,
  LS3R,
  LS2R,
  LS1R,
  S7C1T,
  S8C1T,
  ANSI1,
  ANSI2,
  ANSI3,
  DECDHL,
  DECSWL,
  DECDWL,
  DECALN,
  ISO8859_1,
  UTF_8,
  G0,
  G1,
  G2,
  G3,
  IND,
  NEL,
  SSA,
  ESA,
  HTS,
  RI,
  SS2,
  SS3,
  CCH,
  SPA,
  EPA,
  SOS,
  DECID,
  RIS,
  MemoryLock,
  MemoryUnlock,
  LS2,
  LS3,
  DECSC,
  DECRC,
  DECSED,
  DECSEL,
  DA2,
  DECSET,
  DECMC,
  DECRST,
  DECDSR,
  DECSTR,
  DECRestorePriv,
  DECSavePriv,
  HPA,
  DECSCL,
  DECSCA,
  DECCARA,
  DECRARA,
  DECCRA,
  DECEFR,
  DECLRP,
  DECFRA,
  DECSACE,
  DECELR,
  DECERA,
  DECSLE,
  DECSERA,
  DECRQLP,
  ICH,
  CUU,
  CUD,
  CUF,
  CUB,
  CNL,
  CPL,
  CHA,
  CUP,
  CHT,
  ED,
  EL,
  IL,
  DL,
  DCH,
  SU,
  SD,
  StartMouseTrack,
  ECH,
  CBT,
  REP,
  DA1,
  VPA,
  HVP,
  TBC,
  SM,
  MC,
  RM,
  SGR,
  DSR,
  DECLL,
  DECSTBM,
  SC,
  WindowManip,
  DECREQTPARM,
  DECTST,
  OSC,
  TEK4014,
  SET_DIR,
  SIZED_IMAGE,
  FILE_IMAGE,
  IMAGE,
  PIXEL,
  LINE,
  LINK,
  COMMAND,
  PREVIEW_FILES,
  PASTE,
  TRACE,
  DEBUG,
  DCS,
  END_
};

// base class for escape data
struct CEscapeData {
  CEscapeType type { CEscapeType::NONE };

  CEscapeData(CEscapeType type1=CEscapeType::NONE) :
   type(type1) {
  }

  virtual ~CEscapeData() { }

  virtual void print(std::ostream &os) const;

  static const char *typeToName(CEscapeType type);
};

//---

// character
struct CEscapeDataC : public CEscapeData {
  char c { '\0' };

  CEscapeDataC(CEscapeType type1) :
   CEscapeData(type1) {
  }

  void print(std::ostream &os) const override;
};

//---

// bool
struct CEscapeDataBool : public CEscapeData {
  bool b { false };

  CEscapeDataBool(CEscapeType type1) :
   CEscapeData(type1) {
  }

  void print(std::ostream &os) const override;
};

//---

// string
struct CEscapeDataStr : public CEscapeData {
  std::string str;

  CEscapeDataStr(CEscapeType type1) :
   CEscapeData(type1) {
  }

  void print(std::ostream &os) const override;
};

//---

struct CEscapeDataGS : public CEscapeData {
  enum class Mode {
    NONE,
    MOVE_TO,
    LINE_TO
  };

  int  x    { 0 };
  int  y    { 0 };
  Mode mode { Mode::NONE };

  CEscapeDataGS(CEscapeType type1) :
   CEscapeData(type1) {
  }

  void print(std::ostream &os) const override;
};

//---

struct CEscapeDataDECDHL : public CEscapeData {
  enum Pos { TOP, BOTTOM };

  Pos pos { Pos::TOP };

  CEscapeDataDECDHL(CEscapeType type1) :
   CEscapeData(type1) {
  }

  void print(std::ostream &os) const override;
};

//---

// DCS key/value
struct CEscapeDataDCS : public CEscapeData {
  CKeyType    key { CKEY_TYPE_NUL };
  std::string value;

  CEscapeDataDCS(CEscapeType type1) :
   CEscapeData(type1) {
  }

  void print(std::ostream &os) const override;
};

//---

// array of integers
struct CEscapeDataNums : public CEscapeData {
  int *num { nullptr };
  int  nn  { 0 };

  CEscapeDataNums(CEscapeType type1) :
   CEscapeData(type1) {
  }

  void print(std::ostream &os) const override;
};

//---

// SGR
struct CEscapeDataSGR : public CEscapeDataNums {
  CEscapeDataSGR(CEscapeType type1) :
   CEscapeDataNums(type1) {
  }
};

//---

// OSC
struct CEscapeDataOSC : public CEscapeData {
  int         num { 0 };
  std::string str;

  CEscapeDataOSC() :
   CEscapeData(CEscapeType::OSC) {
  }

  void print(std::ostream &os) const override;
};

//---

// Tek 4014
struct CEscapeDataTek4014 : public CEscapeData {
  enum class Mode {
    NONE,
    VT100,
    STATUS,
    CLEAR,
    APL,
    COPY,
    BYPASS,
    GIN,
    POINT_PLOT,
    CHAR_SET,
    LINE_STYLE
  };

  enum class ZAxis {
    NORMAL,
    DEFOCUSED
  };

  enum class WriteMode {
    NORMAL,
    WRITETHRU
  };

  Mode             mode      { Mode::NONE };
  CEscapeLineStyle lineStyle { CEscapeLineStyle::SOLID };
  ZAxis            zAxis     { ZAxis::NORMAL };
  WriteMode        writeMode { WriteMode::NORMAL };
  int              value     { 0 };

  CEscapeDataTek4014() :
   CEscapeData(CEscapeType::TEK4014) {
  }

  void print(std::ostream &os) const override;
};

//---

// File Image
struct CEscapeDataFileImage : public CEscapeData {
  std::string  name;
  CImageFile * image { nullptr };
  int          x1 { 0 }, y1 { 0 }, x2 { 0 }, y2 { 0 };

  CEscapeDataFileImage(CEscapeType type1) :
   CEscapeData(type1) {
  }

  void print(std::ostream &os) const override;
};

//---

// Image
struct CEscapeDataImage : public CEscapeData {
  CImagePtr image;

  CEscapeDataImage(CEscapeType type1) :
   CEscapeData(type1) {
  }

  void print(std::ostream &os) const override;
};

//---

// Pixel, Color and Point
struct CEscapeDataPixel : public CEscapeData {
  CEscapeColor color { CEscapeColor::BLACK };
  int          x { 0 }, y { 0 };

  CEscapeDataPixel(CEscapeType type1) :
   CEscapeData(type1) {
  }

  void print(std::ostream &os) const override;
};

//---

// Line, Color and Start/End Points
struct CEscapeDataLine : public CEscapeData {
  CEscapeColor color { CEscapeColor::BLACK };
  int          x1 { 0 }, y1 { 0 };
  int          x2 { 1 }, y2 { 1 };

  CEscapeDataLine(CEscapeType type1) :
   CEscapeData(type1) {
  }

  void print(std::ostream &os) const override;
};

//---

// Link
struct CEscapeDataLink : public CEscapeData {
  std::string name;
  std::string path;
  std::string type;

  CEscapeDataLink() :
   CEscapeData(CEscapeType::LINK) {
  }

  void print(std::ostream &os) const override;
};

//---

// Command
struct CEscapeDataCmd : public CEscapeData {
  std::string name;
  std::string path;
  bool        start { false };

  CEscapeDataCmd() :
   CEscapeData(CEscapeType::COMMAND) {
  }

  void print(std::ostream &os) const override;
};

//---

// Interface for handling normal characters, graphic, characters and escape codes
class CEscapeParseIFace {
 public:
  virtual ~CEscapeParseIFace() { }

  // handle character data
  virtual void handleChar   (char c) = 0;
  virtual void handleGraphic(char c) = 0;
  virtual void handleEscape (const CEscapeData *esc) = 0;

  // log information
  virtual void log(const std::string &str) const = 0;

  // log information
  virtual void logError(const std::string &str) const = 0;

  // handle state
  virtual bool isVT52() const { return is_vt52_; }
  virtual void setVT52(bool b) { is_vt52_ = b; }

  virtual bool is4014() const { return is_4014_; }
  virtual void set4014(bool b) { is_4014_ = b; }

 protected:
  bool is_vt52_ { false };
  bool is_4014_ { false };
};

//---

// Regis
struct CEscapeRegisState {
  bool filled { false };
};

//---

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
  bool addControlChar(const char *str, uint *pos, uint len);
  void addEscapeChars(const char *str, uint len, uint *pos);
  void addEscapeChar(char c);
  void addGraphicChar(char c);

  bool isEscape(const char *str, uint *pos);
  bool is4014Escape(const char *str, uint *pos);
  bool isSpaceEscape(const char *str, uint *pos);
  bool isHashEscape(const char *str, uint *pos);
  bool isPercentEscape(const char *str, uint *pos);
  bool isLParenEscape(const char *str, uint *pos);
  bool isRParenEscape(const char *str, uint *pos);
  bool isAsteriskEscape(const char *str, uint *pos);
  bool isPlusEscape(const char *str, uint *pos);

  bool isAlphaEscape(const char *str, uint *pos);
  bool isVT52AlphaEscape(const char *str, uint *pos);

  bool isDigitEscape(const char *str, uint *pos);
  bool isCSIEscape(const char *str, uint *pos);
  bool isOSCEscape(const char *str, uint *pos);
  bool isAPCEscape(const char *str, uint *pos);
  bool isDCSEscape(const char *str, uint *pos);

  bool isOSCEscape(const char *str, uint *pos, int *len);
  bool isAPCEscape(const char *str, uint *pos, int *len);
  bool isDCSTerm(const char *str, uint *pos, int *len);

  bool parseRegisString(const std::string &str, CEscapeRegisState &regisState);

  bool parseNumList(const char *str, uint *pos, int **values, int *num_values);

  bool parseNumber(const char *str, uint *pos, int *num);

  bool parseDCSImageData(const char *str, uint *pos, int *data);

  CImageFile *lookupFileImage(const std::string &filename);
  CImageFile *lookupFileImage(const std::string &filename, int width,
                              int height, bool keep_aspect);

  bool updateStyleFromSGR(const CEscapeDataSGR *sgr, CCellStyle &style);

  // virtual interface
  void handleChar   (char c) override = 0;
  void handleGraphic(char c) override = 0;
  void handleEscape (const CEscapeData *esc) override = 0;

  void log(const std::string &str) const override = 0;

  void logError(const std::string &str) const override = 0;

 private:
  // private data
  bool     in_escape_ { false };
  CCharBuf escape_buf_;

  bool        in_apc_type_ { false };
  std::string apc_type_type_;
  std::string apc_type_value_;

  CEscapeData          NUL_escape_;
  CEscapeData          SOH_escape_;
  CEscapeData          STX_escape_;
  CEscapeData          ETX_escape_;
  CEscapeData          EOT_escape_;
  CEscapeData          ENQ_escape_;
  CEscapeData          ACK_escape_;
  CEscapeData          BEL_escape_;
  CEscapeData          BS_escape_;
  CEscapeData          HT_escape_;
  CEscapeData          LF_escape_;
  CEscapeData          VT_escape_;
  CEscapeData          FF_escape_;
  CEscapeData          CR_escape_;
  CEscapeData          SO_escape_;
  CEscapeData          SI_escape_;
  CEscapeData          DLE_escape_;
  CEscapeData          DC1_escape_;
  CEscapeData          DC2_escape_;
  CEscapeData          DC3_escape_;
  CEscapeData          DC4_escape_;
  CEscapeData          NAK_escape_;
  CEscapeData          SYN_escape_;
  CEscapeData          ETB_escape_;
  CEscapeData          CAN_escape_;
  CEscapeData          EM_escape_;
  CEscapeData          SUB_escape_;
  CEscapeData          FS_escape_;
  CEscapeDataGS        GS_escape_;
  CEscapeData          RS_escape_;
  CEscapeData          US_escape_;
  CEscapeData          DEL_escape_;
  CEscapeData          DECPAM_escape_;
  CEscapeData          DECPNM_escape_;
  CEscapeData          LS3R_escape_;
  CEscapeData          LS2R_escape_;
  CEscapeData          LS1R_escape_;
  CEscapeData          S7C1T_escape_;
  CEscapeData          S8C1T_escape_;
  CEscapeData          ANSI1_escape_;
  CEscapeData          ANSI2_escape_;
  CEscapeData          ANSI3_escape_;
  CEscapeDataDECDHL    DECDHL_escape_;
  CEscapeData          DECSWL_escape_;
  CEscapeData          DECDWL_escape_;
  CEscapeData          DECALN_escape_;
  CEscapeData          ISO8859_1_escape_;
  CEscapeData          UTF_8_escape_;
  CEscapeDataC         G0_escape_;
  CEscapeDataC         G1_escape_;
  CEscapeDataC         G2_escape_;
  CEscapeDataC         G3_escape_;
  CEscapeData          IND_escape_;
  CEscapeData          NEL_escape_;
  CEscapeData          SSA_escape_;
  CEscapeData          ESA_escape_;
  CEscapeData          HTS_escape_;
  CEscapeData          RI_escape_;
  CEscapeData          SS2_escape_;
  CEscapeData          SS3_escape_;
  CEscapeData          CCH_escape_;
  CEscapeData          SPA_escape_;
  CEscapeData          EPA_escape_;
  CEscapeData          SOS_escape_;
  CEscapeData          DECID_escape_;
  CEscapeData          RIS_escape_;
  CEscapeData          MemoryLock_escape_;
  CEscapeData          MemoryUnlock_escape_;
  CEscapeData          LS2_escape_;
  CEscapeData          LS3_escape_;
  CEscapeData          DECSC_escape_;
  CEscapeData          DECRC_escape_;
  CEscapeDataNums      DECSED_escape_;
  CEscapeDataNums      DECSEL_escape_;
  CEscapeDataNums      DA2_escape_;
  CEscapeDataNums      DECSET_escape_;
  CEscapeDataNums      DECMC_escape_;
  CEscapeDataNums      DECRST_escape_;
  CEscapeDataNums      DECDSR_escape_;
  CEscapeData          DECSTR_escape_;
  CEscapeDataNums      DECRestorePriv_escape_;
  CEscapeDataNums      DECSavePriv_escape_;
  CEscapeDataNums      HPA_escape_;
  CEscapeDataNums      DECSCL_escape_;
  CEscapeDataNums      DECSCA_escape_;
  CEscapeDataNums      DECCARA_escape_;
  CEscapeDataNums      DECRARA_escape_;
  CEscapeDataNums      DECCRA_escape_;
  CEscapeDataNums      DECEFR_escape_;
  CEscapeDataNums      DECLRP_escape_;
  CEscapeDataNums      DECFRA_escape_;
  CEscapeDataNums      DECSACE_escape_;
  CEscapeDataNums      DECELR_escape_;
  CEscapeDataNums      DECERA_escape_;
  CEscapeDataNums      DECSLE_escape_;
  CEscapeDataNums      DECSERA_escape_;
  CEscapeDataNums      DECRQLP_escape_;
  CEscapeDataNums      ICH_escape_;
  CEscapeDataNums      CUU_escape_;
  CEscapeDataNums      CUD_escape_;
  CEscapeDataNums      CUF_escape_;
  CEscapeDataNums      CUB_escape_;
  CEscapeDataNums      CNL_escape_;
  CEscapeDataNums      CPL_escape_;
  CEscapeDataNums      CHA_escape_;
  CEscapeDataNums      CUP_escape_;
  CEscapeDataNums      CHT_escape_;
  CEscapeDataNums      ED_escape_;
  CEscapeDataNums      EL_escape_;
  CEscapeDataNums      IL_escape_;
  CEscapeDataNums      DL_escape_;
  CEscapeDataNums      DCH_escape_;
  CEscapeDataNums      SU_escape_;
  CEscapeDataNums      SD_escape_;
  CEscapeDataNums      StartMouseTrack_escape_;
  CEscapeDataNums      ECH_escape_;
  CEscapeDataNums      CBT_escape_;
  CEscapeDataNums      REP_escape_;
  CEscapeDataNums      DA1_escape_;
  CEscapeDataNums      VPA_escape_;
  CEscapeDataNums      HVP_escape_;
  CEscapeDataNums      TBC_escape_;
  CEscapeDataNums      SM_escape_;
  CEscapeDataNums      MC_escape_;
  CEscapeDataNums      RM_escape_;
  CEscapeDataSGR       SGR_escape_;
  CEscapeDataNums      DSR_escape_;
  CEscapeDataNums      DECLL_escape_;
  CEscapeDataNums      DECSTBM_escape_;
  CEscapeData          SC_escape_;
  CEscapeDataNums      WindowManip_escape_;
  CEscapeDataNums      DECREQTPARM_escape_;
  CEscapeDataNums      DECTST_escape_;
  CEscapeDataOSC       OSC_escape_;
  CEscapeDataTek4014   Tek4014_escape_;
  CEscapeDataStr       set_dir_escape_;
  CEscapeDataFileImage sized_image_escape_;
  CEscapeDataFileImage file_image_escape_;
  CEscapeDataImage     image_escape_;
  CEscapeDataPixel     pixel_escape_;
  CEscapeDataLine      line_escape_;
  CEscapeDataLink      link_escape_;
  CEscapeDataCmd       cmd_escape_;
  CEscapeDataStr       preview_files_escape_;
  CEscapeDataStr       paste_escape_;
  CEscapeDataBool      trace_escape_;
  CEscapeDataBool      debug_escape_;
  CEscapeDataDCS       DCS_escape_;
};

//------

class CEscapeDebugParse : public CEscapeParse {
 public:
  CEscapeDebugParse() :
   CEscapeParse() {
  }

  bool isLogging() const { return logging_; }
  void setLogging(bool b) { logging_ = b; }

  void handleChar(char c) override {
    if (nc_ == 0)
      std::cout << "@";

    std::cout << c;

    ++nc_;
  }

  void handleGraphic(char c) override {
    if (nc_ == 0)
      std::cout << "@";

    std::cout << c;

    ++nc_;
  }

  void handleEscape(const CEscapeData *esc) override {
    if (nc_ > 0) {
      std::cout << std::endl;

      nc_ = 0;
    }

    esc->print(std::cout);

    std::cout << std::endl;
  }

  void log(const std::string &str) const override {
    if (isLogging())
      std::cerr << str;
  }

  void logError(const std::string &str) const override {
    std::cerr << str;
  }

 private:
  bool logging_ { false };
  int  nc_      { 0 };
};

#endif
