#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "mcufont/decoder/mcufont.h"


class Font : private mf_font_list_s {
  friend class Text;
  
  public:
    const char * getFullName(){ return font->full_name; }
    const char * getName(){ return font->full_name; }
    Font * getNext(){ return (Font*)next; }
};

class Text {
 public:
  enum ALIGNMENT {
     LEFT,
     CENTER,
     RIGHT,
     JUSTIFY
  };
  Text(uint16_t * buf, uint16_t width, uint16_t height, uint8_t bpp = 4);
  bool setFont(const char * name);
  bool setFont(Font *font);
  Font * getFonts();
  bool setScale(int scale);
  bool setScale(int scaleX, int scaleY);
  bool setMargin(int margin);
  void setAlginment(enum ALIGNMENT alginment);
  int getLines(const char * text);
  int getHeight(const char * text);
  void generate(const char * text, bool invert = false);
  bool setBuffer(uint16_t * buf, uint16_t width, uint16_t height);

 private:
  static bool callbackCountLines(const char *line, uint16_t count, void *state);
  static bool callbackLine(const char *line, uint16_t count, void *state);
  static uint8_t callbackCharacter(int16_t x, int16_t y, mf_char character, void *state);
  static void callbackPixel(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state);

  bool invert;
  uint8_t bpp;
  uint16_t * buffer, width, height, y;
  enum ALIGNMENT alignment;
  int margin = 5;
  const struct mf_font_s *font, * fontBase;
  struct mf_scaledfont_s scaledfont;
};
