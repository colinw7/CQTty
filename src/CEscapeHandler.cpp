#include <CEscapeHandler.h>
#include <CEscapeColors.h>
#include <CCellStyle.h>
#include <CWindow.h>
#include <CRGBName.h>
#include <CUtf8.h>
#include <limits>
#include <sstream>

#define UNHANDLED(m) { \
  std::ostringstream ostr; \
  ostr << "Unhandled Esc: " << m << "\n"; \
  log(ostr.str()); \
}

#define UNHANDLED1(m,a) { \
  std::ostringstream ostr; \
  ostr << "Unhandled Esc: " << m << " " << a << "\n"; \
  log(ostr.str()); \
}

#define UNHANDLED2(m,a,b) { \
  std::ostringstream ostr; \
  ostr << "Unhandled Esc: " << m << " " << a << " " << b << "\n"; \
  log(ostr.str()); \
}

#define UNHANDLED3(m,a,b,c) { \
  std::ostringstream ostr; \
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
  if (is4014()) {
    set4014CharSet(state_.get4014CharSet());

    draw4014Char(c);

    exec4014CUF();

    return;
  }

  ulong utf_char { 0 };
  bool  is_utf   { false };

  char char_set = charset_.getCurrentSetType();

  if (char_set == '0') {
    //if      (c == 95           ) c  = 127;
    //else if (c >  95 && c < 127) c -= 95;

    // 0x5f Blank                              U+00A0 NO-BREAK SPACE
    // 0x60 Diamond                    ◆       U+25C6 BLACK DIAMOND
    // 0x61 Checkerboard               ▒       U+2592 MEDIUM SHADE
    // 0x62 HT                         ␉       U+2409 SYMBOL FOR HORIZONTAL TABULATION
    // 0x63 FF                         ␌       U+240C SYMBOL FOR FORM FEED
    // 0x64 CR                         ␍       U+240D SYMBOL FOR CARRIAGE RETURN
    // 0x65 LF                         ␊       U+240A SYMBOL FOR LINE FEED
    // 0x66 Degree symbol              °       U+00B0 DEGREE SIGN
    // 0x67 Plus/minus                 ±       U+00B1 PLUS-MINUS SIGN
    // 0x68 NL                         ␤       U+2424 SYMBOL FOR NEWLINE
    // 0x69 VT                         ␋       U+240B SYMBOL FOR VERTICAL TABULATION
    // 0x6a Lower-right corner         ┘       U+2518 BOX DRAWINGS LIGHT UP AND LEFT
    // 0x6b Upper-right corner         ┐       U+2510 BOX DRAWINGS LIGHT DOWN AND LEFT
    // 0x6c Upper-left corner          ┌       U+250C BOX DRAWINGS LIGHT DOWN AND RIGHT
    // 0x6d Lower-left corner          └       U+2514 BOX DRAWINGS LIGHT UP AND RIGHT
    // 0x6e Crossing Lines             ┼       U+253C BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL
    // 0x6f Horizontal line - scan 1   ⎺       U+23BA HORIZONTAL SCAN LINE-1
    // 0x70 Horizontal line - scan 3   ⎻       U+23BB HORIZONTAL SCAN LINE-3
    // 0x71 Horizontal line - scan 5   ─       U+2500 BOX DRAWINGS LIGHT HORIZONTAL
    // 0x72 Horizontal line - scan 7   ⎼       U+23BC HORIZONTAL SCAN LINE-7
    // 0x73 Horizontal line - scan 9   ⎽       U+23BD HORIZONTAL SCAN LINE-9
    // 0x74 Left "T"                   ├       U+251C BOX DRAWINGS LIGHT VERTICAL AND RIGHT
    // 0x75 Right "T"                  ┤       U+2524 BOX DRAWINGS LIGHT VERTICAL AND LEFT
    // 0x76 Bottom "T"                 ┴       U+2534 BOX DRAWINGS LIGHT UP AND HORIZONTAL
    // 0x77 Top "T"                    ┬       U+252C BOX DRAWINGS LIGHT DOWN AND HORIZONTAL
    // 0x78 Vertical bar               │       U+2502 BOX DRAWINGS LIGHT VERTICAL
    // 0x79 Less than or equal to      ≤       U+2264 LESS-THAN OR EQUAL TO
    // 0x7a Greater than or equal to   ≥       U+2265 GREATER-THAN OR EQUAL TO
    // 0x7b Pi                         π       U+03C0 GREEK SMALL LETTER PI
    // 0x7c Not equal to               ≠       U+2260 NOT EQUAL TO
    // 0x7d UK pound symbol            £       U+00A3 POUND SIGN
    // 0x7e Centered dot               ·       U+00B7 MIDDLE DOT
    if (c >= 0x5f && c <= 0x7e) {
      static std::string utf_chars =
        "\u00A0\u25C6\u2592\u2409\u240C\u240D\u240A\u00B0"
        "\u00B1\u2424\u240B\u2518\u2510\u250C\u2514\u253C"
        "\u23BA\u23BB\u2500\u23BC\u23BD\u251C\u2524\u2534"
        "\u252C\u2502\u2264\u2265\u03C0\u2260\u00A3\u00B7";

      int ind = (c - 0x5f);

      int i1, i2;

      if (CUtf8::indexChar(utf_chars, ind, i1, i2)) {
        utf_char = CUtf8::readNextChar(utf_chars, i1, utf_chars.length());
        is_utf   = true;
      }
    }
  }

  //---

  // TODO: log utf ?

  logTrace(c);

  //---

  int x = getDataPos().x;
  int y = getDataPos().y;

  int num_cols = getNumDataCols();

  //---

  // if at last column then:
  //  wrap mode   : move to next line
  //  no wrap mode: clipped
  bool visible = true;

  if (x == num_cols - 1) {
    if (! isWrapChar()) {
      setWrapChar(true);
    }
    else {
      if (getLineWrap()) {
        x = 0;

        ++y;
      }
      else {
        visible = false;
      }
    }
  }

  //---

  if (is_utf)
    notifyUtfChar(x, y, utf_char);
  else
    notifyChar(x, y, c);

  //---

  if (getInsertMode())
    shiftRight();

  if (visible) {
    if (is_utf) {
      if (getInAPCType())
        setUtfLinkCell(y, x, utf_char, getStyle(), getAPCTypeType(), getAPCTypeValue());
      else
        setUtfCell(y, x, utf_char, getStyle());
    }
    else {
      if (getInAPCType())
        setLinkCell(y, x, c, getStyle(), getAPCTypeType(), getAPCTypeValue());
      else
        setCell(y, x, c, getStyle());
    }
  }

  //---

  if (x < num_cols - 1)
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
    // NUL
    case CEscapeType::NUL: {
      break;
    }
    // SOH
    case CEscapeType::SOH: {
      UNHANDLED("SOH");
      break;
    }
    // STX
    case CEscapeType::STX: {
      UNHANDLED("STX");
      break;
    }
    // Switch to VT100 Mode (ESC  Ctrl-C)
    case CEscapeType::ETX: {
      UNHANDLED("ETX");
      break;
    }
    // EOT
    case CEscapeType::EOT: {
      UNHANDLED("EOT");
      break;
    }
    // Return Terminal Status (Ctrl-E)
    // Default response is an empty string, but may be overridden
    // by a resource answerbackString
    case CEscapeType::ENQ: {
      // terminal status (default is empty)
      // escapeDA1(NULL, 0);
      break;
    }
    // ACK
    case CEscapeType::ACK: {
      UNHANDLED("ACK");
      break;
    }
    // Bell (Ctrl-G)
    case CEscapeType::BEL: {
      if (is4014()) {
        exec4014BEL();
      }
      else {
        beep();
      }

      break;
    }
    // Backspace (Ctrl-H)
    case CEscapeType::BS: {
      if (is4014()) {
        exec4014BS();
      }
      else {
        decOutputCol();
      }

      break;
    }
    // Horizontal Tab (TAB, HT) (Ctrl-I).
    case CEscapeType::HT: {
      if (is4014()) {
        exec4014TAB();
      }
      else {
        nextTab();
      }

      break;
    }
    // Line Feed or New Line (NL).
    // (LF is Ctrl-J).
    case CEscapeType::LF: {
      if (is4014()) {
        // move cursor down one line, wrap to top if off bottom selecting bypass condition
        // clear bypass condition
        exec4014LF();
      }
      else {
        incOutputRow();

        if (getLfNlMode()) {
          setDispCol(0);

          notifyEnter('\n');
        }
      }

      break;
    }
    // Vertical Tab (Ctrl-K).  This is treated the same as LF.
    case CEscapeType::VT: {
      if (is4014()) {
        exec4014VT();
      }
      else {
        incOutputRow();

        if (getLfNlMode()) {
          setDispCol(0);

          notifyEnter('\v');
        }
      }

      break;
    }
    // Form Feed or New Page (NP)
    // (FF is Ctrl-L). FF is treated the same as LF .
    case CEscapeType::FF: {
      if (is4014()) {
        exec4014FF();
      }
      else {
        if (getFfNpMode()) {
          clearScreen();
        }
        else {
          incOutputRow();

          if (getLfNlMode()) {
            setDispCol(0);

            notifyEnter('\f');
          }
        }
      }

      break;
    }
    // Carriage Return (Ctrl-M)
    case CEscapeType::CR: {
      if (is4014()) {
        // Carriage return, resets Terminal from Graph to Alpha Mode
        // canceles crosshair cursor
        exec4014CR();
      }
      else {
        setDispCol(0);

        notifyEnter('\r');
      }

      break;
    }
    // Shift Out (Ctrl-N) -> Switch to Alternate Character Set.
    // This invokes the G1 character set.
    case CEscapeType::SO: {
      charset_.setCurrentSet(1);
      break;
    }
    // Shift In (Ctrl-O) -> Switch to Standard Character Set.
    // This invokes the G0 character set (the default).
    case CEscapeType::SI: {
      charset_.setCurrentSet(0);
      break;
    }
    case CEscapeType::DLE: {
      break;
    }
    case CEscapeType::DC1: {
      break;
    }
    case CEscapeType::DC2: {
      break;
    }
    case CEscapeType::DC3: {
      break;
    }
    case CEscapeType::DC4: {
      break;
    }
    case CEscapeType::NAK: {
      break;
    }
    case CEscapeType::SYN: {
      break;
    }
    case CEscapeType::ETB: {
      break;
    }
    // cancel current sequence
    case CEscapeType::CAN: {
      UNHANDLED("CAN");
      break;
    }
    case CEscapeType::EM: {
      break;
    }
    case CEscapeType::SUB: {
      // cancel current sequence and substitute character ?
      UNHANDLED("SUB");
      break;
    }
    case CEscapeType::FS: {
      if (is4014()) {
        UNHANDLED("FS");
      }
      else {
        UNHANDLED("FS");
      }
      break;
    }
    case CEscapeType::GS: {
      const CEscapeDataGS *escg = dynamic_cast<const CEscapeDataGS *>(esc);

      if (is4014()) {
        state_.add4014Point(escg->x, escg->y);

        if (escg->mode == CEscapeDataGS::Mode::LINE_TO) {
          if (state_.get4014Points().size() > 1) {
            auto &p1 = state_.get4014Points()[state_.get4014Points().size() - 2];
            auto &p2 = state_.get4014Points()[state_.get4014Points().size() - 1];

            draw4014Line(p1.x, p1.y, p2.x, p2.y, CEscapeColor::WHITE, get4014LineStyle());
          }
        }
      }
      else {
        UNHANDLED("GS");
      }
      break;
    }
    case CEscapeType::RS: {
      if (is4014()) {
        UNHANDLED("RS");
      }
      else {
        UNHANDLED("RS");
      }
      break;
    }
    case CEscapeType::US: {
      if (is4014()) {
        UNHANDLED("US");
      }
      else {
        UNHANDLED("US");
      }
      break;
    }
    case CEscapeType::DEL: {
      break;
    }

    // Application Keypad (DECKPAM)
    case CEscapeType::DECPAM: {
      escapeDECPAM();
      break;
    }
    // Normal Keypad (DECKPNM)
    case CEscapeType::DECPNM: {
      escapeDECPNM();
      break;
    }
    // Invoke the G3 Character Set as GR (LS3R)
    case CEscapeType::LS3R: {
      escapeLS3R();
      break;
    }
    // Invoke the G2 Character Set as GR (LS2R)
    case CEscapeType::LS2R: {
      escapeLS2R();
      break;
    }
    // Invoke the G1 Character Set as GR (LS1R
    case CEscapeType::LS1R: {
      escapeLS1R();
      break;
    }
    // 7-bit controls (S7C1T).
    case CEscapeType::S7C1T: {
      escapeS7C1T();
      break;
    }
    // 8-bit controls (S8C1T).
    case CEscapeType::S8C1T: {
      escapeS8C1T();
      break;
    }
    // Set ANSI conformance level 1 (dpANS X3.134.1).
    case CEscapeType::ANSI1: {
      escapeANSIConformance(1);
      break;
    }
    // Set ANSI conformance level 2 (dpANS X3.134.1).
    case CEscapeType::ANSI2: {
      escapeANSIConformance(2);
      break;
    }
    // Set ANSI conformance level 3 (dpANS X3.134.1).
    case CEscapeType::ANSI3: {
      escapeANSIConformance(3);
      break;
    }
    // DEC double-height line, top half (DECDHL).
    case CEscapeType::DECDHL: {
      const CEscapeDataDECDHL *esc1 = dynamic_cast<const CEscapeDataDECDHL *>(esc);

      escapeDECDHL(esc1->pos);

      break;
    }
    // DEC single-width line (DECSWL).
    case CEscapeType::DECSWL: {
      escapeDECSWL();
      break;
    }
    // DEC double-width line (DECDWL)
    case CEscapeType::DECDWL: {
      escapeDECDWL();
      break;
    }
    // DEC Screen Alignment Test (DECALN).
    case CEscapeType::DECALN: {
      escapeDECALN();
      break;
    }
    // Select default character set.  That is ISO 8859-1 (ISO 2022).
    case CEscapeType::ISO8859_1: {
      escapeISO8859_1();
      break;
    }
    // Select UTF-8 character set (ISO 2022).
    case CEscapeType::UTF_8: {
      escapeUTF_8();
      break;
    }
    // Designate G0 Character Set (ISO 2022, VT100).
    case CEscapeType::G0: {
      const CEscapeDataC *escc = dynamic_cast<const CEscapeDataC *>(esc);

      escapeDesignateG0(escc->c);

      break;
    }
    // Designate G1 Character Set (ISO 2022, VT100).
    // The same character sets apply as for ESC ( C.
    case CEscapeType::G1: {
      const CEscapeDataC *escc = dynamic_cast<const CEscapeDataC *>(esc);

      escapeDesignateG1(escc->c);

      break;
    }
    // Designate G2 Character Set (ISO 2022, VT220).
    // The same character sets apply as for ESC ( C.
    case CEscapeType::G2: {
      const CEscapeDataC *escc = dynamic_cast<const CEscapeDataC *>(esc);

      escapeDesignateG2(escc->c);

      break;
    }
    // Designate G3 Character Set (ISO 2022, VT220).
    // The same character sets apply as for ESC ( C.
    case CEscapeType::G3: {
      const CEscapeDataC *escc = dynamic_cast<const CEscapeDataC *>(esc);

      escapeDesignateG3(escc->c);

      break;
    }
    // Index
    case CEscapeType::IND: {
      escapeIND();
      break;
    }
    // Next Line
    case CEscapeType::NEL: {
      escapeNEL();
      break;
    }
    // Start of Selected Area
    case CEscapeType::SSA: {
      escapeESA();
      break;
    }
    // End of Selected Area
    case CEscapeType::ESA: {
      escapeESA();
      break;
    }
    // Tab Set
    case CEscapeType::HTS: {
      escapeHTS();
      break;
    }
    // Reverse Index
    case CEscapeType::RI: {
      escapeRI();
      break;
    }
    // Single Shift Select of G2 Character Set (next character only)
    case CEscapeType::SS2: {
      escapeSS2();
      break;
    }
    // SS3
    case CEscapeType::SS3: {
      escapeSS3();
      break;
    }
    // SS3
    case CEscapeType::CCH: {
      escapeCCH();
      break;
    }
    // Start of Guarded Area
    case CEscapeType::SPA: {
      escapeSPA();
      break;
    }
    // End of Guarded Area
    case CEscapeType::EPA: {
      escapeEPA();
      break;
    }
    // Start of String
    case CEscapeType::SOS: {
      escapeSOS();
      break;
    }
    // Return Terminal ID
    case CEscapeType::DECID: {
      escapeDECID();
      break;
    }
    // Full Reset (RIS)
    case CEscapeType::RIS: {
      escapeRIS();
      break;
    }
    // Memory Lock (per HP terminals).
    // Locks memory above the cursor
    case CEscapeType::MemoryLock: {
      escapeMemoryLock();
      break;
    }
    // Memory Unlock (per HP terminals)
    case CEscapeType::MemoryUnlock: {
      escapeMemoryUnlock();
      break;
    }
    // Invoke the G2 Character Set as GL (LS2)
    case CEscapeType::LS2: {
      escapeLS2();
      break;
    }
    // Invoke the G3 Character Set as GL (LS3)
    case CEscapeType::LS3: {
      escapeLS3();
      break;
    }
    // Save Cursor (DECSC)
    case CEscapeType::DECSC: {
      escapeDECSC();
      break;
    }
    // Restore Cursor (DECRC)
    case CEscapeType::DECRC: {
      escapeDECRC();
      break;
    }
    // Erase in Display (DECSED).
    case CEscapeType::DECSED: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSED(escn->num[0]);

      break;
    }
    // Erase in Line (DECSEL).
    case CEscapeType::DECSEL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSEL(escn->num[0]);

      break;
    }
    case CEscapeType::DA2: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDA2(escn->num, escn->nn);

      break;
    }
    // DEC Private Mode Set (DECSET)
    case CEscapeType::DECSET: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      for (int i = 0; i < escn->nn; ++i)
        escapeDECSET(escn->num[i]);

      break;
    }
    // Dec-specific Media Copy (DECMC)
    case CEscapeType::DECMC: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECMC(escn->num[0]);

      break;
    }
    // DEC Private Mode Reset (DECRST)
    case CEscapeType::DECRST: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      for (int i = 0; i < escn->nn; ++i)
        escapeDECRST(escn->num[i]);

      break;
    }
    // Soft terminal reset (DECSTR)
    case CEscapeType::DECDSR: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECDSR(escn->num[0]);

      break;
    }
    case CEscapeType::DECSTR: {
      escapeDECSTR();

      break;
    }
    case CEscapeType::DECRestorePriv: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECRestorePriv(escn->num, escn->nn);

      break;
    }
    case CEscapeType::DECSavePriv: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSavePriv(escn->num, escn->nn);

      break;
    }
    case CEscapeType::HPA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeHPA(escn->num[0]);

      break;
    }
    // Set conformance level (DECSCL)
    case CEscapeType::DECSCL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSCL(escn->num[0], escn->num[1]);

      break;
    }
    case CEscapeType::DECSCA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSCA(escn->num[0]);

      break;
    }
    case CEscapeType::DECCARA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECCARA(escn->num[0], escn->num[1], escn->num[2], escn->num[3], escn->num[4]);

      break;
    }
    case CEscapeType::DECRARA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECRARA(escn->num[0], escn->num[1], escn->num[2], escn->num[3], escn->num[4]);

      break;
    }
    case CEscapeType::DECCRA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECCRA(escn->num[0], escn->num[1], escn->num[2], escn->num[3],
                   escn->num[4], escn->num[5], escn->num[6], escn->num[7]);

      break;
    }
    case CEscapeType::DECEFR: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECEFR(escn->num[0], escn->num[1], escn->num[2], escn->num[3]);

      break;
    }
    case CEscapeType::DECLRP: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECLRP(escn->num[0], escn->num[1], escn->num[2], escn->num[3]);

      break;
    }
    case CEscapeType::DECFRA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECFRA(escn->num[0], escn->num[1], escn->num[2], escn->num[3], escn->num[4]);

      break;
    }
    case CEscapeType::DECSACE: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSACE(escn->num[0], escn->num[1], escn->num[2]);

      break;
    }
    case CEscapeType::DECELR: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECELR(escn->num[0], escn->num[1]);

      break;
    }
    case CEscapeType::DECERA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECERA(escn->num[0], escn->num[1], escn->num[2], escn->num[3]);

      break;
    }
    case CEscapeType::DECSLE: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSLE(escn->num[0]);

      break;
    }
    case CEscapeType::DECSERA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECSERA(escn->num[0], escn->num[1], escn->num[2], escn->num[3]);

      break;
    }
    case CEscapeType::DECRQLP: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECRQLP(escn->num[0]);

      break;
    }
    case CEscapeType::ICH: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeICH(escn->num[0]);

      break;
    }
    case CEscapeType::CUU: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCUU(escn->nn > 0 ? escn->num[0] : 1);

      break;
    }
    case CEscapeType::CUD: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCUD(escn->nn > 0 ? escn->num[0] : 1);

      break;
    }
    case CEscapeType::CUF: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCUF(escn->nn > 0 ? escn->num[0] : 1);

      break;
    }
    case CEscapeType::CUB: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCUB(escn->nn > 0 ? escn->num[0] : 1);

      break;
    }
    case CEscapeType::CNL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCNL(escn->num[0]);

      break;
    }
    case CEscapeType::CPL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCPL(escn->num[0]);

      break;
    }
    case CEscapeType::CHA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCHA(escn->num[0]);

      break;
    }
    case CEscapeType::CUP: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCUP(escn->num[0], escn->num[1]);

      break;
    }
    case CEscapeType::CHT: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCHT(escn->num[0]);

      break;
    }
    case CEscapeType::ED: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeED(escn->num[0]);

      break;
    }
    case CEscapeType::EL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeEL(escn->num[0]);

      break;
    }
    case CEscapeType::IL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeIL(escn->num[0]);

      break;
    }
    case CEscapeType::DL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDL(escn->num[0]);

      break;
    }
    case CEscapeType::DCH: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDCH(escn->num[0]);

      break;
    }
    case CEscapeType::SU: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeSU(escn->num[0]);

      break;
    }
    case CEscapeType::SD: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeSD(escn->num[0]);

      break;
    }
    case CEscapeType::StartMouseTrack: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeStartMouseTrack(escn->num[0], escn->num[1], escn->num[2],
                            escn->num[3], escn->num[4]);

      break;
    }
    case CEscapeType::ECH: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeECH(escn->num[0]);

      break;
    }
    case CEscapeType::CBT: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeCBT(escn->num[0]);

      break;
    }
    case CEscapeType::REP: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeREP(escn->num[0]);

      break;
    }
    case CEscapeType::DA1: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDA1(escn->num, escn->nn);

      break;
    }
    case CEscapeType::VPA: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeVPA(escn->num[0]);

      break;
    }
    case CEscapeType::HVP: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeHVP(escn->num[0], escn->num[1]);

      break;
    }
    case CEscapeType::TBC: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeTBC(escn->num[0]);

      break;
    }
    case CEscapeType::SM: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeSM(escn->num[0]);

      break;
    }
    // Media Copy (MC)
    case CEscapeType::MC: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeMC(escn->num[0]);

      break;
    }
    // Reset Mode (RM)
    case CEscapeType::RM: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeRM(escn->num[0]);

      break;
    }
    // Character Attributes (SGR).
    case CEscapeType::SGR: {
      const CEscapeDataSGR *sgr = dynamic_cast<const CEscapeDataSGR *>(esc);

      escapeSGR(sgr);

      break;
    }
    case CEscapeType::DSR: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDSR(escn->num[0]);

      break;
    }
    case CEscapeType::DECLL: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECLL(escn->num[0]);

      break;
    }
    case CEscapeType::DECSTBM: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      if      (escn->nn <= 0)
        escapeDECSTBM(-1, -1);
      else if (escn->nn == 1)
        escapeDECSTBM(escn->num[0], -1);
      else
        escapeDECSTBM(escn->num[0], escn->num[1]);

      break;
    }
    case CEscapeType::SC:
      escapeSC();

      break;
    case CEscapeType::WindowManip: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeWindowManip(escn->num);

      break;
    }
    case CEscapeType::DECREQTPARM: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECREQTPARM(escn->num[0]);

      break;
    }
    case CEscapeType::DECTST: {
      const CEscapeDataNums *escn = dynamic_cast<const CEscapeDataNums *>(esc);

      escapeDECTST(escn->num[0]);

      break;
    }
    case CEscapeType::OSC: {
      const CEscapeDataOSC *esco = dynamic_cast<const CEscapeDataOSC *>(esc);

      escapeOSC(esco->num, esco->str);

      break;
    }
    case CEscapeType::TEK4014: {
      const CEscapeDataTek4014 *esc4014 = dynamic_cast<const CEscapeDataTek4014 *>(esc);

      escape4014(esc4014);

      break;
    }
    case CEscapeType::SET_DIR: {
      const CEscapeDataStr *escs = dynamic_cast<const CEscapeDataStr *>(esc);

      setDirName(escs->str);

      break;
    }
    case CEscapeType::SIZED_IMAGE: {
      const CEscapeDataFileImage *esci = dynamic_cast<const CEscapeDataFileImage *>(esc);

      if (esci->x1 > 0 && esci->y1 > 0 && esci->x2 > 0 && esci->y2 > 0) {
        addSizedImageChar(esci->name, esci->image, esci->x1, esci->y1, esci->x2, esci->y2);

        incOutputCol();
      }
      else
        addImage(esci->name, esci->image);

      break;
    }
    case CEscapeType::FILE_IMAGE: {
      const CEscapeDataFileImage *esci = dynamic_cast<const CEscapeDataFileImage *>(esc);

      addImageChar(esci->name, esci->image, esci->x1, esci->y1, esci->x2, esci->y2);

      incOutputCol();

      break;
    }
    case CEscapeType::IMAGE: {
      const CEscapeDataImage *esci = dynamic_cast<const CEscapeDataImage *>(esc);

      addImage(esci->image);

      break;
    }
    case CEscapeType::PIXEL: {
      const CEscapeDataPixel *escp = dynamic_cast<const CEscapeDataPixel *>(esc);

      addPixel(escp->x, escp->y, escp->color);

      break;
    }
    case CEscapeType::LINE: {
      const CEscapeDataLine *escl = dynamic_cast<const CEscapeDataLine *>(esc);

      addLine(escl->x1, escl->y1, escl->x2, escl->y2, escl->color, CEscapeLineStyle::SOLID);

      break;
    }
    case CEscapeType::LINK: {
      const CEscapeDataLink *escl = dynamic_cast<const CEscapeDataLink *>(esc);

      addLink(escl->name, escl->path, escl->type);

      break;
    }
    case CEscapeType::PREVIEW_FILES: {
      const CEscapeDataStr *escs = dynamic_cast<const CEscapeDataStr *>(esc);

      previewFile(escs->str);

      break;
    }
    case CEscapeType::PASTE: {
      const CEscapeDataStr *escs = dynamic_cast<const CEscapeDataStr *>(esc);

      paste(escs->str);

      break;
    }
    case CEscapeType::TRACE: {
      const CEscapeDataBool *escb = dynamic_cast<const CEscapeDataBool *>(esc);

      setTrace(escb->b);

      break;
    }
    case CEscapeType::DEBUG: {
      const CEscapeDataBool *escb = dynamic_cast<const CEscapeDataBool *>(esc);

      setDebug(escb->b);

      break;
    }
    // Device Control String
    //  DCS Ps; Ps| Pt ST
    //    User-Defined Keys (DECUDK).  The first parameter:
    //      Ps = 0  -> Clear all UDK definitions before starting
    //    (default).
    //      Ps = 1  -> Erase Below (default).
    //    The second parameter:
    //      Ps = 0  <- Lock the keys (default).
    //      Ps = 1  <- Do not lock.
    //    The third parameter is a ';'-separated list of strings denoting the
    //    key-code separated by a '/' from the hex-encoded key value. The key codes
    //    correspond to the DEC function-key codes (e.g., F6=17).
    case CEscapeType::DCS: {
      const CEscapeDataDCS *escd = dynamic_cast<const CEscapeDataDCS *>(esc);

      std::string kstr;

#if 0
      if (escd->key != CKEY_TYPE_NUL)
        kstr = CKeyEvent::getEscapeText(escd->key);
#endif

      std::string reply;

      if (kstr != "")
        reply = "\033P1+r" + escd->value + "=" + kstr + "\033\\";
      else
        reply = "\033P0+r" + escd->value + "\033\\";

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

  int num_rows = getNumDataRows();

  if (getDataPos().y + num >= num_rows)
    num = num_rows - 1 - getDataPos().y;

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

  int num_cols = getNumDataCols();

  if (getDataPos().x + num >= num_cols)
    num = num_cols - 1 - getDataPos().x;

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

  setWrapChar(false);

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
  setWrapChar(false);

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

  // Erase in Display (DECSED).
  //   Ps = 0  -> Selective Erase Below (default).
  //   Ps = 1  -> Selective Erase Above.
  //   Ps = 2  -> Selective Erase All.
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

  // Erase in Line (DECSEL).
  //   Ps = 0  -> Selective Erase to Right (default).
  //   Ps = 1  -> Selective Erase to Left.
  //   Ps = 2  -> Selective Erase All.
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

// Erase num Character(s)
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

// Repeat the preceding graphic character num times
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

// Tab Clear
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

  // Print screen (default)
  if      (num == 0) {
    printScreen();
  }
  // Turn off printer controller mode
  else if (num == 4) {
    stopPrintLog();
  }
  // Turn on printer controller mode.
  else if (num == 5) {
    startPrintLog();
  }
  // HTML screen dump.
  else if (num == 10) {
    // TODO
  }
  // SVG screen dump.
  else if (num == 11) {
    // TODO
  }
  else
    UNHANDLED1("MC", num)
}

void
CEscapeHandler::
escapeDECMC(int num)
{
  logTrace("<DECMC;" + CStrUtil::toString(num) + ">");

  if      (num == 0) {
    printScreen();
  }
  // Print line containing cursor
  else if (num == 1) {
    printLine();
  }
  // Turn off autoprint mode
  else if (num == 4) {
    stopPrintLog();
  }
  // Turn on autoprint mode
  else if (num == 5) {
    startPrintLog();
  }
  // Print composed display, ignores DECPEX
  else if (num == 10) {
    printComposedScreen();
  }
  // Print all pages
  else if (num == 11) {
    printAllPages();
  }
  else
    UNHANDLED1("DECMC", num)
}

void
CEscapeHandler::
escapeDECSET(int num)
{
  logTrace("<DECSET;" + CStrUtil::toString(num) + ">");

  switch (num) {
    // Application Cursor Keys (DECCKM)
    case 1: {
      escapeDECCKM(true);
      break;
    }
    // Designate USASCII for character sets G0-G3 (DECANM), and set VT100 mode.
    case 2: {
      escapeDECANM(true);
      break;
    }
    // 132 Column Mode (DECCOLM).
    case 3: {
      escapeDECCOLM(true);
      break;
    }
    // Smooth (Slow) Scroll (DECSCLM).
    case 4: {
      escapeDECSCLM(true);
      break;
    }
    // Reverse Video (DECSCNM).
    case 5: {
      escapeDECSCNM(true);
      break;
    }
    // Origin Mode (DECOM).
    case 6: {
      escapeDECOM(true);
      break;
    }
    // Wraparound Mode (DECAWM).
    case 7: {
      escapeDECAWM(true);
      break;
    }
    // Auto-repeat Keys (DECARM).
    case 8: {
      escapeDECARM(true);
      break;
    }
    // Send Mouse X & Y on button press. This is the X10 xterm mouse protocol.
    case 9: {
      setSendMousePress(true);
      break;
    }
    // Show toolbar (rxvt)
    case 10: {
      setShowToolBar(true);
      break;
    }
    // Start Blinking Cursor (att610)
    case 12: {
      setCursorBlink(true);
      break;
    }
    // Print form feed (DECPFF)
    case 18: {
      escapeDECPFF(true);
      break;
    }
    // Set print extent to full screen (DECPEX)
    case 19: {
      escapeDECPEX(true);
      break;
    }
    // Show Cursor (DECTCEM)
    case 25: {
      escapeDECTCEM(true);
      break;
    }
    // Show scrollbar (rxvt)
    case 30: {
      setShowScrollBar(true);
      break;
    }
    // Enable font-shifting functions - rxvt
    case 35: {
      // TODO
      break;
    }
    // Enter Tektronix Mode (DECTEK)
    case 38: {
      escapeDECTEK(true);
      break;
    }
    // Allow 80 <-> 132
    case 40: {
      setAllow80To132(true);
      break;
    }
    // enable more fix
    case 41: {
      // TODO
      break;
    }
    // Enable National Replacement Character sets
    case 42: {
      escapeDECNRCM(true);
      break;
    }
    // Turn on Margin Bell
    case 44: {
      // TODO
      break;
    }
    // Reverse-wraparound Mode
    case 45: {
      setReverseWrap(true);
      break;
    }
    // Start Logging
    case 46: {
      // TODO
      break;
    }
    // Use Alternate Screen Buffer
    case 47: {
      setAlternative(true);
      break;
    }
    // Application keypad (DECNKM)
    case 66: {
      escapeDECNKM(true);
      break;
    }
    // Backarrow key sends backspace (DECBKM)
    case 67: {
      escapeDECBKM(true);
      break;
    }
    // Enable left and right margin mode (DECLRMM), VT420 and up
    case 69: {
      // TODO
      break;
    }
    // Do not clear screen when DECCOLM is set/reset (DECNCSM), VT510 and up.
    case 95: {
      // TODO
      break;
    }
    // Send Mouse X & Y on button press and release. This is the X11 xterm mouse protocol
    case 1000: {
      setSendMousePress  (true);
      setSendMouseRelease(true);
      break;
    }
    // Use Hilite Mouse Tracking
    case 1001: {
      // TODO
      break;
    }
    // Use Cell Motion Mouse Tracking.
    case 1002: {
      setSendMousePress  (true);
      setSendMouseRelease(true);
      break;
    }
    // Use All Motion Mouse Tracking.
    case 1003: {
      setSendMousePress  (true);
      setSendMouseRelease(true);
      setSendMouseMotion (true);
      break;
    }
    // Send FocusIn/FocusOut events.
    case 1004: {
      setSendFocusInOut(true);
      break;
    }
    // Enable UTF-8 Mouse Mode.
    case 1005: {
      // TODO
      break;
    }
    // Enable SGR Mouse Mode.
    case 1006: {
      // TODO
      break;
    }
    // Enable Alternate Scroll Mode
    case 1007: {
      // TODO
      break;
    }
    // Scroll to bottom on tty output (rxvt)
    case 1010: {
      setScrollBottomOnTty(true);
      break;
    }
    // Scroll to bottom on key press (rxvt)
    case 1011: {
      setScrollBottomOnKey(true);
      break;
    }
    // Enable urxvt Mouse Mode
    case 1015: {
      // TODO
      break;
    }
    // Interpret "meta" key, sets eighth bit.
    case 1034: {
      // TODO
      break;
    }
    // Enable special modifiers for Alt and NumLock keys
    case 1035: {
      // TODO
      break;
    }
    // Send ESC when Meta modifies a key
    case 1036: {
      // TODO
      break;
    }
    // Send DEL from the editing-keypad Delete key
    case 1037: {
      // TODO
      break;
    }
    // Send ESC  when Alt modifies a key.
    case 1039: {
      // TODO
      break;
    }
    // Keep selection even if not highlighted.
    case 1040: {
      // TODO
      break;
    }
    // Use the CLIPBOARD selection.
    case 1041: {
      // TODO
      break;
    }
    // Enable Urgency window manager hint when Control-G is received
    case 1042: {
      // TODO
      break;
    }
    // Enable raising of the window when Control-G is received.
    case 1043: {
      // TODO
      break;
    }
    // Reuse the most recent data copied to CLIPBOARD
    case 1044: {
      // TODO
      break;
    }
    // Use Alternate Screen Buffer.
    case 1047: {
      setAlternative(true);
      break;
    }
    // Save cursor as in DECSC
    case 1048: {
      saveCursor();
      break;
    }
    // Save cursor as in DECSC and use Alternate Screen Buffer, clearing it first
    case 1049: {
      saveCursor();
      setAlternative(true);
      clearScreen();
      break;
    }
    // Set terminfo/termcap function-key mode.
    case 1050: {
      // TODO
      break;
    }
    // Set Sun function-key mode
    case 1051: {
      // TODO
      break;
    }
    // Set HP function-key mode
    case 1052: {
      // TODO
      break;
    }
    // et SCO function-key mode.
    case 1053: {
      // TODO
      break;
    }
    // Set legacy keyboard emulation (X11R6)
    case 1060: {
      // TODO
      break;
    }
    // Set VT220 keyboard emulation
    case 1061: {
      // TODO
      break;
    }
    // Set bracketed paste mode
    case 2004: {
      // TODO
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
    // Ps = 8 0  -> Sixel scrolling
    case 80: {
      UNHANDLED1("SM", num)
      break;
    }
    // Ps = 1 0 7 0  -> use private color registers for each graphic
    case 1070: {
      UNHANDLED1("SM", num)
      break;
    }
    // Sixel scrolling leaves cursor to right of graphic
    case 8452: {
      UNHANDLED1("SM", num)
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
    // Normal Cursor Keys (DECCKM)
    case 1: {
      escapeDECCKM(false);
      break;
    }
    // Designate VT52 mode (DECANM)
    case 2: {
      escapeDECANM(false);
      break;
    }
    // 80 Column Mode (DECCOLM)
    case 3: {
      escapeDECCOLM(false);
      break;
    }
    // Jump (Fast) Scroll (DECSCLM)
    case 4: {
      escapeDECSCLM(false);
      break;
    }
    // Normal Video (DECSCNM)
    case 5: {
      escapeDECSCNM(false);
      break;
    }
    // Normal Cursor Mode (DECOM)
    case 6: {
      escapeDECOM(false);
      break;
    }
    // No Wraparound Mode (DECAWM)
    case 7: {
      escapeDECAWM(false);
      break;
    }
    // No Auto-repeat Keys (DECARM)
    case 8: {
      escapeDECARM(false);
      break;
    }
    // Don't send Mouse X & Y on button press
    case 9: {
      setSendMousePress(false);
      break;
    }
    // Hide toolbar (rxvt)
    case 10: {
      setShowToolBar(false);
      break;
    }
    // Stop Blinking Cursor (att610)
    case 12: {
      setCursorBlink(false);
      break;
    }
    // Don't print form feed (DECPFF)
    case 18: {
      escapeDECPFF(false);
      break;
    }
    // Limit print to scrolling region (DECPEX)
    case 19: {
      escapeDECPEX(false);
      break;
    }
    // Hide Cursor (DECTCEM)
    case 25: {
      escapeDECTCEM(false);
      break;
    }
    // Don't show scrollbar (rxvt)
    case 30: {
      setShowScrollBar(false);
      break;
    }
    // Disable font-shifting functions (rxvt)
    case 35: {
      // TODO
      break;
    }
    // Leave Tektronix Mode (DECTEK)
    case 38: {
      escapeDECTEK(false);
      break;
    }
    // Disallow 80 <-> 132
    case 40: {
      setAllow80To132(false);
      break;
    }
    // No more(1) fix
    case 41: {
      // TODO
      break;
    }
    // Disable National Replacement Character sets (DECNRCM)
    case 42: {
      escapeDECNRCM(false);
      break;
    }
    // Turn Off Margin Bell
    case 44: {
      // TODO
      break;
    }
    // No Reverse-wraparound Mode
    case 45: {
      setReverseWrap(false);
      break;
    }
    // Stop Logging
    case 46: {
      // TODO
      break;
    }
    // Use Normal Screen Buffer
    case 47: {
      setAlternative(false);
      break;
    }
    // Numeric keypad (DECNKM)
    case 66: {
      escapeDECNKM(false);
      break;
    }
    // Backarrow key sends delete (DECBKM)
    case 67: {
      escapeDECBKM(false);
      break;
    }
    // Disable left and right margin mode (DECLRMM), VT420 and up
    case 95: {
      // TODO
      break;
    }
    // Don't send Mouse X & Y on button press and release
    case 1000: {
      setSendMousePress  (false);
      setSendMouseRelease(false);
      break;
    }
    // Don't use Hilite Mouse Tracking
    case 1001: {
      // TODO
      break;
    }
    // Don't use Cell Motion Mouse Tracking
    case 1002: {
      setSendMousePress  (false);
      setSendMouseRelease(false);
      break;
    }
    // Don't use All Motion Mouse Tracking
    case 1003: {
      setSendMousePress  (false);
      setSendMouseRelease(false);
      setSendMouseMotion (false);
      break;
    }
    // Don't send FocusIn/FocusOut events
    case 1004: {
      setSendFocusInOut(false);
      break;
    }
    // Disable UTF-8 Mouse Mode
    case 1005: {
      // TODO
      break;
    }
    // Disable SGR Mouse Mode
    case 1006: {
      // TODO
      break;
    }
    // Disable Alternate Scroll Mode
    case 1007: {
      // TODO
      break;
    }
    // Don't scroll to bottom on tty output (rxvt)
    case 1010: {
      setScrollBottomOnTty(false);
      break;
    }
    // Don't scroll to bottom on key press (rxvt)
    case 1011: {
      setScrollBottomOnKey(false);
      break;
    }
    // Disable urxvt Mouse Mode
    case 1015: {
      // TODO
      break;
    }
    // Don't interpret "meta" key
    case 1034: {
      // TODO
      break;
    }
    // Disable special modifiers for Alt and NumLock keys
    case 1035: {
      // TODO
      break;
    }
    // Don't send ESC  when Meta modifies a key
    case 1036: {
      // TODO
      break;
    }
    // Send VT220 Remove from the editing-keypad Delete key
    case 1037: {
      // TODO
      break;
    }
    // Don't send ESC  when Alt modifies a key.
    case 1039: {
      // TODO
      break;
    }
    // Do not keep selection when not highlighted.
    case 1040: {
      // TODO
      break;
    }
    // Use the PRIMARY selection
    case 1041: {
      // TODO
      break;
    }
    // Disable Urgency window manager hint when Control-G is received
    case 1042: {
      // TODO
      break;
    }
    // Disable raising of the window when Control-G is received
    case 1043: {
      // TODO
      break;
    }
    // Use Normal Screen Buffer, clearing screen first if in the Alternate Screen
    case 1047: {
      setAlternative(false);
      break;
    }
    // Restore cursor as in DECRC
    case 1048: {
      restoreCursor();
      break;
    }
    // Use Normal Screen Buffer and restore cursor as in DECRC
    case 1049: {
      setAlternative(false);
      restoreCursor();
      break;
    }
    // Reset terminfo/termcap function-key mode
    case 1050: {
      // TODO
      break;
    }
    // Reset Sun function-key mode
    case 1051: {
      // TODO
      break;
    }
    // Reset HP function-key mode
    case 1052: {
      // TODO
      break;
    }
    // Reset SCO function-key mode
    case 1053: {
      // TODO
      break;
    }
    // Reset legacy keyboard emulation (X11R6)
    case 1060: {
      // TODO
      break;
    }
    // Reset keyboard emulation to Sun/PC style
    case 1061: {
      // TODO
      break;
    }
    // Reset bracketed paste mode
    case 2004: {
      // TODO
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
    // Keyboard Action Mode (AM)
    case 2: {
      escapeAM(false);
      break;
    }
    // Replace Mode (IRM)
    case 4: {
      escapeIRM(false);
      break;
    }
    // Send/receive (SRM)
    case 12: {
      escapeSRM(false);
      break;
    }
    // Normal Linefeed (LNM)
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
  logTrace("<DECSTBM;" + CStrUtil::toString(top) + ";" + CStrUtil::toString(bottom) + ">");

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

  processString("\033[?1;2c");
}

void
CEscapeHandler::
escapeDA1(int *, int nn)
{
  logTrace("<DA1>");

  if (nn == 0 || nn == 1)
    processString("\033[?1;2c");
}

void
CEscapeHandler::
escapeDA2(int *, int nn)
{
  logTrace("<DA2>");

  // vt220=1,ver=1000,rom_ver=0
  if (nn == 0 || nn == 1)
    processString("\033[>0;241;0c");

  // TODO: Ps = 3 -> ReGIS graphics
  // TODO: Ps = 4 -> Sixel graphics
}

void
CEscapeHandler::
escapeDECDSR(int mode)
{
  logTrace("<DECDSR;" + CStrUtil::toString(mode) + ">");

  if      (mode == 5) {
    processString("\033[?0n");
  }
  else if (mode == 6) {
    int row, col;

    getDispPos(&row, &col);

    std::string str = "\033[?" + CStrUtil::toString(row + 1) + ";" +
                                 CStrUtil::toString(col + 1) + "R";

    processString(str.c_str());
  }
  else if (mode == 15)
    processString("\033[?10n");
  else if (mode == 25)
    processString("\033[?20n");
  else if (mode == 26)
    processString("\033[?27;1;0;0n");
  else if (mode == 53)
    processString("\033[?53n");
}

void
CEscapeHandler::
escapeDSR(int num)
{
  logTrace("<DSR;" + CStrUtil::toString(num) + ">");

  if      (num == 5)
    processString("\033[0n");
  else if (num == 6) {
    int row, col;

    getDispPos(&row, &col);

    std::string str = "\033[" + CStrUtil::toString(row + 1) + ";" +
                                CStrUtil::toString(col + 1) + "R";

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
    case 0: {
      break;
    }
    // De-iconify window
    case 1: {
      if (window)
        window->deiconize();
      else
        deiconizeWindow();

      break;
    }
    // Iconify window
    case 2: {
      if (window)
        window->iconize();
      else
        iconizeWindow();

      break;
    }
    // Move window to [x, y]
    case 3: {
      if (window)
        window->move(num[1], num[2]);
      else
        moveWindow(num[1], num[2]);

      break;
    }
    // Resize the window to height and width
    // Omitted parameters reuse the current height or width.
    // Zero parameters use the display's height or width.
    case 4: {
      if (window)
        window->resize(num[2], num[1]);
      else
        resizeWindow(num[2], num[1]);

      break;
    }
    // Raise the window to the front of the stacking order
    case 5: {
      if (window)
        window->raise();
      else
        raiseWindow();

      break;
    }
    // Lower the window to the bottom of the stacking order.
    case 6: {
      if (window)
        window->lower();
      else
        lowerWindow();

      break;
    }
    // Refresh the window
    case 7: {
      if (window)
        window->expose();
      else
        exposeWindow();

      break;
    }
    // Resize the text area to given height and width in characters.
    // Omitted parameters reuse the current height or width.
    // Zero parameters use the display's height or width.
    case 8: {
      int w, h;

      charsToPixels(num[2], num[1], &w, &h);

      if (window)
        window->resize(w, h);
      else
        resizeWindow(w, h);

      break;
    }
    // maximize window
    case 9: {
      // Restore maximized window
      if      (num[1] == 0) {
        demaximizeWindow();
      }
      // Maximize window
      else if (num[1] == 1) {
        maximizeWindow();
      }
      // Maximize window vertically
      else if (num[1] == 2) {
      }
      // Maximize window horizontally
      else if (num[1] == 2) {
      }

      break;
    }
    // full screen
    case 10: {
      // Undo full-screen mode.
      if      (num[1] == 0) {
      }
      // Change to full-screen.
      else if (num[1] == 1) {
      }
      // Toggle full-screen.
      else if (num[1] == 2) {
      }

      break;
    }
    // Report window state
    // If the window is open (non-iconified), it returns CSI 1 t .
    // If the window is iconified, it returns CSI 2 t
    case 11: {
      bool mapped = (window ? window->isMapped() : isWindowMapped());

      processString(mapped ? "\033[1t" : "\033[2t");

      break;
    }
    case 12: {
      break;
    }
    // Report window position
    case 13: {
      int x, y;

      if (window)
        window->getPosition(&x, &y);
      else
        getWindowPosition(&x, &y);

      std::string str = "\033[3;" + CStrUtil::toString(x) + ";" + CStrUtil::toString(y) + "t";

      processString(str.c_str());

      break;
    }
    // Report window in pixels
    case 14: {
      uint width, height;

      if (window)
        window->getSize(&width, &height);
      else
        getWindowSize(&width, &height);

      std::string str = "\033[4;" + CStrUtil::toString(height) + ";" +
                                    CStrUtil::toString(width ) + "t";

      processString(str.c_str());

      break;
    }
    case 15: {
      break;
    }
    case 16: {
      break;
    }
    case 17: {
      break;
    }
    // Report the size of the text area in characters
    case 18: {
      std::string str = "\033[8;" + CStrUtil::toString(getNumDispRows()) + ";" +
                                    CStrUtil::toString(getNumDispCols()) + "t";

      processString(str.c_str());

      break;
    }
    // Report the size of the screen in characters
    case 19: {
      uint width, height;

      if (window)
        window->getScreenSize(&width, &height);
      else
        getScreenSize(&width, &height);

      int rows, cols;

      pixelsToChars(width, height, &cols, &rows);

      std::string str = "\033[9;" + CStrUtil::toString(rows) + ";" + CStrUtil::toString(cols) + "t";

      processString(str.c_str());

      break;
    }
    // Report window's icon label. Result is OSC L label ST
    case 20: {
      std::string name;

      if (window)
        window->getIconTitle(name);
      else
        getIconTitle(name);

      std::string str = "\033]L" + name + "";

      processString(str.c_str());

      break;
    }
    // Report window's title. Result is OSC l label ST
    case 21: {
      std::string name;

      if (window)
        window->getWindowTitle(name);
      else
        getWindowTitle(name);

      std::string str = "\033]1" + name + "";

      processString(str.c_str());

      break;
    }
    // Save icon and window title on stack
    case 22: {
      break;
    }
    // Restore icon and window title from stack
    case 23: {
      break;
    }
    // Resize to Ps lines (DECSLPP)
    case 24:
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
    processString("\033[2;1;1;128;128;1;0x");
  else if (num == 1)
    processString("\033[3;1;1;128;128;1;0x");
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
  setControl8Bit(false);
}

void
CEscapeHandler::
escapeS8C1T()
{
  setControl8Bit(true);
}

void
CEscapeHandler::
escapeANSIConformance(int mode)
{
  UNHANDLED1("ANSI Conformance", mode)
}

// DEC Double Height Line
void
CEscapeHandler::
escapeDECDHL(CEscapeDataDECDHL::Pos pos)
{
  int y = getDataPos().y;

  if (pos == CEscapeDataDECDHL::Pos::TOP)
    setLineHeightStyle(y, CCellLineHeightStyle::DOUBLE_TOP);
  else
    setLineHeightStyle(y, CCellLineHeightStyle::DOUBLE_BOTTOM);

  setLineWidthStyle(y, CCellLineWidthStyle::DOUBLE);
}

// DEC Single Width Line
void
CEscapeHandler::
escapeDECSWL()
{
  int y = getDataPos().y;

  setLineWidthStyle (y, CCellLineWidthStyle ::SINGLE);
  setLineHeightStyle(y, CCellLineHeightStyle::SINGLE);
}

// DEC Double Width Line
void
CEscapeHandler::
escapeDECDWL()
{
  int y = getDataPos().y;

  setLineWidthStyle (y, CCellLineWidthStyle ::DOUBLE);
  setLineHeightStyle(y, CCellLineHeightStyle::SINGLE);
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
  // Designate G0 Character Set (ISO 2022, VT100).
  // Final character C for designating 94-character sets. In this list, 0, A and B
  // apply to VT100 and up, the remainder to VT220 and up. The VT220 character sets,
  // together with the Portuguese character set are activated by the National
  // Replacement Character controls. The A is a special case, since it is also
  // activated by the VT300-control for British Latin-1 separately from the
  // National Replacement Character controls.
  //   C = 0  -> DEC Special Character and Line Drawing Set.
  //   C = <  -> DEC Supplementary (VT200).
  //   C = % 5  -> DEC Supplementary Graphics (VT300).
  //   C = >  -> DEC Technical (VT300).
  //   C = A  -> United Kingdom (UK).
  //   C = B  -> United States (USASCII).
  //   C = 4  -> Dutch.
  //   C = C  or 5  -> Finnish.
  //   C = R  or f  -> French.
  //   C = Q  or 9  -> French Canadian (VT200, VT300).
  //   C = K  -> German.
  //   C = Y  -> Italian.
  //   C = ` , E  or 6  -> Norwegian/Danish.
  //   C = % 6  -> Portuguese (VT300).
  //   C = Z  -> Spanish.
  //   C = H  or 7  -> Swedish.
  //   C = =  -> Swiss.
  charset_.setSetType(0, c);
}

void
CEscapeHandler::
escapeDesignateG1(char c)
{
  charset_.setSetType(1, c);
}

void
CEscapeHandler::
escapeDesignateG2(char c)
{
  charset_.setSetType(2, c);
}

void
CEscapeHandler::
escapeDesignateG3(char c)
{
  charset_.setSetType(3, c);
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
  int num_rows = getNumDataRows();

  if (getDataPos().y >= num_rows - 1)
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
  int num_rows = getNumDataRows();

  if (getDataPos().y >= num_rows - 1)
    scrollUp();
  else
    incOutputRow();

  setDispCol(0);
}

void
CEscapeHandler::
escapeSSA()
{
  UNHANDLED("SSA")
}

void
CEscapeHandler::
escapeESA()
{
  UNHANDLED("ESA")
}

// Tab Set
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
  // Start Guarded Area. Writes an SSA-symbol to the Page
  // These symbols are characters, just like any of the other graphic characters.
  // They may be erased with erase controls, moved with edit controls, and are included
  // in transfers exactly as any other character. When transferred to the printer,
  // they are sent as a Space code (2/0) (not as ESC F or ESC G).
  UNHANDLED("SPA")
}

void
CEscapeHandler::
escapeEPA()
{
  // Start Guarded Area. Writes an ESA-symbol to the Page
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
  // Set conformance level (DECSCL).
  //   Valid values for the first parameter:
  //     Ps = 6 1  -> VT100.
  //     Ps = 6 2  -> VT200.
  //     Ps = 6 3  -> VT300.
  //   Valid values for the second parameter:
  //     Ps = 0  -> 8-bit controls.
  //     Ps = 1  -> 7-bit controls (always set for VT100).
  //     Ps = 2  -> 8-bit controls.
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
  if (set) {
    // Designate USASCII for character sets G0-G3 (DECANM), and set VT100 mode.
    setAnsiVT52Mode(false);
  }
  else {
    // Designate VT52 mode (DECANM)
    setAnsiVT52Mode(true);

    set4014(false);
  }
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
  int num_cols = getNumDataCols();

  setDispSize(num_rows, num_cols);
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
// When Not Set:
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
escapeDECTEK(bool set)
{
  set4014(set);

  if (set)
    setAnsiVT52Mode(false);
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
escapeOSC(int num, const std::string &str1)
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
      std::string::size_type pos = str1.find('=');

      std::string name, value;

      if (pos == std::string::npos)
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

      std::vector<std::string> words;

      CStrUtil::addFields(str1, words, ";");

      uint num_words = words.size();

      for (uint i = 0; i < num_words; ++i) {
        uint pos = i + start;

        switch (pos) {
          case 0: // Text Foreground Color
            setColor(CEscapeColor::FG, CRGBName::toRGBA(words[i]));
            break;
          case 1: // Text Background Color
            setColor(CEscapeColor::BG, CRGBName::toRGBA(words[i]));
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
escape4014(const CEscapeDataTek4014 *esc)
{
  if      (esc->mode == CEscapeDataTek4014::Mode::VT100) {
    // switch to VT100 mode
    escapeDECTEK(false);
  }
  else if (esc->mode == CEscapeDataTek4014::Mode::STATUS) {
  }
  else if (esc->mode == CEscapeDataTek4014::Mode::CLEAR) {
    // erase screen, selects alpha, move cursor home, sets margin 1, clears bypass condition
    clear4014();
  }
  else if (esc->mode == CEscapeDataTek4014::Mode::COPY) {
  }
  else if (esc->mode == CEscapeDataTek4014::Mode::BYPASS) {
  }
  else if (esc->mode == CEscapeDataTek4014::Mode::GIN) {
    set4014GIN(true);
  }
  else if (esc->mode == CEscapeDataTek4014::Mode::POINT_PLOT) {
  }
  else if (esc->mode == CEscapeDataTek4014::Mode::CHAR_SET) {
    state_.set4014CharSet(esc->value);
  }
  else if (esc->mode == CEscapeDataTek4014::Mode::LINE_STYLE) {
    state_.clear4014Points();

    state_.set4014LineStyle(esc->lineStyle);
  }
}

void
CEscapeHandler::
incOutputRow()
{
  setWrapChar(false);

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
  setWrapChar(false);

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
  setWrapChar(false);

  int row, col;

  getDispPos(&row, &col);

  ++col;

  setDispPos(row, col);
}

void
CEscapeHandler::
decOutputCol()
{
  setWrapChar(false);

  int row, col;

  getDispPos(&row, &col);

  if (col > 0)
    --col;

  setDispPos(row, col);
}

//----------

void
CEscapeHandler::
setState(const CEscapeState &state)
{
  state_ = state;

  notifyStateChange();
}

void
CEscapeHandler::
resetState()
{
  state_.reset();

  notifyStateChange();
}

void
CEscapeHandler::
setInverseVideo(bool flag)
{
  state_.setInverseVideo(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setSendMousePress(bool flag)
{
  state_.setSendMousePress(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setSendMouseRelease(bool flag)
{
  state_.setSendMouseRelease(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setSendMouseMotion(bool flag)
{
  state_.setSendMouseMotion(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setSendFocusInOut(bool flag)
{
  state_.setSendFocusInOut(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setScrollBottomOnKey(bool flag)
{
  state_.setScrollBottomOnKey(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setScrollBottomOnTty(bool flag)
{
  state_.setScrollBottomOnTty(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setApplicationCursorKeys(bool flag)
{
  state_.setApplicationCursorKeys(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setInsertMode(bool flag)
{
  state_.setInsertMode(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setLineWrap(bool flag)
{
  state_.setLineWrap(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setAnsiVT52Mode(bool flag)
{
  state_.setAnsiVT52Mode(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setTek4014(bool flag)
{
  state_.setTek4014(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setKeyPadMode(bool flag)
{
  state_.setKeyPadMode(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setLfNlMode(bool flag)
{
  state_.setLfNlMode(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setFfNpMode(bool flag)
{
  state_.setFfNpMode(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setSmoothScroll(bool flag)
{
  state_.setSmoothScroll(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setOriginMode(bool flag)
{
  state_.setOriginMode(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setAutoRepeat(bool flag)
{
  state_.setAutoRepeat(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setCursorVisible(bool flag)
{
  state_.setCursorVisible(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setCursorBlink(bool blink)
{
  state_.setCursorBlink(blink);

  notifyStateChange();
}

void
CEscapeHandler::
setReverseWrap(bool wrap)
{
  state_.setReverseWrap(wrap);

  notifyStateChange();
}

void
CEscapeHandler::
setAllow80To132(bool flag)
{
  state_.setAllow80To132(flag);

  notifyStateChange();
}

void
CEscapeHandler::
setControl8Bit(bool flag)
{
  state_.setControl8Bit(flag);

  notifyStateChange();
}

void
CEscapeHandler::
set4014LineStyle(CEscapeLineStyle style)
{
  state_.set4014LineStyle(style);

  notifyStateChange();
}

//----------

void
CEscapeHandler::
nextTab()
{
  setWrapChar(false);

  uint rows, cols;

  getDispSize(&rows, &cols);

  tabs_.setLength(cols);

  int row, col;

  getDispPos(&row, &col);

  int col1;

  if (tabs_.nextPos(col, &col1))
    setDispPos(row, col1);
  else
    setDispPos(row, cols - 1);
}

void
CEscapeHandler::
prevTab()
{
  setWrapChar(false);

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
  setWrapChar(false);

  setDispPos(save_cursor_.row, save_cursor_.col);

  setState(save_cursor_.state);

  setStyle(save_cursor_.style);

  charset_ = save_cursor_.charset;
}

//----------

const CRGBA &
CEscapeHandler::
getColor(CEscapeColor color) const
{
  return CEscapeColorsInst->getColor(color);
}

void
CEscapeHandler::
setColor(CEscapeColor color, const CRGBA &rgba)
{
  CEscapeColorsInst->setColor(color, rgba);
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

  str[0] = '\033';
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

  str[0] = '\033';
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
send4014MousePress(int button, int x, int y)
{
  static char str[6];

  if      (button == 1)
    str[0] = char(uchar(0x80) | uchar('l'));
  else if (button == 2)
    str[0] = char(uchar(0x80) | uchar('m'));
  else if (button == 3)
    str[0] = char(uchar(0x80) | uchar('r'));
  else
    str[0] = char(uchar(0x80) | uchar('?'));

  str[1] = ((x >> 5) & 0x1F);
  str[2] = ( x       & 0x1F);
  str[3] = ((y >> 5) & 0x1F);
  str[4] = ( y       & 0x1F);
  str[5] = '\0';

  processString(str);
}

void
CEscapeHandler::
logTrace(char c) const
{
  logTrace(std::string(&c, 1));
}
