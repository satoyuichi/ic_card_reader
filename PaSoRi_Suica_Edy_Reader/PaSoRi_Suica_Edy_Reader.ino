#include "Spi.h"
#include "Usb.h"

#include <Ethernet.h>
#include "PaSoRi.h"

//#define ETHERNET
#define FELICA

#ifdef ETHERNET
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x48, 0xCE };
char server[] = "www.google.com";
EthernetClient client;
#endif	/* ETHERNET */

#ifdef FELICA
PaSoRi pasori;
#endif	/* FELICA */

void setup()
{
  Serial.begin(115200);
  //  Serial.begin(9600);
  Serial.println("Start");

#ifdef FELICA
  byte rcode = pasori.begin(); // initialize PaSoRi
  if (rcode != 0) {
    Serial.print("PaSoRi initialization failed! : rcode = ");
    Serial.println(rcode, HEX);
    while (1); // stop
  }
#endif  /* FELICA */

#ifdef ETHERNET
  Serial.println("Get DHCP Address");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.print("connected... ");
    Serial.println(Ethernet.localIP());
    // Make a HTTP request:
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
#endif  /* ETHERNET */
}

void loop()
{
  byte rcode, i;

#ifdef FELICA
  pasori.task(); // call this at every loop
  static short syscode = POLLING_EDY;

  // Polling for SFC or Edy each time
  syscode = syscode == POLLING_SUICA ? POLLING_EDY : POLLING_SUICA;
  rcode = pasori.poll(syscode);
  if (rcode) {
    Serial.println(rcode, HEX);
    delay(500);
  } else {
    // Polling successful
    Serial.print("FeliCa detected. IDm=");
    for (i = 0; i < 8; i++) {
      Serial.print(pasori.getIDm()[i], HEX);
      Serial.print(" ");
    }
    Serial.print("  PMm=");
    for (i = 0; i < 8; i++) {
      Serial.print(pasori.getPMm()[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
    delay(3000);
  }
#endif	/* FELICA */

#ifdef ETHERNET
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    // if the server's disconnected, stop the client:
    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();

      // do nothing forevermore:
      while (true);
    }
#endif	/* ETHERNET */
}

/* void loop() */
/* { */
/*   byte rcode, i; */
/*   pasori.task(); // call this at every loop */
/*   //  static short syscode = POLLING_EDY; */
/*   static short syscode = POLLING_SUICA; */

/*   // Polling for SFC or Edy each time */
/*   syscode = syscode == POLLING_SUICA ? POLLING_EDY : POLLING_SUICA; */
/*   rcode = pasori.poll(syscode); */
/*   if (rcode) { */
/*     Serial.println(rcode, HEX); */
/*     delay(500); */
/*   } else { */
/*     Serial.println(rcode, HEX); */
/*     // Polling successful */
/*     Serial.print("FeliCa detected. IDm="); */
/*     for (i = 0; i < 8; i++) { */
/*       Serial.print(pasori.getIDm()[i], HEX); */
/*       Serial.print(" "); */
/*     } */
/*     Serial.print("  PMm="); */
/*     for (i = 0; i < 8; i++) { */
/*       Serial.print(pasori.getPMm()[i], HEX); */
/*       Serial.print(" "); */
/*     } */
/*     Serial.println(""); */

/*     if (syscode == POLLING_SUICA) { */
/*       // Read SFC (Suica etc.) */
/*       Serial.println("<< SFC >>"); */
/*       readSFC(); */
/*     } */
/*     else if (syscode == POLLING_EDY) { */
/*       // Read Edy */
/*       Serial.println("<< Edy >>"); */
/*       readEdy(); */
/*     } */
/*     else { */
/*       Serial.println(syscode); */
/*     } */
/*     delay(3000); */
/*   } */
/* } */

/* int readSFC() */
/* { */
/*   byte b[16]; */
/*   int ret = -1; */
/*   for (int i = 0; i < 32; i++) { */
/*     int rcode = pasori.read_without_encryption02(0x090f,i,b); */
/*     if (rcode) { */
/*       Serial.print("rcode = "); */
/*       Serial.println(rcode, HEX); */
/*       break; */
/*     } */
/*     Serial.print(i, DEC); */
/*     Serial.print(": "); */
/*     Serial.print(b[11]*256+b[10]); */
/*     Serial.print(" YEN  "); */
/*     if (i == 0) ret = (unsigned int)b[11]*256+b[10]; */
    
/*     // date */
/*     Serial.print(2000+((b[4]>>1)&0x7f), DEC); */
/*     Serial.print("."); */
/*     Serial.print(((b[4]&0x01)<<3)|((b[5]>>5)&0x07), DEC); */
/*     Serial.print("."); */
/*     Serial.print(b[5]&0x1F, DEC); */
    
/*     // from/to */
/*     Serial.print(" from:"); */
/*     Serial.print(b[6], HEX); */
/*     Serial.print("-"); */
/*     Serial.print(b[7], HEX); */
/*     Serial.print(" to:"); */
/*     Serial.print(b[8], HEX); */
/*     Serial.print("-"); */
/*     Serial.print(b[9], HEX); */

/*     Serial.println(""); */
/*   } */
/*   return ret; */
/* } */

/* int readEdy() */
/* { */
/*   byte b[16]; */
/*   int ret = -1; */
/*   for (int i = 0; i < 32; i++) { */
/*     int rcode = pasori.read_without_encryption02(0x170F,i,b); */
/*     if (rcode) { */
/*       Serial.print("rcode = "); */
/*       Serial.println(rcode, HEX); */
/*       break; */
/*     } */
/*     Serial.print(i, DEC); */
/*     Serial.print(": "); */
/*     Serial.print(b[14]*256+b[15], DEC); */
/*     Serial.println(" YEN"); */
/*     if (i == 0) ret = (unsigned int)b[14]*256+b[15]; */
/*   } */
/*   return ret; */
/* } */

