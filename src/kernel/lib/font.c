#include <lib/font.h>
#include <lib/symbols.h>
#include <libc/stddef.h>
#include <libc/ctype.h>

int8_t font_unknown[FONT_HEIGHT]        = {UNKNOWN};

int8_t font_space[FONT_HEIGHT]          = {SPACE};
int8_t font_exclamation[FONT_HEIGHT]    = {EXCLAMATION};
int8_t font_question[FONT_HEIGHT]       = {QUESTION};
int8_t font_comma[FONT_HEIGHT]          = {COMMA};
int8_t font_point[FONT_HEIGHT]          = {POINT};
int8_t font_colon[FONT_HEIGHT]          = {COLON};
int8_t font_semicolon[FONT_HEIGHT]      = {SEMICOLON};
int8_t font_plus[FONT_HEIGHT]           = {PLUS};
int8_t font_minus[FONT_HEIGHT]          = {MINUS};
int8_t font_asterisk[FONT_HEIGHT]       = {ASTERISK};
int8_t font_slash[FONT_HEIGHT]          = {SLASH};
int8_t font_equal[FONT_HEIGHT]          = {EQUAL};
int8_t font_backslash[FONT_HEIGHT]      = {BACKSLASH};
int8_t font_underscore[FONT_HEIGHT]     = {UNDERSCORE};
int8_t font_pipe[FONT_HEIGHT]           = {PIPE};
int8_t font_percent[FONT_HEIGHT]        = {PERCENT};
int8_t font_hashtag[FONT_HEIGHT]        = {HASHTAG};

int8_t font_chevron_left[FONT_HEIGHT]   = {CHEVRON_LEFT};
int8_t font_chevron_right[FONT_HEIGHT]  = {CHEVRON_RIGHT};
int8_t font_par_left[FONT_HEIGHT]       = {PARENTHESIS_LEFT};
int8_t font_par_right[FONT_HEIGHT]      = {PARENTHESIS_RIGHT};
int8_t font_bracket_left[FONT_HEIGHT]   = {BRACKET_LEFT};
int8_t font_bracket_right[FONT_HEIGHT]  = {BRACKET_RIGHT};
int8_t font_brace_left[FONT_HEIGHT]     = {BRACE_LEFT};
int8_t font_brace_right[FONT_HEIGHT]    = {BRACE_RIGHT};

int8_t font_a[FONT_HEIGHT]              = {A};
int8_t font_b[FONT_HEIGHT]              = {B};
int8_t font_c[FONT_HEIGHT]              = {C};
int8_t font_d[FONT_HEIGHT]              = {D};
int8_t font_e[FONT_HEIGHT]              = {E};
int8_t font_f[FONT_HEIGHT]              = {F};
int8_t font_g[FONT_HEIGHT]              = {G};
int8_t font_h[FONT_HEIGHT]              = {H};
int8_t font_i[FONT_HEIGHT]              = {I};
int8_t font_j[FONT_HEIGHT]              = {J};
int8_t font_k[FONT_HEIGHT]              = {K};
int8_t font_l[FONT_HEIGHT]              = {L};
int8_t font_m[FONT_HEIGHT]              = {M};
int8_t font_n[FONT_HEIGHT]              = {N};
int8_t font_o[FONT_HEIGHT]              = {O};
int8_t font_p[FONT_HEIGHT]              = {P};
int8_t font_q[FONT_HEIGHT]              = {Q};
int8_t font_r[FONT_HEIGHT]              = {R};
int8_t font_s[FONT_HEIGHT]              = {S};
int8_t font_t[FONT_HEIGHT]              = {T};
int8_t font_u[FONT_HEIGHT]              = {U};
int8_t font_v[FONT_HEIGHT]              = {V};
int8_t font_w[FONT_HEIGHT]              = {W};
int8_t font_x[FONT_HEIGHT]              = {X};
int8_t font_y[FONT_HEIGHT]              = {Y};
int8_t font_z[FONT_HEIGHT]              = {Z};

int8_t font_0[FONT_HEIGHT]              = {ZERO};
int8_t font_1[FONT_HEIGHT]              = {ONE};
int8_t font_2[FONT_HEIGHT]              = {TWO};
int8_t font_3[FONT_HEIGHT]              = {THREE};
int8_t font_4[FONT_HEIGHT]              = {FOUR};
int8_t font_5[FONT_HEIGHT]              = {FIVE};
int8_t font_6[FONT_HEIGHT]              = {SIX};
int8_t font_7[FONT_HEIGHT]              = {SEVEN};
int8_t font_8[FONT_HEIGHT]              = {EIGHT};
int8_t font_9[FONT_HEIGHT]              = {NINE};

void font_get(int8_t** font, char c) {
  if (isdigit(c)) {
    font_get_digit(font, c);
    return;
  }

  if (isalpha(c)) {
    font_get_alpha(font, c);
    return;
  }

  switch (c) {
    case ' ':
      *font = font_space;
      break;
    case '!':
      *font = font_exclamation;
      break;
    case '?':
      *font = font_question;
      break;
    case ',':
      *font = font_comma;
      break;
    case '.':
      *font = font_point;
      break;
    case ':':
      *font = font_colon;
      break;
    case ';':
      *font = font_semicolon;
      break;
    case '+':
      *font = font_plus;
      break;
    case '-':
      *font = font_minus;
      break;
    case '*':
      *font = font_asterisk;
      break;
    case '/':
      *font = font_slash;
      break;
    case '=':
      *font = font_equal;
      break;
    case '\\':
      *font = font_backslash;
      break;
    case '_':
      *font = font_underscore;
      break;
    case '|':
      *font = font_pipe;
      break;
    case '<':
      *font = font_chevron_left;
      break;
    case '>':
      *font = font_chevron_right;
      break;
    case '(':
      *font = font_par_left;
      break;
    case ')':
      *font = font_par_right;
      break;
    case '[':
      *font = font_bracket_left;
      break;
    case ']':
      *font = font_bracket_right;
      break;
    case '{':
      *font = font_brace_left;
      break;
    case '}':
      *font = font_brace_right;
      break;
    case '%':
      *font = font_percent;
      break;
    case '#':
      *font = font_hashtag;
      break;
    default:
      *font = font_unknown;
      break;
  }
}

void font_get_alpha(int8_t** font, char c) {
  switch (c) {
    case 'a':
    case 'A':
      *font = font_a;
      break;
    case 'b':
    case 'B':
      *font = font_b;
      break;
    case 'c':
    case 'C':
      *font = font_c;
      break;
    case 'd':
    case 'D':
      *font = font_d;
      break;
    case 'e':
    case 'E':
      *font = font_e;
      break;
    case 'f':
    case 'F':
      *font = font_f;
      break;
    case 'g':
    case 'G':
      *font = font_g;
      break;
    case 'h':
    case 'H':
      *font = font_h;
      break;
    case 'i':
    case 'I':
      *font = font_i;
      break;
    case 'j':
    case 'J':
      *font = font_j;
      break;
    case 'k':
    case 'K':
      *font = font_k;
      break;
    case 'l':
    case 'L':
      *font = font_l;
      break;
    case 'm':
    case 'M':
      *font = font_m;
      break;
    case 'n':
    case 'N':
      *font = font_n;
      break;
    case 'o':
    case 'O':
      *font = font_o;
      break;
    case 'p':
    case 'P':
      *font = font_p;
      break;
    case 'q':
    case 'Q':
      *font = font_q;
      break;
    case 'r':
    case 'R':
      *font = font_r;
      break;
    case 's':
    case 'S':
      *font = font_s;
      break;
    case 't':
    case 'T':
      *font = font_t;
      break;
    case 'u':
    case 'U':
      *font = font_u;
      break;
    case 'v':
    case 'V':
      *font = font_v;
      break;
    case 'w':
    case 'W':
      *font = font_w;
      break;
    case 'x':
    case 'X':
      *font = font_x;
      break;
    case 'y':
    case 'Y':
      *font = font_y;
      break;
    case 'z':
    case 'Z':
      *font = font_z;
      break;
  }
}

void font_get_digit(int8_t** font, char c) {
  switch (c) {
    case '0':
      *font = font_0;
      break;
    case '1':
      *font = font_1;
      break;
    case '2':
      *font = font_2;
      break;
    case '3':
      *font = font_3;
      break;
    case '4':
      *font = font_4;
      break;
    case '5':
      *font = font_5;
      break;
    case '6':
      *font = font_6;
      break;
    case '7':
      *font = font_7;
      break;
    case '8':
      *font = font_8;
      break;
    case '9':
      *font = font_9;
      break;
  }
}