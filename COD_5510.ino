#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Ticker.h>  // Include the Ticker library for timer interrupts

// Pin definitions for Nokia 5110 LCD
#define SCLK_PIN 15
#define DIN_PIN 2
#define DC_PIN 5
#define CS_PIN 4
#define RST_PIN 17

// Create an instance of the Adafruit_PCD8544 class
Adafruit_PCD8544 display = Adafruit_PCD8544(SCLK_PIN, DIN_PIN, DC_PIN, CS_PIN, RST_PIN);

// Replace these with your network credentials
const char* ssid = "12345678";
const char* password = "12345678";

WebServer server(80);

Ticker textScrollTicker;  // Ticker for handling text scrolling
volatile bool scrollEnabled = false;
volatile bool scrollLeftToRight = false;

int x = 0;
int y = 0;
String text = "";

// Bird bitmap data
static const unsigned char PROGMEM new_bird_bitmap[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xe0, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0x7f, 0x3f, 0x1f, 0x0f, 0x1f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x0f, 0x0f, 0x0f, 0x0f, 0x1f, 
  0x3f, 0x3f, 0x7f, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x3f, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 
  0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 
  0xc0, 0xc0, 0x80, 0x80, 0x08, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x07, 0xff, 0x03, 0x00, 0x00, 0x08, 
  0x1d, 0x0f, 0x0f, 0x1f, 0x1f, 0x0f, 0x0f, 0x04, 0x00, 0x00, 0x00, 0x3e, 0xfe, 0x8f, 0x07, 0x0f, 
  0x1f, 0x0f, 0x1f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x68, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x10, 0x00, 0x20, 0x30, 0x30, 0x70, 0x3b, 0xf8, 
  0xf0, 0x80, 0x80, 0x80, 0x00, 0x70, 0xe0, 0xe8, 0xf8, 0xf0, 0x07, 0xdf, 0xbb, 0x3f, 0x03, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x02, 0x02, 
  0x22, 0x22, 0x62, 0x22, 0x32, 0x3e, 0x1f, 0x5f, 0xef, 0xf2, 0xf1, 0xfc, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0x2f, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x67, 0xce, 0x1e, 0x1c, 0x38, 0x78, 0x78, 0xf8, 
  0xf8, 0xfc, 0xfc, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0x3f, 0x7f, 0xdf, 0x2f, 0x87, 0xc3, 0xf1, 0xf8, 
  0xfc, 0xff, 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xe0, 0xc0, 0xc0, 0xc0, 0x80, 0x80, 0x80, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00
  };

void IRAM_ATTR scrollText() {
  if (scrollEnabled) {
    if (scrollLeftToRight) {
      x += 1;
      if (x > 83) x = -text.length() * 6; // Move to start if the end is reached
    } else {
      x -= 1;
      if (x < -text.length() * 6) x = 83; // Move to end if the start is reached
    }
    updateDisplay();
  }
}

void handleRoot() {
  String html = "<html><head><style>button {background-color: #0000FF;"
                    "border: none;"
                    "color: white;"
                    "padding: 15px 32px;"
                    "text-align: center;"
                    "text-decoration: none;"
                    "display: inline-block;"
                    "font-size: 16px;"
                    "margin: 4px 2px;"
                    "cursor: pointer;"
                    "border-radius: 8px;"
                    "transition-duration: 0.4s;}"
                  "button:hover {background-color: #808080;}"
                  "input[type=text] {"
                    "width: 100%;"
                    "padding: 12px 20px;"
                    "margin: 8px 0;"
                    "box-sizing: border-box;"
                    "border: 2px solid #ccc;"
                    "border-radius: 4px;}"
                  "input[type=submit] {"
                    "background-color: #0000FF;"
                    "color: white;"
                    "padding: 14px 20px;"
                    "margin: 8px 0;"
                    "border: none;"
                    "border-radius: 4px;"
                    "cursor: pointer;}"
                  "input[type=submit]:hover {background-color: #808080;}"
                "</style></head><body>"
                "<h1>Nokia LCD Controller</h1>"
                "<form action=\"/displayText\" method=\"POST\">"
                "Enter text: <input type=\"text\" name=\"text\" value=\"" + text + "\"><br>"
                "<input type=\"submit\" value=\"Display Text\">"
                "</form>"
                "<button onclick=\"startScroll()\">Start Scrolling</button>"
                "<button onclick=\"stopScroll()\">Stop Scrolling</button>"
                "<button onclick=\"scrollLeftToRight()\">Scroll Left to Right</button>"
                "<button onclick=\"scrollRightToLeft()\">Scroll Right to Left</button>"
                "<button onclick=\"displayBird()\">Display Bird</button>"
                "<script>"
                "function startScroll() {"
                "  fetch('/startScroll');"
                "}"
                "function stopScroll() {"
                "  fetch('/stopScroll');"
                "}"
                "function scrollLeftToRight() {"
                "  fetch('/scrollLeftToRight');"
                "}"
                "function scrollRightToLeft() {"
                "  fetch('/scrollRightToLeft');"
                "}"
                "function displayBird() {"
                "  fetch('/displayBird');"
                "}"
                "</script>"
                "</body></html>";
  server.send(200, "text/html", html);
}

void handleDisplayText() {
  if (server.hasArg("text")) {
    text = server.arg("text");
    x = 0;
    y = 0;
    updateDisplay();
    server.send(200, "text/html", "<html><body><h1>Text displayed!</h1><a href=\"/\">Back</a></body></html>");
  } else {
    server.send(400, "text/html", "<html><body><h1>Error: No text provided</h1><a href=\"/\">Back</a></body></html>");
  }
}

void handleStartScroll() {
  scrollEnabled = true;
  server.send(200, "text/plain", "Scrolling started");
}

void handleStopScroll() {
  scrollEnabled = false;
  server.send(200, "text/plain", "Scrolling stopped");
}

void handleScrollLeftToRight() {
  scrollEnabled = true;
  scrollLeftToRight = true;
  server.send(200, "text/plain", "Scrolling left to right");
}

void handleScrollRightToLeft() {
  scrollEnabled = true;
  scrollLeftToRight = false;
  server.send(200, "text/plain", "Scrolling right to left");
}

void handleBird() {
  display.clearDisplay();
  display.drawBitmap(0, 0, new_bird_bitmap, 84, 48, BLACK); // Make sure the coordinates are correct
  display.display();
  server.send(200, "text/plain", "New bird image displayed on the LCD.");
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);  // Set text size to 1
  display.setCursor(x, y);
  display.print(text);
  display.display();
}

void setup() {
  // Initialize LCD
  display.begin();
  display.setContrast(60);
  display.clearDisplay();

  // Initialize Serial for debugging
  Serial.begin(115200);

  // Scan for networks
  Serial.println("Scan start");
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
      Serial.println("no networks found");
  } else {
      Serial.print(n);
      Serial.println(" networks found");
      Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
      for (int i = 0; i < n; ++i) {
          Serial.printf("%2d",i + 1);
          Serial.print(" | ");
          Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
          Serial.print(" | ");
          Serial.printf("%4d", WiFi.RSSI(i));
          Serial.print(" | ");
          Serial.printf("%2d", WiFi.channel(i));
          Serial.print(" | ");
          switch (WiFi.encryptionType(i)) {
          case WIFI_AUTH_OPEN:
              Serial.print("open");
              break;
          case WIFI_AUTH_WEP:
              Serial.print("WEP");
              break;
          case WIFI_AUTH_WPA_PSK:
              Serial.print("WPA");
              break;
          case WIFI_AUTH_WPA2_PSK:
              Serial.print("WPA2");
              break;
          case WIFI_AUTH_WPA_WPA2_PSK:
              Serial.print("WPA+WPA2");
              break;
          case WIFI_AUTH_WPA2_ENTERPRISE:
              Serial.print("WPA2-EAP");
              break;
          case WIFI_AUTH_WPA3_PSK:
              Serial.print("WPA3");
              break;
          case WIFI_AUTH_WPA2_WPA3_PSK:
              Serial.print("WPA2+WPA3");
              break;
          case WIFI_AUTH_WAPI_PSK:
              Serial.print("WAPI");
              break;
          default:
              Serial.print("unknown");
          }
          Serial.println();
          delay(10);
      }
  }
  Serial.println("");

  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/displayText", HTTP_POST, handleDisplayText);
  server.on("/startScroll", HTTP_GET, handleStartScroll);
  server.on("/stopScroll", HTTP_GET, handleStopScroll);
  server.on("/scrollLeftToRight", HTTP_GET, handleScrollLeftToRight);
  server.on("/scrollRightToLeft", HTTP_GET, handleScrollRightToLeft);
  server.on("/displayBird", HTTP_GET, handleBird);
  server.begin();
  Serial.println("HTTP server started");

  // Set up the Ticker to call scrollText every 100 ms
  textScrollTicker.attach_ms(100, scrollText);
}

void loop() {
  server.handleClient();
}
