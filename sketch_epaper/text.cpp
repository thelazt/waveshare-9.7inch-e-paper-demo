#include "text.h"

/* Callback to write to a memory buffer. */
void Text::callbackPixel(int16_t x, int16_t y, uint8_t count, uint8_t alpha, void *state) {
  Text * that = (Text*)state;

  uint32_t pos, shift;
  int16_t value;
  
  if (y < 0 || y >= that->height)
    return;
  if (x < 0 || x + count >= that->width)
    return;
  
  while (count--) {
    switch (that->bpp){
      case 2:
        pos = ((uint32_t)that->width * y + x) / 8;
        shift = (((uint32_t)that->width * y + x) % 8) * 2;
        value = ((that->buffer[pos] >> shift) & 0x3) + (that->invert ? 1 : -1) * ((alpha + 32) / 64);
        if (value < 0) 
          value = 0;
        else if (value > 3)
          value = 3;
        that->buffer[pos] &= ~(0x3  << shift);
        that->buffer[pos] |= (value << shift);
        break;
      case 3:
        pos = ((uint32_t)that->width * y + x) / 4;
        shift = (((uint32_t)that->width * y + x) % 4) * 4 + 1;
        value = ((that->buffer[pos] >> shift) & 0x7)  + (that->invert ? 1 : -1) * ((alpha + 16) / 32);
        if (value < 0) 
          value = 0;
        else if (value > 0x7)
          value = 0x7;
        that->buffer[pos] &= ~(0x7  << shift);
        that->buffer[pos] |= (value << shift);
        break;
      case 4:
        pos = ((uint32_t)that->width * y + x) / 4;
        shift = (((uint32_t)that->width * y + x) % 4) * 4;
        value = ((that->buffer[pos] >> shift) & 0xf) + (that->invert ? 1 : -1) * ((alpha + 8) / 16);
        if (value < 0) 
          value = 0;
        else if (value > 0xf)
          value = 0xf;
        that->buffer[pos] &= ~(0xf  << shift);
        that->buffer[pos] |= (value << shift);
        break;
      case 8:
        pos = ((uint32_t)that->width * y + x) / 2;
        shift = (((uint32_t)that->width * y + x) % 2) * 8;
        value = ((that->buffer[pos] >> shift) & 0xff) + (that->invert ? 1 : -1) * alpha;
        if (value < 0) 
          value = 0;
        else if (value > 0xff)
          value = 0xff;
        that->buffer[pos] &= ~(0xff << shift);
        that->buffer[pos] |= (value << shift);
        break;
    }
    x++;
  }
}

/* Callback to render characters. */
uint8_t Text::callbackCharacter(int16_t x, int16_t y, mf_char character, void *state) {
  Text * that = (Text*)state;
  return mf_render_character(that->font, x, y, character, callbackPixel, state);
}

/* Callback to render lines. */
bool Text::callbackLine(const char *line, uint16_t count, void *state) {
  Text * that = (Text*)state;
  switch (that->alignment){
    case LEFT:
      mf_render_aligned(that->font, that->margin, that->y, MF_ALIGN_LEFT, line, count, callbackCharacter, state);
      break;
    case CENTER:
      mf_render_aligned(that->font, that->width / 2, that->y, MF_ALIGN_CENTER, line, count, callbackCharacter, state);
      break;
    case RIGHT:
      mf_render_aligned(that->font, that->width - that->margin, that->y, MF_ALIGN_CENTER, line, count, callbackCharacter, state);
      break;
    default:
      mf_render_justified(that->font, that->margin, that->y, that->width - that->margin * 2, line, count, callbackCharacter, state);
  }
  
  that->y += that->font->line_height;

  return true;
}

bool Text::callbackCountLines(const char *line, uint16_t count, void *state) {
  int *linecount = (int*)state;
  (*linecount)++;
  return true;
}

void Text::setAlginment(enum Text::ALIGNMENT alignment){
  this->alignment = alignment;
}

Text::Text(uint16_t * buf, uint16_t width, uint16_t height, uint8_t bpp) : bpp(bpp), buffer(buf), width(width), height(height), alignment(JUSTIFY), margin(0) {
  setFont(getFonts());
}


bool Text::setFont(Font *font){
  if (font != nullptr){
    this->fontBase = font->font;
    this->font = font->font;
    return true;
  } else {
    return false;
  }
}

Font * Text::getFonts(){
  return (Font*)mf_get_font_list();
}

bool Text::setFont(const char * fontname){
  const struct mf_font_s *font = mf_find_font(fontname);
  if (font){
    this->fontBase = font;
    this->font = font;
    return true;
  } else {
    return false;
  }
}

bool Text::setScale(int scale){
    if (scale > 1) {
        mf_scale_font(&scaledfont, fontBase, scale, scale);
        font = &scaledfont.font;
        return true;
    } else {
      return false;
    }
}

bool Text::setScale(int scaleX, int scaleY){
    if (scaleX >= 1 && scaleY >= 1 && scaleX + scaleY > 2) {
        mf_scale_font(&scaledfont, fontBase, scaleX, scaleY);
        font = &scaledfont.font;
        return true;
    } else {
      return false;
    }
}

int Text::getLines(const char * text){
  int lines = 0;
  mf_wordwrap(font, width - 2 * margin, text, callbackCountLines, &lines);
  return lines;
}

int Text::getHeight(const char * text){
  return getLines(text) * font->height;
}

bool Text::setBuffer(uint16_t * buf, uint16_t width, uint16_t height){
  if (buf != nullptr && width > 0 && height > 0){
    this->buffer = buf;
    this->width = width;
    this->height = height;
    return true;
  } else {
    return false;
  }
}

void Text::generate(const char * text, bool invert){
  this->invert = invert;
  y = margin;
  mf_wordwrap(font, width - 2 * margin, text, callbackLine, this);
}
