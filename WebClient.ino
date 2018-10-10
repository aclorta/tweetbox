/*************************************************** 
  This is an example for the Adafruit CC3000 Wifi Breakout & Shield
 
  Designed specifically to work with the Adafruit WiFi products:
  ----> https://www.adafruit.com/products/1469
 
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!
 
  Written by Limor Fried & Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 
 /*
This example does a test of the TCP client capability:
  * Initialization
  * Optional: SSID scan
  * AP connection
  * DHCP printout
  * DNS lookup
  * Optional: Ping
  * Connect to website and print out webpage contents
  * Disconnect
SmartConfig is still beta and kind of works but is not fully vetted!
It might not work on all networks!
*/
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"
 
// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    17
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed
 
#define WLAN_SSID       "Wireless network SSID"           // cannot be longer than 32 characters!
#define WLAN_PASS       "WPA/WEP passphrase"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_UNSEC
 
#define IDLE_TIMEOUT_MS  2500      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value. Default: 3000
 
// What page to grab!
#define WEBSITE      "www.example.org"
#define WEBPAGE      "/index.php" // insert your php file here
 
 
/**************************************************************************/
/*!
    @brief  Sets up the HW and the CC3000 module (called automatically
            on startup)
*/
/**************************************************************************/
 
uint32_t ip;
const int appPin = 10;  // the number of the appliance pin
 
void setup(void)
{
  Serial.begin(115200);
  pinMode(appPin, OUTPUT);
  Serial.println(F("Hello, CC3000!\n")); 
  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
 
 
  // Optional: Do a ping test on the website
  /*
  Serial.print(F("\n\rPinging ")); cc3000.printIPdotsRev(ip); Serial.print("...");  
  replies = cc3000.ping(ip, 5);
  Serial.print(replies); Serial.println(F(" replies"));
  */  
 
//  Serial.println();
//  Serial.println(F("Printing buffer: "));
//  Serial.println(buff);
//  Serial.println(F("\n-------------------------------------")); 
}
 
void loop(void)
{
  // initialize the wifi module 
  // initializing within the loop improved connectivity
  initCC3000();
 
  /* Try connecting to the website.
     Note: HTTP/1.0 protocol is used to read pages which are written in PHP
  */
  Serial.print(F("Connecting to website... "));
  Adafruit_CC3000_Client www = cc3000.connectTCP(ip, 80);
  if (www.connected()) {
    www.fastrprint(F("GET "));
    www.fastrprint(WEBPAGE);
    www.fastrprint(F(" HTTP/1.0\r\n"));
    www.fastrprint(F("Host: ")); www.fastrprint(WEBSITE); www.fastrprint(F("\r\n"));
    www.fastrprint(F("\r\n"));
    www.println();
    Serial.println(F("completed."));
  } else {
    Serial.println(F("connection failed."));    
    return;
  }
 
  Serial.print(F("Reading data from website... "));
  String buff;  
  /* Read data until either the connection is closed, or the idle timeout is reached. */ 
  unsigned long lastRead = millis();
  while (www.connected() && (millis() - lastRead < IDLE_TIMEOUT_MS)) {
    while (www.available()) {
      char c = www.read();
      buff += c;
//      Serial.print(c);
      lastRead = millis();
    }
  }
  Serial.println(F("completed."));
  www.close();
 
/**************************************************************************/
/*!
    @brief  Parse website data (in buffer) for tweet, username, and command.
*/
/**************************************************************************/
  Serial.print(F("Parsing captured buffer... "));
  int begOfBuff = buff.indexOf("'");
  int endOfBuff = buff.lastIndexOf("'");
  int foundIndex;
  String strTweet;
  String strUsername;
  String strCommand;
 
  // find contents of the first pair of quotes
  foundIndex = buff.indexOf("'", begOfBuff + 1);
  strTweet = buff.substring(begOfBuff + 1, foundIndex);
 
  // find the contents of the second pair of quotes
  foundIndex = buff.lastIndexOf("'", endOfBuff - 1);
  strUsername = buff.substring(foundIndex + 1, endOfBuff);
 
  // find the command
  foundIndex = strTweet.indexOf(":");
  strCommand = strTweet.substring(foundIndex + 1, endOfBuff - 1);
 
  Serial.println(F("completed."));
  Serial.println(F("Parsed buffer playback"));
  Serial.println(F("-------------------------------------"));   
  Serial.print("Tweet: "); Serial.println(strTweet);
  Serial.print("Username: "); Serial.println(strUsername);
  Serial.print("Command: "); Serial.println(strCommand);
  Serial.println(F("-------------------------------------"));
 
 
/**************************************************************************/
/*!
    @brief  Execute command from tweet
*/
/**************************************************************************/
  switch (strCommand.toInt()) {
    case 0:
      digitalWrite(appPin, LOW);
      break;
 
    case 1:
      digitalWrite(appPin, HIGH);    
      break;
 
    default:
      break;
  } 
 
    ip = 0;
  // Try looking up the website's IP address
  //  Serial.print(WEBSITE); Serial.print(F(" -> "));
  while (ip == 0) {
    if (!cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }
 
  /* You need to make sure to clean up after yourself or the CC3000 can freak out */
  /* the next time your try to connect ... */
  Serial.print(F("Disconnecting... "));
  cc3000.disconnect();
  Serial.println(F("disconnected."));
 
  delay(3000); // max 30000, dev account may be banned from Twitter if called too frequently
} // end void loop()
 
  /* Initialise the module */
void initCC3000(void)
{
  Serial.print(F("\nInitializing CC3000... "));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  Serial.println(F("completed."));
  // Optional SSID scan
  // listSSIDResults();
 
  Serial.print(F("Attempting to connect to ")); Serial.print(WLAN_SSID); Serial.print(F("... "));
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
  Serial.println(F("connected."));
 
  /* Wait for DHCP to complete */
  Serial.print(F("Requesting DHCP... ")); 
  unsigned long lastRead2 = millis();
  while (!cc3000.checkDHCP() && (millis() - lastRead2 < IDLE_TIMEOUT_MS))
  {
    delay(100);
    lastRead2 = millis();
  }  
  Serial.println(F("completed."));
 
  ip = 0;
  // Try looking up the website's IP address
//  Serial.print(WEBSITE); Serial.print(F(" -> "));
  while (ip == 0) {
    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }  
}
 
/**************************************************************************/
/*!
    @brief  Begins an SSID scan and prints out all the visible networks
*/
/**************************************************************************/
 
void listSSIDResults(void)
{
  uint8_t valid, rssi, sec, index;
  char ssidname[33]; 
 
  index = cc3000.startSSIDscan();
 
  Serial.print(F("Networks found: ")); Serial.println(index);
  Serial.println(F("================================================"));
 
  while (index) {
    index--;
 
    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);
 
    Serial.print(F("SSID Name    : ")); Serial.print(ssidname);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();
  }
  Serial.println(F("================================================"));
 
  cc3000.stopSSIDscan();
}
 
/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
 
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
