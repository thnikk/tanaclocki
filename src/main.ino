/* ------------- Tanaclocki -------------*/
/* This is a wifi clock that I originally
made for a friend using an ESP8266 and an
SSD1306. Wifi Manager is included for easy
internet access as well as a web server for
setting the timezone. */


#include <Arduino.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

//#include <NTPClient.h>
#include <NTPtimeESP.h>

const char INDEX_HTML[] =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
"<title>Clock Timezone</title>"
"<style>"
"\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\""
"</style>"
"</head>"
"<body>"
"<FORM action=\"/\" method=\"post\">"
"Does your country use DST? <INPUT type=\"radio\" name=\"DST\" value=\"2\">Yes <INPUT type=\"radio\" name=\"DST\" value=\"0\">No<BR>"
"</P>"
"<P>"
"Set UTC/GMT offset: <input type=\"text\" name=\"TZ\">"
"</P>"
"<P>"
"Set girl: <input type=\"text\" name=\"GIRL\">"
"</P>"
"<P>"
"Click this after setting all values: <INPUT type=\"submit\" value=\"Set\">"
"</P>"
"</FORM>"
"</body>"
"</html>";

const unsigned char testArray[][512] PROGMEM = {
  // Asuka
  { 0x00, 0x00, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18,
  0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0c, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x80, 0x00,
  0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00,
  0x00, 0x00, 0x40, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x20, 0x01,
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00, 0x00, 0x02,
  0x00, 0x00, 0xd0, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0xe8, 0x03,
  0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x68, 0x06, 0x00, 0x00, 0x00, 0x08,
  0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x14, 0x00,
  0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x08,
  0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x10,
  0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x44, 0x00, 0x10, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x46, 0x00, 0x10,
  0x00, 0x00, 0x02, 0x00, 0x80, 0xe7, 0x00, 0x14, 0x00, 0x00, 0x02, 0x00,
  0xc0, 0xe3, 0x00, 0x08, 0x00, 0x00, 0x02, 0x00, 0xc0, 0xf3, 0x01, 0x08,
  0x00, 0x00, 0x05, 0x00, 0x9c, 0xf3, 0x01, 0x08, 0x00, 0x00, 0x85, 0x80,
  0xff, 0xf1, 0x01, 0x08, 0x00, 0x00, 0xc5, 0x00, 0xf0, 0x19, 0x03, 0x0a,
  0x00, 0x80, 0xc2, 0x00, 0xc0, 0xfc, 0x07, 0x0a, 0x00, 0x80, 0xc2, 0x00,
  0xc0, 0x1c, 0x06, 0x0a, 0x00, 0x40, 0xc1, 0x30, 0x30, 0x06, 0x08, 0x04,
  0x3c, 0x20, 0x81, 0x30, 0x38, 0xf3, 0x01, 0x04, 0x42, 0xa0, 0x80, 0xf8,
  0x39, 0x08, 0x06, 0x04, 0x9d, 0xd0, 0x00, 0x68, 0x1c, 0x00, 0x0c, 0x0a,
  0xba, 0x68, 0x00, 0xd8, 0xcf, 0xf9, 0x07, 0x08, 0x74, 0x69, 0x00, 0x38,
  0xe7, 0xfb, 0x07, 0x08, 0x74, 0x35, 0x00, 0xf8, 0xf1, 0xf3, 0x03, 0x08,
  0xe8, 0x3a, 0x00, 0xf8, 0xff, 0xcf, 0x0c, 0x10, 0xe8, 0x3a, 0x00, 0xf0,
  0x7f, 0xfe, 0x07, 0x10, 0xd0, 0x3a, 0x00, 0xf0, 0xff, 0xff, 0x01, 0x10,
  0xd0, 0x19, 0x00, 0xe0, 0x83, 0xff, 0x01, 0x20, 0xd0, 0x1f, 0x00, 0xe0,
  0x03, 0x7f, 0x07, 0x10, 0xd0, 0x1f, 0x00, 0xc0, 0x07, 0xff, 0x47, 0x10,
  0xd0, 0x1f, 0x00, 0xc0, 0x8f, 0xdf, 0xe7, 0x10, 0xd0, 0x3f, 0x00, 0x80,
  0xfe, 0xf7, 0x27, 0x27, 0xe8, 0x3f, 0x00, 0x80, 0xfd, 0xf8, 0x07, 0x41,
  0xe8, 0x7f, 0x00, 0x00, 0x33, 0xfe, 0x87, 0x81, 0xf4, 0x7f, 0x00, 0x00,
  0x0e, 0xff, 0x87, 0x80, 0xf4, 0x7f, 0x00, 0x00, 0xfe, 0xff, 0x47, 0x00,
  0xf4, 0x6f, 0x00, 0x00, 0xfe, 0xfd, 0x63, 0x00, 0xf4, 0x77, 0x00, 0x00,
  0x7c, 0xfc, 0x21, 0x00, 0xf4, 0x72, 0x00, 0x00, 0x7c, 0xfe, 0x10, 0x00,
  0xf4, 0x77, 0x00, 0x00, 0xfc, 0x7f, 0x18, 0x00, 0xe4, 0x7d, 0x00, 0x00,
  0xfc, 0x3f, 0x08, 0x00, 0xf4, 0x2f, 0x00, 0x00, 0xfc, 0x1f, 0x04, 0x00,
  0xca, 0x3f, 0x00, 0x00, 0xfc, 0x07, 0x03, 0x00, 0xca, 0x3f, 0x00, 0x00,
  0xfc, 0x83, 0x01, 0x00, 0x4a, 0x7f, 0x00, 0x00, 0xfc, 0xe0, 0x00, 0x00,
  0xca, 0x7f, 0x00, 0x00, 0x38, 0x30, 0x00, 0x00, 0x9a, 0x5f, 0x00, 0x00,
  0x18, 0x18, 0x00, 0x00, 0x3a, 0x5f, 0x00, 0x00, 0x18, 0x06, 0x00, 0x00,
  0x7a, 0x6e, 0x00, 0x00, 0x88, 0x01, 0x00, 0x00, 0xf9, 0x6c, 0x00, 0x00,
  0xe0, 0x00, 0x00, 0x00, 0xf0, 0x31, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
  0xf0, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  // Miku
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x30, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x02, 0x00,
  0x00, 0x06, 0x00, 0xF0, 0x1F, 0x84, 0x05, 0x00, 0x00, 0x09, 0x80, 0x0F,
  0xE0, 0x32, 0x0B, 0x00, 0x80, 0x10, 0x70, 0xF0, 0x1F, 0x59, 0x16, 0x00,
  0x80, 0x26, 0x8C, 0xFF, 0xE3, 0xE0, 0x24, 0x00, 0x40, 0x26, 0xF3, 0xFF,
  0xFF, 0xCF, 0x2D, 0x00, 0x40, 0xAE, 0xFC, 0xFF, 0xFF, 0x3F, 0x5A, 0x00,
  0x40, 0x6D, 0xFF, 0xFF, 0xFF, 0xFB, 0x91, 0x00, 0x20, 0xA9, 0xEF, 0xFF,
  0xFF, 0xF7, 0x27, 0x01, 0xA0, 0xC9, 0xEF, 0xFB, 0xFF, 0xEF, 0x1F, 0x02,
  0xA0, 0xC9, 0xF7, 0xFB, 0xFF, 0xDF, 0x77, 0x06, 0x90, 0xD8, 0xFB, 0xFD,
  0xFF, 0xDF, 0x8F, 0x09, 0x10, 0xD8, 0xFD, 0xFD, 0xFF, 0xBF, 0x1F, 0x16,
  0xD0, 0x99, 0xFD, 0x7D, 0xFF, 0x7F, 0x3F, 0x09, 0xD0, 0x91, 0xFE, 0x3E,
  0xFF, 0x7F, 0x3F, 0x0A, 0xE8, 0xB3, 0xFE, 0x3E, 0xFF, 0xFF, 0x7E, 0x05,
  0xE8, 0x33, 0xFF, 0xBE, 0xFE, 0xFF, 0x7E, 0x05, 0xE8, 0x23, 0xFF, 0xBE,
  0x7E, 0xFF, 0x7D, 0x02, 0xF4, 0xA7, 0x7F, 0xDF, 0xFE, 0xFE, 0xFD, 0x02,
  0xF4, 0xAF, 0x7F, 0xDF, 0xFD, 0xFE, 0xFB, 0x05, 0xF4, 0xAF, 0x7F, 0xDF,
  0xFD, 0x3C, 0xFA, 0x05, 0xF4, 0xAF, 0x7F, 0xDF, 0xFB, 0xCA, 0xF1, 0x09,
  0xF4, 0xA7, 0x7F, 0xEF, 0xFB, 0xF6, 0xC7, 0x0B, 0xFA, 0xB7, 0x7F, 0x03,
  0xFB, 0xED, 0xF7, 0x0B, 0xFA, 0x97, 0x7F, 0xEC, 0xF7, 0xDD, 0xF7, 0x0B,
  0xFA, 0xD3, 0x3F, 0xF7, 0xF7, 0xBD, 0xEF, 0x17, 0xFA, 0xD3, 0x5F, 0xF7,
  0xEF, 0x7D, 0xEF, 0x17, 0xFA, 0xDB, 0x6F, 0xF7, 0xDF, 0xFB, 0xEE, 0x17,
  0xFA, 0xDB, 0x77, 0xFB, 0xDF, 0xBB, 0xED, 0x17, 0xFA, 0xC9, 0x7F, 0xFB,
  0xBF, 0xDB, 0xE8, 0x2F, 0xFD, 0xC9, 0x7F, 0x3D, 0x7E, 0x36, 0xA7, 0x2F,
  0xFD, 0xCD, 0xFF, 0xFC, 0xFF, 0x15, 0xCE, 0x2F, 0xFD, 0xCD, 0xFF, 0x0C,
  0xFF, 0x0B, 0x9C, 0x2F, 0xFD, 0xD4, 0xFF, 0x02, 0xFC, 0x07, 0xDD, 0x2F,
  0xFD, 0xD4, 0xFF, 0x04, 0xF9, 0x0F, 0xDC, 0x2B, 0xFD, 0xD0, 0xBF, 0x06,
  0xFE, 0x0F, 0xCC, 0x15, 0xFC, 0xBE, 0x7F, 0x04, 0xFE, 0x5F, 0xDD, 0x0D,
  0xFE, 0xBE, 0x7F, 0x41, 0xFE, 0x5F, 0xDD, 0x0A, 0xFE, 0xBE, 0xFF, 0x01,
  0xFE, 0xDF, 0xDD, 0x0A, 0xFE, 0xBC, 0xFD, 0x81, 0xFE, 0x3F, 0x5E, 0x16,
  0x7E, 0x5A, 0xF9, 0x89, 0xFE, 0xFF, 0xAB, 0x16, 0x7E, 0x5A, 0xF5, 0xF1,
  0xFE, 0xEF, 0xCA, 0x16, 0x7E, 0x16, 0xE5, 0x0B, 0xFF, 0xFF, 0xCE, 0x16,
  0x7F, 0x8A, 0x96, 0xF7, 0xFF, 0xFF, 0xDF, 0x16, 0x7F, 0x52, 0x70, 0xB6,
  0xFE, 0xFF, 0xDF, 0x2E, 0x7F, 0xA2, 0xEF, 0xA9, 0xFE, 0xFF, 0xDF, 0x2E,
  0xBF, 0x41, 0xEF, 0xF5, 0xFF, 0xFF, 0x6F, 0x2D, 0xBF, 0x40, 0xC3, 0xFD,
  0xFF, 0xFF, 0x67, 0x2D, 0xBF, 0x80, 0xDC, 0xFB, 0xFF, 0xF7, 0x6B, 0x5B,
  0xBF, 0x80, 0xAA, 0xFB, 0xBF, 0xFB, 0x6D, 0x5B, 0x5F, 0x01, 0xB1, 0xFB,
  0x7F, 0xFC, 0x6A, 0x57, 0x5F, 0x01, 0x06, 0xF7, 0xFF, 0x7F, 0xA9, 0xB6,
  0x5F, 0x01, 0x38, 0xC7, 0xFF, 0xBF, 0xA8, 0xB6, 0x6F, 0x01, 0xD0, 0x36,
  0xFC, 0x4F, 0xA8, 0xAE, 0x6F, 0x01, 0xE8, 0xED, 0xC3, 0x37, 0xA8, 0x6E,
  0xAF, 0x00, 0xE8, 0x0A, 0x3F, 0x08, 0xD0, 0x5E, 0xAF, 0x00, 0x74, 0xD3,
  0x7F, 0x06, 0xD0, 0x5E, 0xB7, 0x00, 0x94, 0xE5, 0x7F, 0x05, 0x20, 0xDD,
  0xB7, 0x00, 0x6A, 0xED, 0x7F, 0x09, 0x00, 0xBD, 0xB7, 0x00, 0x95, 0x9E,
  0x7F, 0x16, 0x00, 0xBD, 0x5B, 0x00, 0x4A, 0x7F, 0x7E, 0x6E, 0x00, 0x7D,
  0x5B, 0x00, 0xA4, 0xFF, 0x79, 0x9F, 0x00, 0x7D },
  // Momiji
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff,
  0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0xc0, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x80, 0xc1, 0xff,
  0x1f, 0x18, 0x00, 0x00, 0x00, 0x40, 0xf0, 0xff, 0xff, 0x21, 0x00, 0x00,
  0x00, 0x20, 0xfe, 0xff, 0xff, 0x43, 0xe0, 0x01, 0x00, 0x18, 0xff, 0xe7,
  0xff, 0x87, 0x10, 0x02, 0x00, 0xc4, 0x83, 0xc3, 0xff, 0x07, 0x0b, 0x02,
  0x00, 0xe2, 0x01, 0x80, 0xff, 0x0f, 0x04, 0x04, 0x00, 0xf1, 0x00, 0x00,
  0xff, 0x1f, 0x00, 0x04, 0x80, 0x70, 0x00, 0x00, 0xfe, 0x1f, 0x00, 0x04,
  0x80, 0x38, 0x00, 0x00, 0xfc, 0x1f, 0x00, 0x04, 0x40, 0x1c, 0x80, 0x03,
  0xfc, 0x1f, 0x00, 0x04, 0x20, 0x0c, 0xfe, 0x7f, 0xf8, 0x3f, 0x00, 0x04,
  0x20, 0x8e, 0xfb, 0x7f, 0xf3, 0x27, 0xf0, 0x04, 0x10, 0xe7, 0xf9, 0x3f,
  0xf6, 0x07, 0xf0, 0x09, 0x10, 0xf7, 0xfd, 0x3f, 0xe4, 0x0f, 0xf8, 0x09,
  0x08, 0xf3, 0xfc, 0x27, 0xad, 0x07, 0xf8, 0x09, 0x88, 0xf3, 0xfc, 0xa7,
  0x09, 0x03, 0xf8, 0x13, 0x84, 0xf9, 0xfe, 0xb7, 0x4b, 0x00, 0xf8, 0x13,
  0x04, 0x79, 0xf6, 0xb7, 0x13, 0x00, 0xf8, 0x13, 0x84, 0x78, 0xf2, 0x93,
  0x07, 0x00, 0xf8, 0x13, 0x04, 0x78, 0xf2, 0x93, 0x07, 0x00, 0xf8, 0x13,
  0x02, 0x78, 0xfa, 0xc3, 0x0f, 0x00, 0xd8, 0x13, 0x02, 0x70, 0xfb, 0xc0,
  0x1f, 0x00, 0xd0, 0x13, 0x02, 0x00, 0x78, 0xe0, 0x1f, 0x00, 0xd0, 0x17,
  0x02, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0xd0, 0x17, 0x02, 0x00, 0x00, 0xe0,
  0x3f, 0x00, 0xc0, 0x13, 0x02, 0x00, 0x00, 0xf0, 0x7c, 0x00, 0xc0, 0x13,
  0x02, 0x00, 0x00, 0x70, 0xfe, 0x00, 0xc0, 0x13, 0x02, 0x80, 0x00, 0x10,
  0xff, 0x00, 0xc0, 0x23, 0x02, 0x80, 0x80, 0xc0, 0xff, 0x00, 0xc0, 0x23,
  0x02, 0x80, 0x40, 0x3c, 0xff, 0x00, 0x80, 0x23, 0x02, 0xfe, 0xf9, 0xff,
  0xff, 0x00, 0x80, 0x21, 0x04, 0x00, 0xfe, 0x1f, 0x00, 0x00, 0x80, 0x21,
  0x04, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x80, 0x21, 0x04, 0x30, 0xf0, 0x27,
  0x60, 0x80, 0x81, 0x41, 0x08, 0x78, 0xf8, 0xff, 0x40, 0x20, 0x03, 0x41,
  0x08, 0x38, 0xf0, 0x7f, 0xc0, 0x30, 0x02, 0x41, 0x08, 0x38, 0xf8, 0x7f,
  0xf1, 0xb8, 0x02, 0x84, 0x90, 0xb0, 0xf4, 0x7f, 0xca, 0x18, 0x00, 0x80,
  0x90, 0x78, 0xf3, 0xff, 0xee, 0x3c, 0x01, 0x80, 0xd0, 0xf0, 0xff, 0xff,
  0xff, 0x9c, 0x00, 0x10, 0xd0, 0xf1, 0x7f, 0xff, 0xff, 0x1e, 0x00, 0xb0,
  0x90, 0xf1, 0x7f, 0xff, 0xff, 0x1e, 0x00, 0x50, 0x90, 0xf1, 0xff, 0xff,
  0x7f, 0x1e, 0x00, 0x08, 0x90, 0xf2, 0xff, 0xff, 0x7f, 0x1e, 0x00, 0x08,
  0x90, 0xf2, 0xff, 0xff, 0x7f, 0x06, 0x00, 0x08, 0x90, 0xe2, 0xff, 0xff,
  0x7f, 0x06, 0x00, 0x08, 0x90, 0xe6, 0xff, 0xff, 0x3f, 0x07, 0x80, 0x11,
  0x90, 0x86, 0xff, 0xff, 0x1f, 0x07, 0x80, 0x12, 0x10, 0x06, 0xff, 0xff,
  0x0f, 0x07, 0x80, 0x0c, 0x10, 0x0e, 0x7e, 0xe0, 0x83, 0x03, 0x40, 0x00,
  0x20, 0x0e, 0x78, 0xe0, 0x99, 0x03, 0x40, 0x00, 0x20, 0x08, 0xe0, 0x7f,
  0x9c, 0x01, 0x38, 0x00, 0x20, 0x00, 0x80, 0x1f, 0x1f, 0x00, 0x04, 0x00,
  0x40, 0x00, 0x00, 0xc4, 0x1f, 0x00, 0x03, 0x00, 0x40, 0x00, 0x7e, 0xe0,
  0x1f, 0xc0, 0x0c, 0x00, 0x40, 0x80, 0x81, 0xe1, 0x0f, 0xf0, 0x73, 0x00,
  0x80, 0x03, 0xfe, 0xe7, 0x0f, 0x00, 0x80, 0x01, 0x00, 0x0f, 0x00, 0xe0,
  0xff, 0x00, 0x0c, 0x02, 0x00, 0x46, 0x02, 0xe0, 0xff, 0xff, 0x7f, 0x04,
  0x00, 0xf2, 0xff, 0xe7, 0xff, 0xff, 0xff, 0x04 }
};

// A UDP instance to let us send and receive packets over UDP
//WiFiUDP udp;
WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP);

NTPtime NTPch("time.nist.gov");
strDateTime dateTime;

U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, D3, D2, U8X8_PIN_NONE);
//IPAddress timeServerIP; // time.nist.gov NTP server address
ESP8266WebServer server(80);

//unsigned int localPort = 2390;      // local port to listen for UDP packets
//const char* ntpServerName = "time.nist.gov";
//const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
//byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
byte hPic = 64;
byte hText = 5;
int gmtOffset = -8;
byte version = 1;
byte dstState = 2;
byte girl = 2;

void handleRoot(){
  if (server.hasArg("TZ") || server.hasArg("GIRL") || server.hasArg("DST")) handleSubmit();
  else server.send(200, "text/html", INDEX_HTML);
}

void returnFail(String msg){
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
}

void handleSubmit(){
  String formData;
  String dstData;
  String girlData;

  // if (!server.hasArg("TZ") || !server.hasArg("DST") || !server.hasArg("GIRL")) return returnFail("BAD ARGS");
  if (server.hasArg("TZ")){
    formData = server.arg("TZ");
    gmtOffset = formData.toInt();
    EEPROM.write(0, gmtOffset + 11);
    Serial.print("Wrote GMT ");
    Serial.print(EEPROM.read(0) - 11);
    Serial.println(" to EEPROM.");
  }
  if (server.hasArg("DST")){
    dstData = server.arg("DST");
    dstState = dstData.toInt();
    EEPROM.write(2, dstState);
    Serial.print("Wrote DST state ");
    Serial.print(EEPROM.read(2));
    Serial.println(" to EEPROM.");
  }
  if (server.hasArg("GIRL")){
    girlData = server.arg("GIRL");
    girl = girlData.toInt();
    EEPROM.write(3, girl);
    Serial.print("Wrote girl ");
    Serial.print(EEPROM.read(3));
    Serial.println(" to EEPROM.");
  }
  EEPROM.commit();

  server.send(200, "text/html", INDEX_HTML);
}

void returnOK(){
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK\r\n");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  EEPROM.begin(2);
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);
  //u8g2_SetI2CAddress(u8g2.getU8g2(), 0x78);

  u8g2.begin();
  u8g2.enableUTF8Print();		// enable UTF8 support for the Arduino print() function

  u8g2.setFont(u8g2_font_b10_t_japanese1);  // all the glyphs of "こんにちは世界" are already included in japanese1: Lerning Level 1-6
  u8g2.setFontDirection(0);
  u8g2.firstPage();
  do {

    u8g2.setCursor(15, 15);
    u8g2.print("Connecting...");

  } while ( u8g2.nextPage() );

  /* Time keeping */
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  if (EEPROM.read(1) != version) {
    EEPROM.write(0, gmtOffset + 11);
    EEPROM.write(1, version);
    EEPROM.commit();
  }
  else {
    gmtOffset = EEPROM.read(0) - 11;
    dstState = EEPROM.read(2);
    girl = EEPROM.read(3);
  }

  WiFiManager wifiManager;
  // wifiManager.resetSettings(); // Reset network and credentials, used for testing

  wifiManager.autoConnect("AutoConnectAP", "waifuclock");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /*

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

  */

  //timeClient.begin();

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();

  u8g2.setFont(u8g2_font_b10_t_japanese1);  // all the glyphs of "こんにちは世界" are already included in japanese1: Lerning Level 1-6
  u8g2.setFontDirection(0);
  u8g2.firstPage();
  do {

    u8g2.setCursor(15, 15);
    u8g2.print("Local IP:");
    u8g2.setCursor(15, 30);
    u8g2.print(WiFi.localIP());

  } while ( u8g2.nextPage() );
  delay(1000);
}


unsigned long previousMillis;
byte blink = 0;

void loop(){

  //timeClient.update();

  dateTime = NTPch.getNTPtime(gmtOffset,dstState);


  server.handleClient();

  if ((millis() - previousMillis) > 1000){

  blink++; if (blink > 1) blink = 0;

  if(dateTime.valid) NTPch.printDateTime(dateTime);

  //int hour = timeClient.getHours();
  //int minutes = timeClient.getMinutes();


  u8g2.setFont(u8g2_font_b10_t_japanese1);
  u8g2.setFontDirection(0);
  u8g2.firstPage();
  do {

    int hour = dateTime.hour;
    int minutes = dateTime.minute;

    u8g2.drawXBMP(hPic, 0, 64, 64, testArray[girl]);

    bool PM = 0;
    //hour+=gmtOffset;
    //if (hour < 1) hour+=24;
    //if (hour > 24) hour-=24;

    u8g2.setCursor(hText, 15);
    if ((hour >= 6) && (hour < 12)) u8g2.print("おはよう");		   // morning
    if ((hour >= 12) && (hour < 18)) u8g2.print("こんにちは");		// day
    if ((hour >= 18) || (hour < 6)) u8g2.print("こんばんは");		  // night

    if (hour > 12) {
      hour-=12;
      PM=1;
    }

    u8g2.setCursor(hText, 30);
    u8g2.print("いまは");

    u8g2.setCursor(hText, 45);
    u8g2.setFont(u8g2_font_6x12_mn);
    if (hour <= 12) {
    u8g2.print(hour);
    if (blink == 0) u8g2.print(":");
    if (blink == 1) u8g2.print(" ");
    if (minutes < 10) { u8g2.print("0"); } // Add a leading zero if under 10 minutes
    u8g2.print(minutes);
    }
    else u8g2.print("??:??");

    u8g2.setFont(u8g2_font_b10_t_japanese1);
    if (PM == 1) u8g2.print("ごご");
    if (PM == 0) u8g2.print("ごぜん");
    u8g2.setCursor(hText, 60);
    u8g2.print("です");
  } while ( u8g2.nextPage() );

  previousMillis = millis();
}
}
