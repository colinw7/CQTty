#include <CEscape.h>
#include <CEscapeParse.h>
#include <CStrUtil.h>
#include <COSPty.h>
#include <COSFile.h>
#include <COSRead.h>
#include <CFile.h>
#include <CCommand.h>
#include <CRGBA.h>
#include <cstring>

class CEscapeTest {
 public:
  CEscapeTest() { }

  bool parseEscapeFile(const std::string &filename);
  bool parseExecFile  (const std::string &filename);

  std::string runCommand(const std::string &cmd);

  bool parseEscape(const std::string &str, std::vector<std::string> &args);
};

void test1(int pass=0);
void test2(int pass=0);
void test3();
void test4();

void colors();

bool
set_tty_crmod()
{
  struct termios t;

  if (tcgetattr(0, &t) < 0)
    return false;

  struct termios t1 = t;

  t1.c_iflag |= ICRNL | IXON;
  t1.c_lflag |= ICANON;

  memcpy(t1.c_cc, t.c_cc, sizeof(t1.c_cc));

  if (tcsetattr(0, TCSAFLUSH, &t1) < 0)
    return false;

  return true;
}

int
main(int argc, char **argv)
{
  CEscapeTest test;

  std::vector<std::string> words;

  bool run_cmd   = false;
  bool exec_file = false;
  bool parse     = false;
  bool do_test1  = false;
  bool do_test2  = false;
  bool do_test3  = false;
  bool do_test4  = false;
  bool do_colors = false;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (argv[i][0] == '-') {
      std::string arg1 = arg.substr(1);

      if      (arg1 == "-")
        words.push_back(arg);
      else if (arg1 == "run")
        run_cmd = true;
      else if (arg1 == "exec")
        exec_file = true;
      else if (arg1 == "parse")
        parse = true;
      else if (arg1 == "test1")
        do_test1 = true;
      else if (arg1 == "test2")
        do_test2 = true;
      else if (arg1 == "test3")
        do_test3 = true;
      else if (arg1 == "test4")
        do_test4 = true;
      else if (arg1 == "colors")
        do_colors = true;
    }
    else {
      words.push_back(arg);
    }
  }

  //---

  if (do_colors) {
    colors();

    exit(0);
  }

  //---

  if (parse) {
    if (words.empty()) {
      std::cerr << "Missing filenames for -parse" << std::endl;
      exit(1);
    }

    for (const auto &word : words)
      test.parseEscapeFile(word);

    exit(0);
  }

  //---

  if (exec_file) {
    if (words.empty()) {
      std::cerr << "Missing filenames for -exec" << std::endl;
      exit(1);
    }

    for (const auto &word : words)
      test.parseExecFile(word);

    exit(0);
  }

  //---

  if (do_test1) test1();
  if (do_test2) test2();
  if (do_test3) test3();
  if (do_test4) test4();

  for (const auto &word : words) {
    std::string str = CEscape::stringToEscape(word);

    if (run_cmd) {
      std::string res = test.runCommand(str);

      CEscape::WindowOp        op;
      std::string              arg1, arg2;
      std::vector<std::string> args;

      if      (CEscape::decodeWindowOp(res, op, arg1, arg2)) {
        std::cout << (int) op << ":" << arg1 << ":" << arg2 << std::endl;
      }
      else if (test.parseEscape(res, args)) {
        std::cout << CStrUtil::toString(args, ";") << std::endl;
      }
      else {
        uint len = res.size();

        for (uint i = 0; i < len; ++i) {
          char c = res[i];

          if (c >= 32 && c < 0x7f)
            std::cout << c;
          else
            std::cout << int(c);
        }

        std::cout << std::endl;
      }
    }
    else {
      std::cout << str;
    }
  }

  return true;
}

bool
CEscapeTest::
parseEscapeFile(const std::string &filename)
{
  CFile *file = nullptr;

  if (filename == "--")
    file = new CFile(stdin);
  else {
    file = new CFile(filename);

    if (! file->isRegular()) {
      std::cerr << "Invalid filename '" << filename << "'" << std::endl;

      delete file;

      return false;
    }
  }

  //---

  std::string data;

  int c;

  while ((c = file->getC()) != EOF)
    data += char(c);

  file->close();

  delete file;

  //---

  CEscapeDebugParse parse;

  parse.addOutputChars(data);

  return true;
}

bool
CEscapeTest::
parseExecFile(const std::string &filename)
{
  CFile *file = nullptr;

  if (filename == "--")
    file = new CFile(stdin);
  else {
    file = new CFile(filename);

    if (! file->isRegular()) {
      std::cerr << "Invalid filename '" << filename << "'" << std::endl;

      delete file;

      return false;
    }
  }

  //---

  std::string str;

  std::string line;

  while (file->readLine(line)) {
    if (line[0] == '#')
      continue;

    if (line[0] == '@') {
      str += line.substr(1);

      continue;
    }

    std::vector<std::string> words1;

    CStrUtil::toWords(line, words1);

    for (const auto &word1 : words1)
      str += CEscape::stringToEscape(word1);
  }

  file->close();

  delete file;

  //---

  std::cout << str;

  return true;
}

std::string
CEscapeTest::
runCommand(const std::string &cmd)
{
  COSPty pty;

  pty.set_raw();

  COSRead::write(0, cmd);

  std::string result;

  if (COSRead::wait_read(0, 1, 0))
    COSRead::read(0, result);

  pty.reset_raw();

  return result;
}

bool
CEscapeTest::
parseEscape(const std::string &str, std::vector<std::string> &args)
{
  int len = str.size();

  if (len < 3) return false;

  if (str[0] != '' || str[1] != '[' || ! isalpha(str[len - 1]))
    return false;

  int i = 2;

  while (i < len && ! isalpha(str[i])) {
    std::string arg;

    while (i < len && str[i] != ';' && ! isalpha(str[i]))
      arg += str[i++];

    if (i < len && str[i] == ';')
      ++i;

    args.push_back(arg);
  }

  return true;
}

void
println(const char *s)
{
  std::cout << s << "\r\n";
}

void
ed(int n)
{
  std::cout << CEscape::ED(n);
}

int
cup(int r, int c)
{
  std::cout << CEscape::CUP(r,c);

  return 1;
}

void
hvp(int r, int c)
{
  std::cout << CEscape::HVP(r,c);
}

void
csi(const char *s)
{
  std::cout << CEscape::CSI(s);
}

void
el(int n)
{
  std::cout << CEscape::EL(n);
}

void
rm(int n)
{
  std::cout << CEscape::RM(n);
}

void
cub(int n)
{
  std::cout << CEscape::CUB(n);
}

void
cuf(int n)
{
  std::cout << CEscape::CUF(n);
}

void
cuu(int n)
{
  std::cout << CEscape::CUU(n);
}

void
cud(int n)
{
  std::cout << CEscape::CUD(n);
}

void
deccolm(bool on)
{
  if (on)
    std::cout << CEscape::DECSET(3);
  else
    std::cout << CEscape::DECRST(3);
}

void
decaln()
{
  std::cout << CEscape::DECALN();
}

void
decstbm(int top, int bottom)
{
  std::cout << CEscape::DECSTBM(top, bottom);
}

void
decom(bool on)
{
  if (on)
    std::cout << CEscape::DECSET(6);
  else
    std::cout << CEscape::DECRST(6);
}

void
ind()
{
  std::cout << CEscape::IND();
}

void
ri()
{
  std::cout << CEscape::RI();
}

void
nel()
{
  std::cout << CEscape::NEL();
}

#define CSI_s "["

/* Test of:
   CUF (Cursor Forward)
   CUB (Cursor Backward)
   CUD (Cursor Down)      IND (Index)  NEL (Next Line)
   CUU (Cursor Up)        RI  (Reverse Index)
   CUP (Cursor Position)  HVP (Horizontal and Vertical Position)
   ED  (Erase in Display)
   EL  (Erase in Line)
   DECALN (Screen Alignment Display)
   DECAWM (Autowrap)
   <CR> <BS>
   Cursor control characters inside CSI sequences
*/

void
test1(int pass)
{
  int max_lines = 24;

  int max_cols = 132;
  int min_cols = 80;

  //set_tty_crmod();  /* want to disable tab/space conversion */

  int width;

  if (pass == 0) { deccolm(false); width = min_cols; }
  else           { deccolm(true ); width = max_cols; }

  decaln();

  /* Compute left/right columns for a 60-column box centered in 'width' */
  int inner_l = (width - 60) / 2;
  int inner_r = 61 + inner_l;

  int hlfxtra = (width - 80) / 2;

  cup( 9,inner_l); ed(1);
  cup(18,60+hlfxtra); ed(0); el(1);
  cup( 9,inner_r); el(0);

  /* 132: 36..97 */
  /*  80: 10..71 */
  for (int row = 10; row <= 16; row++) {
    cup(row, inner_l); el(1);
    cup(row, inner_r); el(0);
  }

  cup(17,30); el(2);

  for (int col = 1; col <= width; col++) {
    hvp(max_lines, col); std::cout << "*";
    hvp( 1, col); std::cout << "*";
  }

  cup(2,2);

  for (int row = 2; row <= max_lines-1; row++) {
    std::cout << "+";
    cub(1);
    ind();
  }

  cup(max_lines-1,width-1);

  for (int row = max_lines-1; row >=2; row--) {
    std::cout << "+";
    cub(1); ri();
  }

  cup(2,1);

  for (int row = 2; row <= max_lines-1; row++) {
    std::cout << "*";  cup(row, width);
    std::cout << "*";
    cub(10);
    if (row < 10) nel();
    else          std::cout << "\n";
  }

  cup(2,10);
  cub(42+hlfxtra); cuf(2);

  for (int col = 3; col <= width-2; col++) {
    std::cout << "+";
    cuf(0); cub(2); cuf(1);
  }

  cup(max_lines-1,inner_r-1);
  cuf(42+hlfxtra); cub(2);

  for (int col = width-2; col >= 3; col--) {
    std::cout << "+";
    cub(1); cuf(1); cub(0); std::cout << char(8);
  }

  cup( 1, 1); cuu(10); cuu(1); cuu(0);

  cup(max_lines,width); cud(10); cud(1); cud(0);

  cup(10,2+inner_l);

  for (int row = 10; row <= 15; row++) {
    for (int col = 2+inner_l; col <= inner_r-2; col++) std::cout << " ";
    cud(1); cub(58);
  }

  cuu(5); cuf(1);
  std::cout << "The screen should be cleared,  and have an unbroken bor-";

  cup(12,inner_l+3);
  std::cout << "der of *'s and +'s around the edge,   and exactly in the";

  cup(13,inner_l+3);
  std::cout << "middle  there should be a frame of E's around this  text";

  cup(14,inner_l+3);
  std::cout << "with  one (1) free position around it.    ";

  cup(50,1);
}

void
test2(int pass)
{
  #define A_AND_B(a,b) (void)(a && b)

  int max_lines = 24;

  int max_cols = 132;
  int min_cols = 80;

  /* DECAWM demo */
  static char on_left [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static char on_right[] = "abcdefghijklmnopqrstuvwxyz";

  int height = sizeof(on_left) - 1;
  int region = max_lines - 6;

  /* note: DECCOLM clears the screen */
  int width;

  if (pass == 0) { deccolm(false); width = min_cols; }
  else           { deccolm(true ); width = max_cols; }

  cup(1,1);

#if 0
  println("Test of autowrap, mixing control and print characters.");
  println("The left/right margins should have letters in order:");
#endif

  decstbm(3, region + 3);
  decom(true);        /* this also homes the cursor */

  for (int i = 0; i < height; ++i) {
    switch (i % 4) {
    case 0:
#if 0
      /* draw characters as-is, for reference */
      A_AND_B(cup(region + 1, 1), std::cout << char(on_left[i])));
      A_AND_B(cup(region + 1, width), std::cout << char(on_right[i])));
      std::cout << "\n";
#endif
      break;
    case 1:
      /* simple wrapping */
      A_AND_B(cup(region, width), std::cout << char(on_right[i - 1]) << char(on_left[i]));
      /* backspace at right margin */
      A_AND_B(cup(region + 1, width), std::cout << char(on_left[i]) << "\010 " << char(on_right[i]));
      std::cout << "\n";
      break;
    case 2:
#if 0
      /* tab to right margin */
      A_AND_B(cup(region + 1, width), std::cout << char(on_left[i]) << "\010\010\011\011" << char(on_right[i]));
      A_AND_B(cup(region + 1, 2), std::cout << "\010" << char(on_left[i]) << "\n");
#endif
      break;
    default:
#if 0
      /* newline at right margin */
      A_AND_B(cup(region + 1, width), std::cout << "\n");
      A_AND_B(cup(region, 1), std::cout << char(on_left[i]));
      A_AND_B(cup(region, width), std::cout << char(on_right[i]));
#endif
      break;
    }
  }

  decom(false);
  decstbm(0, 0);

  cup(max_lines - 2, 1);

  cup(40,1);
}

// vt_clear - ed
// vt_move  - cup
void
test3()
{
  ed(2);
  cup(1,1);

  println("Test of cursor-control characters inside ESC sequences.");
  println("Below should be four identical lines:");
  println("");

  println("A B C D E F G H I");

  for (int i = 1; i < 10; i++) {
    std::cout << char('@' + i);

    csi("2\010C"); /* Two forward, one backspace */
  }

  println("");

  /* Now put CR in CUF sequence. */
  std::cout << "A ";

  for (int i = 2; i < 10; i++)
    std::cout << CSI_s << "\015" << 2 * i - 2 << "C" << char('@' + i);

  println("");

  /* Now put VT in CUU sequence. */
  rm(20);

  for (int i = 1; i < 10; i++) {
    std::cout << char('@' + i) << " ";

    csi("1\013A");
  }

  println("");

  cup(10, 1);
}

void
test4()
{
  static int max_lines = 24;
  static int min_cols  = 80;

  static char on_left [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static char on_right[] = "abcdefghijklmnopqrstuvwxyz";

  int height = sizeof(on_left) - 1;
  int region = max_lines - 6;

  //set_tty_crmod(); /* want to disable tab/space conversion */

  /* note: DECCOLM clears the screen */
  deccolm(false);

  int width = min_cols;

  println("Test of autowrap, mixing control and print characters.");
  println("The left/right margins should have letters in order:");

  decstbm(3, region + 3);
  decom(true);  /* this also homes the cursor */
  for (int i = 0; i < height; ++i) {
    switch (i % 4) {
      case 0:
        /* draw characters as-is, for reference */
        A_AND_B(cup(region + 1, 1), printf("%c", on_left[i]));
        A_AND_B(cup(region + 1, width), printf("%c", on_right[i]));
        printf("\n");
        break;
      case 1:
        /* simple wrapping */
        A_AND_B(cup(region, width), printf("%c%c", on_right[i - 1], on_left[i]));
        /* backspace at right margin */
        A_AND_B(cup(region + 1, width), printf("%c\b %c", on_left[i], on_right[i]));
        printf("\n");
        break;
      case 2:
        /* tab to right margin */
        A_AND_B(cup(region + 1, width),
          printf("%c\b\b\t\t%c", on_left[i], on_right[i]));
        A_AND_B(cup(region + 1, 2), printf("\b%c\n", on_left[i]));
        break;
      default:
        /* newline at right margin */
        A_AND_B(cup(region + 1, width), printf("\n"));
        A_AND_B(cup(region, 1), printf("%c", on_left[i]));
        A_AND_B(cup(region, width), printf("%c", on_right[i]));
        break;
    }
  }
  decom(false);
  decstbm(0, 0);
  cup(max_lines - 2, 1);
}

void
colors()
{
  char c = '*';

  CRGBA rgba;

  for (int k = 0; k < 2; ++k) {
    if (k == 0)
      std::cout << "[48;0;0;0m";
    else
      std::cout << "[38;0;0;0m";

    for (int i = 0; i < 6; ++i) {
      for (int n = 0; n < 64; ++n) {
        double n1 = std::min(std::max(n*4, 0), 255)/255.0;

        if      (i == 0) {
          rgba = CRGBA(1.0, n1, 0);
        }
        else if (i == 1) {
          rgba = CRGBA(1.0 - n1, 1.0, 0);
        }
        else if (i == 2) {
          rgba = CRGBA(0, 1.0, n1);
        }
        else if (i == 3) {
          rgba = CRGBA(0, 1.0 - n1, 1.0);
        }
        else if (i == 4) {
          rgba = CRGBA(n1, 0, 1.0);
        }
        else if (i == 5) {
          rgba = CRGBA(1.0, 0, 1.0 - n1);
        }

        if (k == 0)
          std::cout << "[38;" << rgba.getRedI() << ";" <<
                                   rgba.getGreenI() << ";" <<
                                   rgba.getBlueI() << "m" << c << "[0,";
        else
          std::cout << "[48;" << rgba.getRedI() << ";" <<
                                   rgba.getGreenI() << ";" <<
                                   rgba.getBlueI() << "m" << c << "[0,";
      }

      std::cout << "\n";
    }
  }
}
