#include <CEscapeHandler.h>
#include <CCellStyle.h>
#include <CWindow.h>
#include <CRGBName.h>
#include <sstream>

using std::string;
using std::vector;
using std::ostringstream;

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

CEscapeHandler::
CEscapeHandler() :
 debug_(false), trace_(false)
{
  cursor_color_ = CRGBA(1,1,0);
}

void
CEscapeHandler::
handleChar(char c)
{
  char char_set = getCharSet(0);

  if (char_set == '0') {
    if      (c == 95           ) c  = 127;
    else if (c >  95 && c < 127) c -= 95;
  }

  logTrace(c);

  // if at last column then:
  //  wrap mode   : move to next line
  //  no wrap mode: move back and overwrite last char
  if (getDataPos().x >= int(getNumDataCols())) {
    if (getLineWrap())
      escapeNEL();
    else
      decOutputCol();
  }

  int x = getDataPos().x;
  int y = getDataPos().y;

  notifyChar(x, y, c);

  if (getInsertMode())
    shiftRight();

  if (getInAPCType())
    setLinkCell(y, x, c, getStyle(), getAPCTypeType(), getAPCTypeValue());
  else
    setCell(y, x, c, getStyle());

  incOutputCol();
}

void
CEscapeHandler::
handleGraphic(char c)
{
  logTrace(c);
}

void
CEscapeHandler::
handleEscape(const CEscapeData *esc)
{
  switch (esc->type) {
    case CESCAPE_TYPE_NUL:
      break;
    case CESCAPE_TYPE_SOH:
      UNHANDLED("SOH");
      break;
    case CESCAPE_TYPE_STX:
      UNHANDLED("STX");
      break;
    case CESCAPE_TYPE_ETX:
      UNHANDLED("ETX");
      break;
    case CESCAPE_TYPE_EOT:
      UNHANDLED("EOT");
      break;
    case CESCAPE_TYPE_ENQ:
      // terminal status (default is empty)
      // escapeDA1(NULL, 0);
      break;
    case CESCAPE_TYPE_ACK:
      UNHANDLED("ACK");
      break;
    case CESCAPE_TYPE_BEL: {
      beep();
      break;
    }
    case CESCAPE_TYPE_BS:
      decOutputCol();
      break;
    case CESCAPE_TYPE_HT:
      nextTab();
      break;
    case CESCAPE_TYPE_LF: {
      setDispCol(0);

      incOutputRow();

      notifyEnter();

      break;
    }
    case CESCAPE_TYPE_VT:
      incOutputRow();

      if (getLfNlMode())
        setDispCol(0);

      break;
    case CESCAPE_TYPE_FF: {
      if (getFfNpMode())
        clearScreen();
      else {
        incOutputRow();

        if (getLfNlMode())
          setDispCol(0);
      }

      break;
    }
    case CESCAPE_TYPE_CR: {
      setDispCol(0);

      break;
    }
    case CESCAPE_TYPE_SO:
      charset_.num = 1;
      break;
    case CESCAPE_TYPE_SI:
      charset_.num = 0;
      break;
    case CESCAPE_TYPE_DLE:
      break;
    case CESCAPE_TYPE_DC1:
      break;
    case CESCAPE_TYPE_DC2:
      break;
    case CESCAPE_TYPE_DC3:
      break;
    case CESCAPE_TYPE_DC4:
      break;
    case CESCAPE_TYPE_NAK:
      break;
    case CESCAPE_TYPE_SYN:
      break;
    case CESCAPE_TYPE_ETB:
      break;
    case CESCAPE_TYPE_CAN:
      // cancel current sequence
      UNHANDLED("CAN");
      break;
    case CESCAPE_TYPE_EM:
      break;
    case CESCAPE_TYPE_SUB:
      // cancel current sequence and substitute character ?
      UNHANDLED("SUB");
      break;
    case CESCAPE_TYPE_FS:
      break;
    case CESCAPE_TYPE_GS:
      break;
    case CESCAPE_TYPE_RS:
      break;
    case CESCAPE_TYPE_US:
      break;
    case CESCAPE_TYPE_DEL:
      break;

    case CESCAPE_TYPE_DECPAM:
      escapeDECPAM();

      break;
    case CESCAPE_TYPE_DECPNM:
      escapeDECPNM();

      break;
    case CESCAPE_TYPE_LS3R:
      escapeLS3R();

      break;
    case CESCAPE_TYPE_LS2R:
      escapeLS2R();

      break;
    case CESCAPE_TYPE_LS1R:
      escapeLS1R();

      break;
    case CESCAPE_TYPE_S7C1T:
      escapeS7C1T();

      break;
    case CESCAPE_TYPE_S8C1T:
      escapeS8C1T();

      break;
    case CESCAPE_TYPE_ANSI1:
      escapeANSIConformance(1);

      break;
    case CESCAPE_TYPE_ANSI2:
      escapeANSIConformance(2);

      break;
    case CESCAPE_TYPE_ANSI3:
      escapeANSIConformance(3);

      break;
    case CESCAPE_TYPE_DECDHLTop:
      escapeDECDHLTop();

      break;
    case CESCAPE_TYPE_DECDHLBottom:
      escapeDECDHLBottom();

      break;
    case CESCAPE_TYPE_DECSWL:
      escapeDECSWL();

      break;
    case CESCAPE_TYPE_DECDWL:
      escapeDECDWL();

      break;
    case CESCAPE_TYPE_DECALN:
      escapeDECALN();

      break;
    case CESCAPE_TYPE_ISO8859_1:
      escapeISO8859_1();

      break;
    case CESCAPE_TYPE_UTF_8:
      escapeUTF_8();

      break;
    case CESCAPE_TYPE_G0: {
      const CEscapeDataC *escc = dynamic_cast<const CEscapeDataC *>(esc);

      escapeDesignateG0(escc->c);

      break;
    }
    case CESCAPE_TYPE_G1: {
      const CEscapeDataC *escc = dynamic_cast<const CEscapeDataC *>(esc);

      escapeDesignateG1(escc->c);

      break;
    }
    case CESCAPE_TYPE_G2: {
      const CEscapeDataC *escc = dynamic_cast<const CEscapeDataC *>(esc);

      escapeDesignateG2(escc->c);

      break;
    }
    case CESCAPE_TYPE_G3: {
      const CEscapeDataC *escc = dynamic_cast<const CEscapeDataC *>(esc);

      escapeDesignateG3(escc->c);

      break;
    }
    case CESCAPE_TYPE_IND:
      escapeIND();

      break;
    case CESCAPE_TYPE_NEL:
      escapeNEL();

      break;
    case CESCAPE_TYPE_ESA:
      escapeESA();

      break;
    case CESCAPE_TYPE_HTS:
      escapeHTS();

      break;
    case CESCAPE_TYPE_RI:
      escapeRI();

      break;
    case CESCAPE_TYPE_SS2:
      escapeSS2();

      break;
    case CESCAPE_TYPE_SS3:
      escapeSS3();

      break;
    case CESCAPE_TYPE_CCH:
      escapeCCH();

      break;
    case CESCAPE_TYPE_SPA:
      escapeSPA();

      break;
    case CESCAPE_TYPE_EPA:
      escapeEPA();

      break;
    case CESCAPE_TYPE_SOS:
      escapeSOS();

      break;
    case CESCAPE_TYPE_DECID:
      escapeDECID();

      break;
    case CESCAPE_TYPE_RIS:
      escapeRIS();

      break;
    case CESCAPE_TYPE_MemoryLock:
      escapeMemoryLock();

      break;
    case CESCAPE_TYPE_MemoryUnlock:
      escapeMemoryUnlock();

      break;
    case CESCAPE_TYPE_LS2:
      escapeLS2();

      break;
    case CESCAPE_TYPE_LS3:
      escapeLS3();

      break;
    case CESCAPE_TYPE_DECSC:
      escapeDECSC();

      break;
    case CESCAPE_TYPE_DECRC:
      escapeDECRC();

      break;
    case CESCAPE_TYPE_DECSED: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSED(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DECSEL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSEL(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DA2: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDA2(escn->num, escn->nn);

      break;
    }
    case CESCAPE_TYPE_DECSET: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      for (int i = 0; i < escn->nn; ++i)
        escapeDECSET(escn->num[i]);

      break;
    }
    case CESCAPE_TYPE_DECMC: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECMC(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DECRST: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      for (int i = 0; i < escn->nn; ++i)
        escapeDECRST(escn->num[i]);

      break;
    }
    case CESCAPE_TYPE_DECDSR: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECDSR(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DECSTR: {
      escapeDECSTR();

      break;
    }
    case CESCAPE_TYPE_DECRestorePriv: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECRestorePriv(escn->num, escn->nn);

      break;
    }
    case CESCAPE_TYPE_DECSavePriv: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSavePriv(escn->num, escn->nn);

      break;
    }
    case CESCAPE_TYPE_HPA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeHPA(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DECSCL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSCL(escn->num[0], escn->num[1]);

      break;
    }
    case CESCAPE_TYPE_DECSCA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSCA(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DECCARA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECCARA(escn->num[0], escn->num[1], escn->num[2], escn->num[3], escn->num[4]);

      break;
    }
    case CESCAPE_TYPE_DECRARA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECRARA(escn->num[0], escn->num[1], escn->num[2], escn->num[3], escn->num[4]);

      break;
    }
    case CESCAPE_TYPE_DECCRA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECCRA(escn->num[0], escn->num[1], escn->num[2], escn->num[3],
                   escn->num[4], escn->num[5], escn->num[6], escn->num[7]);

      break;
    }
    case CESCAPE_TYPE_DECEFR: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECEFR(escn->num[0], escn->num[1], escn->num[2], escn->num[3]);

      break;
    }
    case CESCAPE_TYPE_DECLRP: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECLRP(escn->num[0], escn->num[1], escn->num[2], escn->num[3]);

      break;
    }
    case CESCAPE_TYPE_DECFRA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECFRA(escn->num[0], escn->num[1], escn->num[2], escn->num[3], escn->num[4]);

      break;
    }
    case CESCAPE_TYPE_DECSACE: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSACE(escn->num[0], escn->num[1], escn->num[2]);

      break;
    }
    case CESCAPE_TYPE_DECELR: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECELR(escn->num[0], escn->num[1]);

      break;
    }
    case CESCAPE_TYPE_DECERA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECERA(escn->num[0], escn->num[1], escn->num[2], escn->num[3]);

      break;
    }
    case CESCAPE_TYPE_DECSLE: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSLE(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DECSERA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSERA(escn->num[0], escn->num[1], escn->num[2], escn->num[3]);

      break;
    }
    case CESCAPE_TYPE_DECRQLP: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECRQLP(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_ICH: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeICH(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_CUU: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCUU(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_CUD: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCUD(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_CUF: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCUF(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_CUB: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCUB(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_CNL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCNL(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_CPL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCPL(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_CHA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCHA(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_CUP: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCUP(escn->num[0], escn->num[1]);

      break;
    }
    case CESCAPE_TYPE_CHT: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCHT(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_ED: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeED(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_EL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeEL(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_IL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeIL(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDL(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DCH: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDCH(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_SU: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeSU(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_SD: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeSD(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_StartMouseTrack: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeStartMouseTrack(escn->num[0], escn->num[1], escn->num[2],
                            escn->num[3], escn->num[4]);

      break;
    }
    case CESCAPE_TYPE_ECH: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeECH(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_CBT: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCBT(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_REP: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeREP(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DA1: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDA1(escn->num, escn->nn);

      break;
    }
    case CESCAPE_TYPE_VPA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeVPA(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_HVP: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeHVP(escn->num[0], escn->num[1]);

      break;
    }
    case CESCAPE_TYPE_TBC: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeTBC(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_SM: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeSM(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_MC: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeMC(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_RM: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeRM(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_SGR: {
      const CEscapeDataSGR *sgr = dynamic_cast<const CEscapeDataSGR *>(esc);

      escapeSGR(sgr);

      break;
    }
    case CESCAPE_TYPE_DSR: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDSR(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DECLL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECLL(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DECSTBM: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      if      (escn->nn <= 0)
        escapeDECSTBM(-1, -1);
      else if (escn->nn == 1)
        escapeDECSTBM(escn->num[0], -1);
      else
        escapeDECSTBM(escn->num[0], escn->num[1]);

      break;
    }
    case CESCAPE_TYPE_SC:
      escapeSC();

      break;
    case CESCAPE_TYPE_WindowManip: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeWindowManip(escn->num);

      break;
    }
    case CESCAPE_TYPE_DECREQTPARM: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECREQTPARM(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_DECTST: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECTST(escn->num[0]);

      break;
    }
    case CESCAPE_TYPE_OSC: {
      const CEscapeDataOSC *esco = dynamic_cast<const CEscapeDataOSC *>(esc);

      escapeOSC(esco->num, esco->str);

      break;
    }
    case CESCAPE_TYPE_SET_DIR: {
      const CEscapeDataStr *escs = dynamic_cast<const CEscapeDataStr *>(esc);

      setDirName(escs->str);

      break;
    }
    case CESCAPE_TYPE_SIZED_IMAGE: {
      const CEscapeDataImage *esci = dynamic_cast<const CEscapeDataImage *>(esc);

      if (esci->x1 > 0 && esci->y1 > 0 && esci->x2 > 0 && esci->y2 > 0) {
        addSizedImageChar(esci->name, esci->image, esci->x1, esci->y1, esci->x2, esci->y2);

        incOutputCol();
      }
      else
        addImage(esci->name, esci->image);

      break;
    }
    case CESCAPE_TYPE_IMAGE: {
      const CEscapeDataImage *esci = dynamic_cast<const CEscapeDataImage *>(esc);

      addImageChar(esci->name, esci->image, esci->x1, esci->y1, esci->x2, esci->y2);

      incOutputCol();

      break;
    }
    case CESCAPE_TYPE_PIXEL: {
      const CEscapeDataPixel *escp = dynamic_cast<const CEscapeDataPixel *>(esc);

      addPixel(escp->x, escp->y, escp->color);

      break;
    }
    case CESCAPE_TYPE_LINE: {
      const CEscapeDataLine *escl = dynamic_cast<const CEscapeDataLine *>(esc);

      addLine(escl->x1, escl->y1, escl->x2, escl->y2, escl->color);

      break;
    }
    case CESCAPE_TYPE_LINK: {
      const CEscapeDataLink *escl = dynamic_cast<const CEscapeDataLink *>(esc);

      addLink(escl->name, escl->path, escl->type);

      break;
    }
    case CESCAPE_TYPE_PREVIEW_FILES: {
      const CEscapeDataStr *escs = dynamic_cast<const CEscapeDataStr *>(esc);

      previewFile(escs->str);

      break;
    }
    case CESCAPE_TYPE_PASTE: {
      const CEscapeDataStr *escs = dynamic_cast<const CEscapeDataStr *>(esc);

      paste(escs->str);

      break;
    }
    case CESCAPE_TYPE_TRACE: {
      const CEscapeDataBool *escb = dynamic_cast<const CEscapeDataBool *>(esc);

      setTrace(escb->b);

      break;
    }
    case CESCAPE_TYPE_DEBUG: {
      const CEscapeDataBool *escb = dynamic_cast<const CEscapeDataBool *>(esc);

      setDebug(escb->b);

      break;
    }
    case CESCAPE_TYPE_DCS: {
      const CEscapeDataDCS *escd = dynamic_cast<const CEscapeDataDCS *>(esc);

      string kstr;

#if 0
      if (escd->key != CKEY_TYPE_NUL)
        kstr = CKeyEvent::getEscapeText(escd->key);
#endif

      string reply;

      if (kstr != "")
        reply = "P1+r" + escd->value + "=" + kstr + "\\";
      else
        reply = "P0+r" + escd->value + "\\";

      processString(reply.c_str());

      if (escd->key == CKEY_TYPE_NUL)
        UNHANDLED1("DCS", escd->value)

      break;
    }
    default: {
      assert(false);

      break;
    }
  }
}

void
CEscapeHandler::
log(const std::string &str) const
{
  logDebug(str);
}

// Insert N blank characters at the cursor position, with the character attributes
// set to normal. The cursor does not move and remains at the beginning of the inserted
// blank characters. A parameter of 0 or 1 inserts one blank character. Data on the line
// is shifted forward as in character insertion.
void
CEscapeHandler::
escapeICH(int num)
{
  logTrace("<ICH;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  for (int i = 0; i < num; ++i)
    shiftRight();

  CIPoint2D data_pos = getDataPos();

  for (int i = 0; i < num; ++i)
    handleChar(' ');

  setDataPos(data_pos);
}

// Moves cursor up N lines in same column.
// Cursor stops at top margin.
void
CEscapeHandler::
escapeCUU(int num)
{
  logTrace("<CUU;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  if (num > getDataPos().y)
    num = getDataPos().y;

  for (int i = 0; i < num; ++i)
    decOutputRow();
}

// Moves cursor down N lines in same column.
// Cursor stops at bottom margin.
void
CEscapeHandler::
escapeCUD(int num)
{
  logTrace("<CUD;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  if (getDataPos().y + num >= int(getNumDataRows()))
    num = getNumDataRows() - 1 - getDataPos().y;

  for (int i = 0; i < num; ++i)
    incOutputRow();
}

// Moves cursor right N columns.
// Cursor stops at right margin.
void
CEscapeHandler::
escapeCUF(int num)
{
  logTrace("<CUF;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  if (getDataPos().x + num >= int(getNumDataCols()))
    num = getNumDataCols() - 1 - getDataPos().x;

  for (int i = 0; i < num; ++i)
    incOutputCol();
}

// Moves cursor left N columns.
// Cursor stops at left margin.
void
CEscapeHandler::
escapeCUB(int num)
{
  logTrace("<CUB;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  int row, col;

  getDispPos(&row, &col);

  if (num > col) {
    if (getReverseWrap()) {
      uint rows, cols;

      getDispSize(&rows, &cols);

      while (num > col) {
        num -= col;

        if (row > 0) {
          --row;

          col = cols - 1;

          setDispPos(row, col);
        }
        else {
          setDispPos(row, 0);

          return;
        }
      }
    }
    else {
      setDispPos(row, 0);

      return;
    }
  }

  col -= num;

  setDispPos(row, col);
}

void
CEscapeHandler::
escapeCNL(int num)
{
  logTrace("<CNL;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  for (int i = 0; i < num; ++i)
    incOutputRow();

  setDispCol(0);
}

void
CEscapeHandler::
escapeCPL(int num)
{
  logTrace("<CPL;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  for (int i = 0; i < num; ++i)
    decOutputRow();

  setDispCol(0);
}

void
CEscapeHandler::
escapeCHA(int num)
{
  logTrace("<CHA;" + CStrUtil::toString(num) + ">");

  setDispCol(num - 1);
}

void
CEscapeHandler::
escapeHPA(int num)
{
  logTrace("<HPA;" + CStrUtil::toString(num) + ">");

  setDispCol(num - 1);
}

void
CEscapeHandler::
escapeHVP(int row, int col)
{
  logTrace("<HVP;" + CStrUtil::toString(row) + ";" + CStrUtil::toString(col) + ">");

  escapeDispPos(row, col);
}

// Moves cursor to specified 'row' and 'col'.
// If 'row' or 'col' are invalid or 0, the cursor moves to first line or column, respectively.
// Origin mode (DECOM) selects line numbering and ability to move cursor into margins.
// NOTE: CUP operates the same as the horizontal and vertical position (HVP) sequence.
void
CEscapeHandler::
escapeCUP(int row, int col)
{
  if (row < 1) row = 1;
  if (col < 1) col = 1;

  logTrace("<CUP;" + CStrUtil::toString(row) + ";" + CStrUtil::toString(col) + ">");

  escapeDispPos(row, col);
}

void
CEscapeHandler::
escapeDispPos(int row, int col)
{
  // if origin mode make sure it is in scroll region
  if (getOriginMode()) {
    int scroll_bottom = getScrollBottom();
    int scroll_top    = getScrollTop   ();

    row += scroll_top - 1;

    if (row < scroll_top   ) row = scroll_top;
    if (row > scroll_bottom) row = scroll_bottom;
  }

  uint rows, cols;

  getDispSize(&rows, &cols);

  row = std::min(std::max(row, 1), int(rows));
  col = std::min(std::max(col, 1), int(cols));

  setDispPos(row - 1, col - 1);
}

void
CEscapeHandler::
escapeCHT(int num)
{
  logTrace("<CHT;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  for (int i = 0; i < num; ++i)
    nextTab();
}

void
CEscapeHandler::
escapeDECSED(int num)
{
  logTrace("<DECSED;" + CStrUtil::toString(num) + ">");

  if      (num == 0)
    clearScreenBelow();
  else if (num == 1)
    clearScreenAbove();
  else if (num == 2)
    clearScreen();
  else if (num == 3)
    clearScreenSaved();
  else
    UNHANDLED1("DECSED", num)
}

void
CEscapeHandler::
escapeED(int num)
{
  logTrace("<ED;" + CStrUtil::toString(num) + ">");

  if      (num == 0)
    clearScreenBelow();
  else if (num == 1)
    clearScreenAbove();
  else if (num == 2)
    clearScreen();
  else if (num == 3)
    clearScreenSaved();
  else
    UNHANDLED1("ED", num)
}

void
CEscapeHandler::
escapeDECSEL(int type)
{
  logTrace("<DECSEL;" + CStrUtil::toString(type) + ">");

  if      (type == 0)
    eraseLineRight();
  else if (type == 1)
    eraseLineLeft ();
  else if (type == 2)
    eraseLineAll  ();
  else
    UNHANDLED1("DECSEL", type)
}

void
CEscapeHandler::
escapeEL(int type)
{
  logTrace("<EL;" + CStrUtil::toString(type) + ">");

  if      (type == 0)
    eraseLineRight();
  else if (type == 1)
    eraseLineLeft ();
  else if (type == 2)
    eraseLineAll  ();
  else
    UNHANDLED1("EL", type)
}

// Inserts N lines at line with cursor.
// Lines displayed below cursor move down. Lines moved past the bottom margin are lost.
// This sequence is ignored when cursor is outside scrolling region.
void
CEscapeHandler::
escapeIL(int num)
{
  logTrace("<IL;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  insertLine(num);
}

// Deletes N lines, starting at line with cursor.
// As lines are deleted, lines displayed below cursor move up.
// Lines added to bottom of screen have spaces with same character attributes
// as last line moved up.
// This sequence is ignored when cursor is outside scrolling region.
void
CEscapeHandler::
escapeDL(int num)
{
  logTrace("<DL;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  deleteLine(num);
}

// Deletes N characters, starting with the character at cursor position.
// When character is deleted, all characters to the right of cursor move left.
// This creates a space character at right margin.
void
CEscapeHandler::
escapeDCH(int num)
{
  logTrace("<DCH;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  for (int i = 0; i < num; ++i)
    shiftLeft();
}

void
CEscapeHandler::
escapeSU(int num)
{
  logTrace("<SU;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  for (int i = 0; i < num; ++i)
    scrollUp();
}

void
CEscapeHandler::
escapeSD(int num)
{
  logTrace("<SD;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  for (int i = 0; i < num; ++i)
    scrollDown();
}

void
CEscapeHandler::
escapeECH(int num)
{
  logTrace("<ECH;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  CIPoint2D data_pos = getDataPos();

  for (int i = 0; i < num; ++i) {
    resetCell();

    incOutputCol();
  }

  setDataPos(data_pos);
}

void
CEscapeHandler::
escapeCBT(int num)
{
  logTrace("<CBT;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  for (int i = 0; i < num; ++i)
    prevTab();
}

void
CEscapeHandler::
escapeVPA(int num)
{
  logTrace("<VPA;" + CStrUtil::toString(num) + ">");

  setDispRow(num - 1);
}

void
CEscapeHandler::
escapeREP(int num)
{
  logTrace("<REP;" + CStrUtil::toString(num) + ">");

  if (num <= 0) num = 1;

  if (getDataPos().x <= 0)
    return;

  for (int i = 0; i < num; ++i) {
    replicatePrevChar();

    incOutputCol();
  }
}

void
CEscapeHandler::
escapeTBC(int num)
{
  logTrace("<TBC;" + CStrUtil::toString(num) + ">");

  if      (num == 0)
    clearTab(getDataPos().x);
  else if (num == 3)
    clearTabAll();
  else
    UNHANDLED1("TBC", num)
}

void
CEscapeHandler::
escapeMC(int num)
{
  logTrace("<MC;" + CStrUtil::toString(num) + ">");

  if      (num == 0)
    printScreen();
  else if (num == 4)
    stopPrintLog();
  else if (num == 5)
    startPrintLog();
  else
    UNHANDLED1("MC", num)
}

void
CEscapeHandler::
escapeDECMC(int num)
{
  logTrace("<DECMC;" + CStrUtil::toString(num) + ">");

  if      (num == 0)
    printScreen();
  else if (num == 1)
    printLine();
  else if (num == 4)
    stopPrintLog();
  else if (num == 5)
    startPrintLog();
  else if (num == 10)
    printComposedScreen();
  else if (num == 11)
    printAllPages();
  else
    UNHANDLED1("DECMC", num)
}

void
CEscapeHandler::
escapeDECSET(int num)
{
  logTrace("<DECSET;" + CStrUtil::toString(num) + ">");

  switch (num) {
    case 1: {
      escapeDECCKM(true);
      break;
    }
    case 2: {
      escapeDECANM(true);
      break;
    }
    case 3: {
      escapeDECCOLM(true);
      break;
    }
    case 4: {
      escapeDECSCLM(true);
      break;
    }
    case 5: {
      escapeDECSCNM(true);
      break;
    }
    case 6: {
      escapeDECOM(true);
      break;
    }
    case 7: {
      escapeDECAWM(true);
      break;
    }
    case 8: {
      escapeDECARM(true);
      break;
    }
    case 9: {
      setSendMousePress(true);
      break;
    }
    case 10: {
      setShowToolBar(true);
      break;
    }
    case 12: {
      setCursorBlink(true);
      break;
    }
    case 18: {
      escapeDECPFF(true);
      break;
    }
    case 19: {
      escapeDECPEX(true);
      break;
    }
    case 25: {
      escapeDECTCEM(true);
      break;
    }
    case 30: {
      setShowScrollBar(true);
      break;
    }
    case 35: {
      // Enable shift fonts - rxvt
      break;
    }
    case 38: {
      escapeDECTEK(true);
      break;
    }
    case 40: {
      // Allow 80 <-> 132
      setAllow80To132(true);
      break;
    }
    case 41: {
      // enable more fix
      break;
    }
    case 42: {
      escapeDECNRCM(true);
      break;
    }
    case 44: {
      // enable margin bell
      break;
    }
    case 45: {
      setReverseWrap(true);
      break;
    }
    case 46: {
      // start logging
      break;
    }
    case 47: {
      // use alternate screen
      setAlternative(true);
      break;
    }
    case 66: {
      escapeDECNKM(true);
      break;
    }
    case 67: {
      escapeDECBKM(true);
      break;
    }
    case 1000: {
      setSendMousePress  (true);
      setSendMouseRelease(true);
      break;
    }
    case 1001: {
      // hilite mouse tracking
      break;
    }
    case 1002: {
      // use cell motion mouse tracking
      setSendMousePress  (true);
      setSendMouseRelease(true);
      break;
    }
    case 1003: {
      // use all motion mouse tracking
      break;
    }
    case 1010: {
      setScrollBottomOnTty(true);
      break;
    }
    case 1011: {
      setScrollBottomOnKey(true);
      break;
    }
    case 1035: {
      // enable special modifiers Alt/NumLock
      break;
    }
    case 1036: {
      // Send ESC for meta
      break;
    }
    case 1037: {
      // Delete is DEL
      break;
    }
    case 1047: {
      // use alternate screen
      setAlternative(true);
      break;
    }
    case 1048: {
      // save cursor
      saveCursor();
      break;
    }
    case 1049: {
      // save cursor and use alternative screen
      saveCursor();
      setAlternative(true);
      clearScreen();
      break;
    }
    case 1051: {
      // set sun fn key mode
      break;
    }
    case 1052: {
      // set hp fn key mode
      break;
    }
    case 1053: {
      // set sco fn key mode
      break;
    }
    case 1060: {
      // set legacy keyboard emulation
      break;
    }
    case 1061: {
      // set vt220 keyboard emulation
      break;
    }
    default: {
      UNHANDLED1("DECSET", num)
      break;
    }
  }
}

void
CEscapeHandler::
escapeSM(int num)
{
  logTrace("<SM;" + CStrUtil::toString(num) + ">");

  switch (num) {
    case 2: {
      escapeAM(true);
      break;
    }
    case 4: {
      escapeIRM(true);
      break;
    }
    case 12: {
      escapeSRM(true);
      break;
    }
    case 20: {
      escapeLNM(true);
      break;
    }
    default: {
      UNHANDLED1("SM", num)
      break;
    }
  }
}

void
CEscapeHandler::
escapeDECRST(int num)
{
  logTrace("<DECRST;" + CStrUtil::toString(num) + ">");

  switch (num) {
    case 1: {
      escapeDECCKM(false);
      break;
    }
    case 2: {
      escapeDECANM(false);
      break;
    }
    case 3: {
      escapeDECCOLM(false);
      break;
    }
    case 4: {
      escapeDECSCLM(false);
      break;
    }
    case 5: {
      escapeDECSCNM(false);
      break;
    }
    case 6: {
      escapeDECOM(false);
      break;
    }
    case 7: {
      escapeDECAWM(false);
      break;
    }
    case 8: {
      escapeDECARM(false);
      break;
    }
    case 9: {
      setSendMousePress(false);
      break;
    }
    case 10: {
      setShowToolBar(false);
      break;
    }
    case 12: {
      setCursorBlink(false);
      break;
    }
    case 18: {
      escapeDECPFF(false);
      break;
    }
    case 19: {
      escapeDECPEX(false);
      break;
    }
    case 25: {
      escapeDECTCEM(false);
      break;
    }
    case 30: {
      setShowScrollBar(false);
      break;
    }
    case 35: {
      // Enable shift fonts - rxvt
      break;
    }
    case 38: {
      escapeDECTEK(false);
      break;
    }
    case 40: {
      // Disallow 80 <-> 132
      setAllow80To132(false);
      break;
    }
    case 41: {
      // disable more fix
      break;
    }
    case 42: {
      escapeDECNRCM(false);
      break;
    }
    case 44: {
      // disable margin bell
      break;
    }
    case 45: {
      setReverseWrap(false);
      break;
    }
    case 46: {
      // stop logging
      break;
    }
    case 47: {
      // use normal screen
      setAlternative(false);
      break;
    }
    case 66: {
      escapeDECNKM(false);
      break;
    }
    case 67: {
      escapeDECBKM(false);
      break;
    }
    case 1000: {
      setSendMousePress  (false);
      setSendMouseRelease(false);
      break;
    }
    case 1001: {
      // no hilite mouse tracking
      break;
    }
    case 1002: {
      // don't use cell motion mouse tracking
      setSendMousePress  (false);
      setSendMouseRelease(false);
      break;
    }
    case 1003: {
      // don't use all motion mouse tracking
      break;
    }
    case 1010: {
      setScrollBottomOnTty(false);
      break;
    }
    case 1011: {
      setScrollBottomOnKey(false);
      break;
    }
    case 1035: {
      // disable special modifiers Alt/NumLock
      break;
    }
    case 1036: {
      // Don't send ESC for meta
      break;
    }
    case 1037: {
      // Delete is Normal (not DEL)
      break;
    }
    case 1047: {
      // use normal screen
      setAlternative(false);
      break;
    }
    case 1048: {
      // restore cursor
      restoreCursor();
      break;
    }
    case 1049: {
      // save cursor and use normal screen
      setAlternative(false);
      restoreCursor();
      break;
    }
    case 1051: {
      // reset sun fn key mode
      break;
    }
    case 1052: {
      // reset hp fn key mode
      break;
    }
    case 1053: {
      // reset sco fn key mode
      break;
    }
    case 1060: {
      // reset legacy keyboard emulation
      break;
    }
    case 1061: {
      // reset vt220 keyboard emulation
      break;
    }
    default: {
      UNHANDLED1("DECRST", num)
      break;
    }
  }
}

void
CEscapeHandler::
escapeRM(int num)
{
  logTrace("<RM;" + CStrUtil::toString(num) + ">");

  switch (num) {
    case 2: {
      escapeAM(false);
      break;
    }
    case 4: {
      escapeIRM(false);
      break;
    }
    case 12: {
      escapeSRM(false);
      break;
    }
    case 20: {
      escapeLNM(false);
      break;
    }
    default: {
      UNHANDLED1("RM", num)
      break;
    }
  }
}

void
CEscapeHandler::
escapeSGR(const CEscapeDataSGR *sgr)
{
  int *num = sgr->num;
  int  nn  = sgr->nn;

  assert(nn);

  logTrace("<SGR;" + CStrUtil::toString(num[0]) + ">");

  CCellStyle style = getStyle();

  updateStyleFromSGR(sgr, style);

  setStyle(style);
}

// Selects top and bottom margins, defining the scrolling region.
// 'top' is line number of first line in the scrolling region.
// 'bottom' is line number of bottom line.
// If 'top' and 'bottom' are not set (-1), the complete screen is used (no margins).
// Minimum of two lines for scroll region ('top' must be less than 'bottom')
void
CEscapeHandler::
escapeDECSTBM(int top, int bottom)
{
  logTrace("<DECSTBM;" + CStrUtil::toString(top   ) + ";" + CStrUtil::toString(bottom) + ">");

  if (top > 0 && bottom <= 0) {
    uint rows, cols;

    getDispSize(&rows, &cols);

    bottom = rows;
  }

  if (top > 0 && bottom > 0 && top < bottom) {
    setScrollArea(top, bottom);

    if (getOriginMode())
      escapeDispPos(0, 0);
  }
  else
    resetScrollArea();
}

void
CEscapeHandler::
escapeDECID()
{
  logTrace("<DECID>");

  processString("[?1;2c");
}

void
CEscapeHandler::
escapeDA1(int *, int nn)
{
  logTrace("<DA1>");

  if (nn == 0 || nn == 1)
    processString("[?1;2c");
}

void
CEscapeHandler::
escapeDA2(int *, int nn)
{
  logTrace("<DA2>");

  // vt220=1,ver=1000,rom_ver=0
  if (nn == 0 || nn == 1)
    processString("[>0;241;0c");
}

void
CEscapeHandler::
escapeDECDSR(int mode)
{
  logTrace("<DECDSR;" + CStrUtil::toString(mode) + ">");

  if      (mode == 5) {
    processString("[?0n");
  }
  else if (mode == 6) {
    int row, col;

    getDispPos(&row, &col);

    string str = "[?" + CStrUtil::toString(row + 1) +
                    ";" + CStrUtil::toString(col + 1) + "R";

    processString(str.c_str());
  }
  else if (mode == 15)
    processString("[?10n");
  else if (mode == 25)
    processString("[?20n");
  else if (mode == 26)
    processString("[?27;1;0;0n");
  else if (mode == 53)
    processString("[?53n");
}

void
CEscapeHandler::
escapeDSR(int num)
{
  logTrace("<DSR;" + CStrUtil::toString(num) + ">");

  if      (num == 5)
    processString("[0n");
  else if (num == 6) {
    int row, col;

    getDispPos(&row, &col);

    string str = "[" + CStrUtil::toString(row + 1) +
                   ";" + CStrUtil::toString(col + 1) + "R";

    processString(str.c_str());
  }
}

void
CEscapeHandler::
escapeWindowManip(int *num)
{
  if (num[0] < 0)
    num[0] = 0;

  CWindow *window = getWindow();

  switch (num[0]) {
    case 0:
      break;
    case 1: {
      if (window)
        window->deiconize();
      else
        deiconizeWindow();

      break;
    }
    case 2: {
      if (window)
        window->iconize();
      else
        iconizeWindow();

      break;
    }
    case 3:
      if (window)
        window->move(num[1], num[2]);
      else
        moveWindow(num[1], num[2]);

      break;
    case 4:
      if (window)
        window->resize(num[2], num[1]);
      else
        resizeWindow(num[2], num[1]);

      break;
    case 5:
      if (window)
        window->raise();
      else
        raiseWindow();

      break;
    case 6:
      if (window)
        window->lower();
      else
        lowerWindow();

      break;
    case 7:
      if (window)
        window->expose();
      else
        exposeWindow();

      break;
    case 8: {
      int w, h;

      charsToPixels(num[2], num[1], &w, &h);

      if (window)
        window->resize(num[2], num[1]);
      else
        resizeWindow(num[2], num[1]);

      break;
    }
    case 9:
      if      (num[1] == 0)
        demaximizeWindow();
      else if (num[1] == 1)
        maximizeWindow();

      break;
    case 10:
      break;
    case 11: {
      bool mapped = (window ? window->isMapped() : isWindowMapped());

      processString(mapped ? "[1t" : "[2t");

      break;
    }
    case 12:
      break;
    case 13: {
      int x, y;

      if (window)
        window->getPosition(&x, &y);
      else
        getWindowPosition(&x, &y);

      string str = "[3;" + CStrUtil::toString(x) + ";" + CStrUtil::toString(y) + "t";

      processString(str.c_str());

      break;
    }
    case 14: {
      uint width, height;

      if (window)
        window->getSize(&width, &height);
      else
        getWindowSize(&width, &height);

      string str = "[4;" + CStrUtil::toString(height) + ";" + CStrUtil::toString(width ) + "t";

      processString(str.c_str());

      break;
    }
    case 15:
    case 16:
    case 17:
      break;
    case 18: {
      string str = "[8;" + CStrUtil::toString(getNumDispRows()) + ";" +
                             CStrUtil::toString(getNumDispCols()) + "t";

      processString(str.c_str());

      break;
    }
    case 19: {
      uint width, height;

      if (window)
        window->getScreenSize(&width, &height);
      else
        getScreenSize(&width, &height);

      int rows, cols;

      pixelsToChars(width, height, &cols, &rows);

      string str = "[9;" + CStrUtil::toString(rows) + ";" + CStrUtil::toString(cols) + "t";

      processString(str.c_str());

      break;
    }
    case 20: {
      string name;

      if (window)
        window->getIconTitle(name);
      else
        getIconTitle(name);

      string str = "]L" + name + "";

      processString(str.c_str());

      break;
    }
    case 21: {
      string name;

      if (window)
        window->getWindowTitle(name);
      else
        getWindowTitle(name);

      string str = "]1" + name + "";

      processString(str.c_str());

      break;
    }
    case 22:
    case 23:
      break;
    default: {
      escapeDECSLPP(num[0]);

      break;
    }
  }
}

void
CEscapeHandler::
escapeDECCARA(int, int, int, int, int)
{
  UNHANDLED("DECCARA")
}

void
CEscapeHandler::
escapeDECRARA(int, int, int, int, int)
{
  UNHANDLED("DECRARA")
}

void
CEscapeHandler::
escapeDECCRA(int, int, int, int, int, int, int, int)
{
  UNHANDLED("DECCRA")
}

void
CEscapeHandler::
escapeDECFRA(int, int, int, int, int)
{
  UNHANDLED("DECFRA")
}

void
CEscapeHandler::
escapeDECSACE(int, int, int)
{
  UNHANDLED("escapeDECSACE")
}

void
CEscapeHandler::
escapeDECERA(int, int, int, int)
{
  UNHANDLED("DECERA")
}

void
CEscapeHandler::
escapeDECSERA(int, int, int, int)
{
  UNHANDLED("DECSERA")
}

void
CEscapeHandler::
escapeDECREQTPARM(int num)
{
  if      (num == 0)
    processString("[2;1;1;128;128;1;0x");
  else if (num == 1)
    processString("[3;1;1;128;128;1;0x");
}

void
CEscapeHandler::
escapeDECTST(int num)
{
  UNHANDLED1("DECTST", num)
}

void
CEscapeHandler::
escapeS7C1T()
{
  UNHANDLED("S7C1T")
}

void
CEscapeHandler::
escapeS8C1T()
{
  UNHANDLED("S8C1T")
}

void
CEscapeHandler::
escapeANSIConformance(int mode)
{
  UNHANDLED1("ANSI Conformance", mode)
}

void
CEscapeHandler::
escapeDECDHLTop()
{
  UNHANDLED1("DECDHL", "top")
}

void
CEscapeHandler::
escapeDECDHLBottom()
{
  UNHANDLED1("DECDHL", "bottom")
}

void
CEscapeHandler::
escapeDECSWL()
{
  UNHANDLED("DECSWL")
}

void
CEscapeHandler::
escapeDECDWL()
{
  UNHANDLED("DECDWL")
}

void
CEscapeHandler::
escapeDECALN()
{
  logTrace("<DECALN>");

  fill('E');
}

void
CEscapeHandler::
escapeISO8859_1()
{
  UNHANDLED("ISO8859-1")
}

void
CEscapeHandler::
escapeUTF_8()
{
  UNHANDLED("UTF-8")
}

void
CEscapeHandler::
escapeDesignateG0(char c)
{
  charset_.id[0] = c;
}

void
CEscapeHandler::
escapeDesignateG1(char c)
{
  charset_.id[1] = c;
}

void
CEscapeHandler::
escapeDesignateG2(char c)
{
  charset_.id[2] = c;
}

void
CEscapeHandler::
escapeDesignateG3(char c)
{
  charset_.id[3] = c;
}

// Saves cursor position, character attribute (graphic rendition),
// character set, and origin mode selection. (See restore cursor).
void
CEscapeHandler::
escapeDECSC()
{
  saveCursor();
}

void
CEscapeHandler::
escapeSC()
{
  saveCursor();
}

// Restores previously saved cursor position, character attribute (graphic rendition),
// character set, and origin mode selection.
// If none were saved, the cursor moves to home position.
void
CEscapeHandler::
escapeDECRC()
{
  restoreCursor();
}

void
CEscapeHandler::
escapeRC()
{
  restoreCursor();
}

// Selects application keypad mode. Keypad generates control functions.
// Cursor key mode (DECCKM) selects type of characters generated by cursor keys.
void
CEscapeHandler::
escapeDECPAM()
{
  logTrace("<DECPAM>");

  setKeyPadMode(true);
}

// Selects numeric keypad mode. Keypad generates characters that match the numeric,
// comma, period, and minus sign keys on main keyboard. Cursor keys generate ANSI
// cursor control sequences.
void
CEscapeHandler::
escapeDECPNM()
{
  logTrace("<DECPNM>");

  setKeyPadMode(false);
}

void
CEscapeHandler::
escapeRIS()
{
  clearScreen();

  resetAll();
}

void
CEscapeHandler::
escapeMemoryLock()
{
  // set scroll area ?
  UNHANDLED("escapeMemoryLock");
}

void
CEscapeHandler::
escapeMemoryUnlock()
{
  // reset scroll area ?
  UNHANDLED("escapeMemoryUnlock");
}

void
CEscapeHandler::
escapeLS2()
{
  UNHANDLED("LS2")
}

void
CEscapeHandler::
escapeLS3()
{
  UNHANDLED("LS3")
}

void
CEscapeHandler::
escapeLS1R()
{
  UNHANDLED("LS1R")
}

void
CEscapeHandler::
escapeLS2R()
{
  UNHANDLED("LS2R")
}

void
CEscapeHandler::
escapeLS3R()
{
  UNHANDLED("LS3R")
}

// Moves cursor down one line in same column.
// If cursor is at bottom margin, screen performs a scroll-up.
void
CEscapeHandler::
escapeIND()
{
  if (getDataPos().y >= int(getNumDataRows() - 1))
    scrollUp();
  else
    incOutputRow();
}

// Moves cursor to first position on next line.
// If cursor is at bottom margin, screen performs a scroll-up.
void
CEscapeHandler::
escapeNEL()
{
  if (getDataPos().y >= int(getNumDataRows() - 1))
    scrollUp();
  else
    incOutputRow();

  setDispCol(0);
}

void
CEscapeHandler::
escapeESA()
{
  UNHANDLED("ESA")
}

void
CEscapeHandler::
escapeHTS()
{
  setTab(getDataPos().x);
}

// Moves cursor up one line in same column.
// If cursor is at top margin, screen performs a scroll-down.
void
CEscapeHandler::
escapeRI()
{
  if (getDataPos().y <= 0)
    scrollDown();
  else
    decOutputRow();
}

void
CEscapeHandler::
escapeSS2()
{
  UNHANDLED("SS2")
}

void
CEscapeHandler::
escapeSS3()
{
  UNHANDLED("SS3")
}

void
CEscapeHandler::
escapeCCH()
{
  UNHANDLED("CCH")
}

void
CEscapeHandler::
escapeSPA()
{
  UNHANDLED("SPA")
}

void
CEscapeHandler::
escapeEPA()
{
  UNHANDLED("EPA")
}

void
CEscapeHandler::
escapeSOS()
{
  UNHANDLED("SOS")
}

void
CEscapeHandler::
escapeDECSTR()
{
  UNHANDLED("DECSTR")
}

void
CEscapeHandler::
escapeDECSCL(int, int)
{
  UNHANDLED("DECSCL")
}

void
CEscapeHandler::
escapeDECSCA(int)
{
  UNHANDLED("DECSCA")
}

void
CEscapeHandler::
escapeDECLL(int)
{
  UNHANDLED("DECLL")
}

void
CEscapeHandler::
escapeDECEFR(int, int, int, int)
{
  UNHANDLED("DECEFR")
}

void
CEscapeHandler::
escapeDECLRP(int, int, int, int)
{
  UNHANDLED("DECLRP")
}

void
CEscapeHandler::
escapeDECELR(int, int)
{
  UNHANDLED("DECELR")
}

void
CEscapeHandler::
escapeDECSLE(int)
{
  UNHANDLED("DECSLE")
}

void
CEscapeHandler::
escapeDECRQLP(int)
{
  UNHANDLED("DECRQLP")
}

// Set   : selects cursor keys to generate control (application) functions.
// Reset : selects cursor keys to generate ANSI cursor control sequences.
void
CEscapeHandler::
escapeDECCKM(bool set)
{
  setApplicationCursorKeys(set);
}

void
CEscapeHandler::
escapeDECANM(bool set)
{
  setAnsiVT52Mode(set);
}

// This mode selects the number of columns in a display line, 80 or 132.
// NOTE: When you change the number of columns per line, the screen is erased.
// This also sets the scrolling region for full screen
void
CEscapeHandler::
escapeDECCOLM(bool set)
{
  if (! getAllow80To132()) return;

  if (set)
    setDispSize(getNumDispRows(), 132);
  else
    setDispSize(getNumDispRows(), 80);

  clearScreen();

  resetScrollArea();

  escapeDispPos(0, 0);
}

void
CEscapeHandler::
escapeDECSLPP(int num_rows)
{
  setDispSize(num_rows, getNumDataCols());
}

void
CEscapeHandler::
escapeDECSCLM(bool set)
{
  setSmoothScroll(set);
}

// This mode selects either light (reverse) or dark display background on the screen.
// Set   : selects reverse screen, a white screen background with black characters.
// Reset : selects normal  screen, a black screen background with white characters.
void
CEscapeHandler::
escapeDECSCNM(bool set)
{
  setInverseVideo(set);
}

// If set:
//  Selects home position in scrolling region.
//  Line numbers start at top margin of scrolling region.
//  The cursor cannot move out of scrolling region.
// If reset:
//  Reset selects home position in upper-left corner of screen.
//  Line numbers are independent of the scrolling region (absolute).
//  Use CUP and HVP sequences to move cursor out of scrolling region.
void
CEscapeHandler::
escapeDECOM(bool set)
{
  setOriginMode(set);

  escapeDispPos(0, 0);
}

// This mode selects where a received character will appear when the cursor
// is at the right margin.
//
// When Set:
//  Any display characters received when cursor is at right margin appear on next line.
//  The display scrolls up if cursor is at end of scrolling region.
// When No Set:
//  Reset turns auto wrap off. Display characters received when cursor is at right margin
//  replace previously displayed character.
//
// NOTE: Regardless of the auto wrap SET-UP feature selection, the tab character
// never moves the cursor to the next line.
void
CEscapeHandler::
escapeDECAWM(bool set)
{
  setLineWrap(set);
}

// Set   : selects auto repeat. A key pressed for more than one-half second automatically repeats.
// Reset : turns off auto repeat. Keys do not automatically repeat.
void
CEscapeHandler::
escapeDECARM(bool set)
{
  setAutoRepeat(set);
}

void
CEscapeHandler::
escapeDECPFF(bool)
{
  UNHANDLED("DECPFF")
}

void
CEscapeHandler::
escapeDECPEX(bool)
{
  UNHANDLED("DECPEX")
}

void
CEscapeHandler::
escapeDECTCEM(bool set)
{
  setCursorVisible(set);
}

void
CEscapeHandler::
escapeDECTEK(bool)
{
  UNHANDLED("DECTEK")
}

void
CEscapeHandler::
escapeDECNRCM(bool)
{
  UNHANDLED("DECNRCM")
}

void
CEscapeHandler::
escapeDECNKM(bool)
{
  UNHANDLED("DECNKM")
}

void
CEscapeHandler::
escapeDECBKM(bool)
{
  UNHANDLED("DECBKM")
}

// This mode is only used during full-duplex, not half-duplex communication.
// Keyboard action lets the computer turn the keyboard on or off.
// This mode always resets when you enter SET-UP.
//
// Set   : turns off keyboard and turns on  KBD LOCKED.
// Reset : turns on  keyboard and turns off KBD LOCKED.
void
CEscapeHandler::
escapeAM(bool)
{
  UNHANDLED("AM")
}

void
CEscapeHandler::
escapeIRM(bool flag)
{
  setInsertMode(flag);
}

// Local Echo (Keyboard Send-Receive)
//
// This mode selects local echo, which causes every character transmitted by the
// terminal to automatically appear on the screen. Therefore, the computer does not
// have to transmit (echo) the character back to the terminal for display.
// When local echo is off, the terminal only transmits characters to the computer.
// The computer must echo the characters back to the terminal for display.
//
// Set   : turns off local echo. The terminal transmits characters to the computer,
//         which must echo characters for display on screen.
// Reset : selects local echo. Characters transmitted to the computer automatically
//         appear on the screen.
void
CEscapeHandler::
escapeSRM(bool)
{
  UNHANDLED("SRM")
}

// This mode selects the control character(s) transmitted by RETURN.
// Linefeed/new line also selects the action taken by the terminal when receiving linefeed,
// form feed, and vertical tab.
//
// Set   : causes a received linefeed, form feed, or vertical tab to move cursor to first column
//         of next line. RETURN transmits both a carriage return and linefeed.
//         This selection is also called new line option.
// Reset : causes a received linefeed, form feed, or vertical tab to move cursor to next line
//         in current column. RETURN transmits a carriage return.
void
CEscapeHandler::
escapeLNM(bool set)
{
  setLfNlMode(set);
}

void
CEscapeHandler::
escapeDECSavePriv(int *, int)
{
  UNHANDLED("DEC Save Private Mode")
}

void
CEscapeHandler::
escapeDECRestorePriv(int *, int)
{
  UNHANDLED("DEC Restore Private Mode")
}

void
CEscapeHandler::
escapeStartMouseTrack(int, int, int, int, int)
{
  UNHANDLED("Start Mouse Tracking")
}

void
CEscapeHandler::
escapeOSC(int num, const string &str1)
{
  CWindow *window = getWindow();

  switch (num) {
    case 0: {
      logTrace("<OSC;0;" + str1 + ">");

      if (window) {
        window->setWindowTitle(str1);
        window->setIconTitle  (str1);
      }
      else {
        setWindowTitle(str1);
        setIconTitle  (str1);
      }

      break;
    }
    case 1: {
      logTrace("<OSC;1;" + str1 + ">");

      if (window)
        window->setIconTitle(str1);
      else
        setIconTitle(str1);

      break;
    }
    case 2: {
      logTrace("<OSC;2;" + str1 + ">");

      if (window)
        window->setWindowTitle(str1);
      else
        setWindowTitle(str1);

      break;
    }
    case 3: {
      string::size_type pos = str1.find('=');

      string name, value;

      if (pos == string::npos)
        name = str1;
      else {
        name  = str1.substr(0, pos);
        value = str1.substr(pos + 1);
      }

      if (window)
        window->setProperty(name, value);
      else
        setWindowProperty(name, value);

      break;
    }
    case 4:  // Change Color Number
      UNHANDLED1("OSC Escape", num)
      break;
    case 10:   // Change Text Foreground Color
    case 11:   // Change Text Background Color
    case 12:   // Change Text Cursor Color
    case 13:   // Change Mouse Foreground Color
    case 14:   // Change Mouse Background Color
    case 15:   // Change Tek Foreground Color
    case 16:   // Change Tek Background Color
    case 17: { // Change Highlight Color (Tek Cursor Color ?)
      int start = num - 10;

      vector<string> words;

      CStrUtil::addFields(str1, words, ";");

      uint num_words = words.size();

      for (uint i = 0; i < num_words; ++i) {
        uint pos = i + start;

        switch (pos) {
          case 0: // Text Foreground Color
            setColor(CESCAPE_COLOR_FG, CRGBName::toRGBA(words[i]));
            break;
          case 1: // Text Background Color
            setColor(CESCAPE_COLOR_BG, CRGBName::toRGBA(words[i]));
            break;
          case 2: // Text Cursor Color
            setCursorColor(CRGBName::toRGBA(words[i]));
            break;
          case 3: // Mouse Foreground Color
            break;
          case 4: // Mouse Background Color
            break;
          case 5: // Tek Foreground Color
            break;
          case 6: // Tek Background Color
            break;
          case 7: // Highlight Color (Tek Cursor Color ?)
            break;
          default:
            break;
        }
      }

      break;
    }
    case 18: // Change Cursor Color
      UNHANDLED1("OSC Escape", num)
      break;
    case 46: // Change Log File
      UNHANDLED1("OSC Escape", num)
      break;
    case 50: // Change Font
      UNHANDLED1("OSC Escape", num)
      break;
    default:
      break;
  }
}

void
CEscapeHandler::
incOutputRow()
{
  int row, col;

  getDispPos(&row, &col);

  ++row;

  int scroll_bottom = getScrollBottom();

  if (row > scroll_bottom - 1)
    scrollUp();
  else
    setDispPos(row, col);
}

void
CEscapeHandler::
decOutputRow()
{
  int row, col;

  getDispPos(&row, &col);

  --row;

  int scroll_top = getScrollTop();

  if (row < scroll_top - 1)
    scrollDown();
  else
    setDispPos(row, col);
}

void
CEscapeHandler::
incOutputCol()
{
  int row, col;

  getDispPos(&row, &col);

  ++col;

  setDispPos(row, col);
}

void
CEscapeHandler::
decOutputCol()
{
  int row, col;

  getDispPos(&row, &col);

  if (col > 0)
    --col;

  setDispPos(row, col);
}

//----------

void
CEscapeHandler::
nextTab()
{
  uint rows, cols;

  getDispSize(&rows, &cols);

  tabs_.setLength(cols);

  int row, col;

  getDispPos(&row, &col);

  int col1;

  if (tabs_.nextPos(col, &col1))
    setDispPos(row, col1);
}

void
CEscapeHandler::
prevTab()
{
  uint rows, cols;

  getDispSize(&rows, &cols);

  tabs_.setLength(cols);

  int row, col;

  getDispPos(&row, &col);

  int col1;

  if (tabs_.prevPos(col, &col1))
    setDispPos(row, col1);
}

void
CEscapeHandler::
setTab(uint x)
{
  tabs_.set(x);
}

void
CEscapeHandler::
clearTab(uint x)
{
  tabs_.clear(x);
}

void
CEscapeHandler::
clearTabAll()
{
  tabs_.clearAll();
}

//----------

uint
CEscapeHandler::
getScrollTop() const
{
  if (scroll_area_.isSet())
    return scroll_area_.getTop();
  else
    return 1;
}

uint
CEscapeHandler::
getScrollBottom() const
{
  if (scroll_area_.isSet())
    return scroll_area_.getBottom();
  else {
    uint rows, cols;

    getDispSize(&rows, &cols);

    return rows;
  }
}

//----------

void
CEscapeHandler::
saveCursor()
{
  int row, col;

  getDispPos(&row, &col);

  save_cursor_.row = row;
  save_cursor_.col = col;

  save_cursor_.state = getState();

  save_cursor_.style = getStyle();

  save_cursor_.charset = charset_;
}

void
CEscapeHandler::
restoreCursor()
{
  setDispPos(save_cursor_.row, save_cursor_.col);

  setState(save_cursor_.state);

  setStyle(save_cursor_.style);

  charset_ = save_cursor_.charset;
}

//----------

char
CEscapeHandler::
getCharSet(int id) const
{
  return charset_.id[id];
}

const CRGBA &
CEscapeHandler::
getColor(CEscapeColor color) const
{
  return colors_.getColor(color);
}

void
CEscapeHandler::
setColor(CEscapeColor color, const CRGBA &rgba)
{
  colors_.setColor(color, rgba);
}

const CRGBA &
CEscapeHandler::
getCursorColor() const
{
  return cursor_color_;
}

void
CEscapeHandler::
setCursorColor(const CRGBA &rgba)
{
  cursor_color_ = rgba;
}

//----------

void
CEscapeHandler::
sendMousePress(int button, int x, int y)
{
  static char str[7];

  str[0] = '';
  str[1] = '[';
  str[2] = 'M';
  str[3] = char(button + 32);
  str[4] = char(x + 32);
  str[5] = char(y + 32);
  str[6] = '\0';

  processString(str);
}

void
CEscapeHandler::
sendMouseRelease(int button, int x, int y)
{
  static char str[7];

  button = 3; // release

  str[0] = '';
  str[1] = '[';
  str[2] = 'M';
  str[3] = char(button + 32);
  str[4] = char(x + 32);
  str[5] = char(y + 32);
  str[6] = '\0';

  processString(str);
}

void
CEscapeHandler::
logTrace(char c) const
{
  logTrace(string(&c, 1));
}
