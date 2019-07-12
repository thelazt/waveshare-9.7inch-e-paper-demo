#include "epaper/it8951.h"
#include "text.h"

// e-Paper Pins
IT8951 epd(GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27);

bool print(const char * text, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char * font = "DejaVuSans12", enum Text::ALIGNMENT align = Text::JUSTIFY, bool invert = false){
  const int bpp = 4; // 4 Bits per Pixel

  // memory align
  x = x - x % (16/bpp);
  width = width - width % (16/bpp);


  // reserve Buffer  (beware of out-of-memory!)
  size_t len = width * height / bpp; // 4 Bits per Pixel

  void * buf=malloc(len * 2);  // char size!

  // Set to black (0x00) or white (0xff)
  memset(buf, invert ? 0x00 : 0xff, len * 2);

  // Create Text Object
  Text out((uint16_t*)buf, width, height, bpp);

  // Select font
  if (!out.setFont(font))
    Serial.println("Font not found, using default");

  // Optional configuration
  out.setAlginment(align);

  // Generate Text in buffer
  out.generate(text, invert);

  // Load buffer to epaper
  if (!epd.load((uint16_t*)buf, len, x, y, width, height)){
    Serial.println("Loading buffer failed");
    free(buf);
    return false;
  }
  free(buf);

  // Partial displaying 
  if (!epd.display(x, y, width, height)){
    Serial.println("Partial display failed");

    return false;
  }

  return true;
}



void setup() {
  Serial.begin(115200);
  Serial.println("Boot");
  Serial.println("Init");

  epd.begin(4000000);
  Serial.print("Device Info:\n Panel ");
  Serial.print(epd.width());
  Serial.print(" x ");
  Serial.println(epd.height());
  Serial.print(" Address 0x");
  Serial.println(epd.defaultImageBuffer(), HEX);
  Serial.print(" Firmware: ");
  Serial.println(epd.getFW());
  Serial.print(" LUT: ");
  Serial.println(epd.getLUT());
  
  if (epd.active())
    Serial.println("active");
  if (epd.clear())
    Serial.println("cleared");

  if (!print("handi demo", epd.width() / 2 - 105, 50, 210, 40, "DejaVuSans32", Text::CENTER, true))
    Serial.println("Title failed");

  if (!print("Zornig und gequält rügen jeweils Pontifex und Volk die maßlose bischöfliche Hybris!", 100, 400, 400, 100, "DejaVuSans24", Text::LEFT))
    Serial.println("Text failed");

  epd.display(0,0, 2000, 2000);
  Serial.println("displayed");
}

void loop() {
  // put your main code here, to run repeatedly:

}
