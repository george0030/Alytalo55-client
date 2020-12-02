#include <stddef.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCE, 0xAD };
byte ownIp[] = {192, 168, 1, 5};
IPAddress ip(ownIp[0], ownIp[1], ownIp[2], ownIp[3]); 
const int packetSize = UDP_TX_PACKET_MAX_SIZE;


const char PROTOCOL_HEADER = 55; 
const unsigned char MAJOR_VERSION = 1;
const unsigned char MINOR_VERSION = 1;

unsigned char incomingBuffer[packetSize];
unsigned char outcomingBuffer[packetSize];
unsigned int port = 6761;

unsigned long aloitusAika;

unsigned int potentiometer = 65535;
byte thermometerOne =(byte) -1;
byte thermometerTwo = (byte) -1;
byte ceilingLight = (byte) -1;
byte doorbell = (byte) -1;
IPAddress senderIp;
byte ledState = 0;


EthernetUDP udp;

void setup()
{
  pinMode(8, OUTPUT);
  Ethernet.begin(mac, ip);
   udp.begin(port);
}

void loop() {

  if(udp.parsePacket()){
  senderIp = udp.remoteIP();
  udp.read(incomingBuffer, packetSize);
  if(incomingBuffer[0] != PROTOCOL_HEADER){ return;}
  if((incomingBuffer[1] >> 5) != MAJOR_VERSION){ return;}
  if(incomingBuffer[1] & 1 == 1){ return;}
  if((incomingBuffer[1] & 2) >> 1 == 1){

    udp.beginPacket(senderIp, port);
    byte outbound[] = {PROTOCOL_HEADER, (MAJOR_VERSION << 5) | (MINOR_VERSION << 2) | (1 << 1) | 1, 0,6,0,7 };
    udp.write(outbound, 6);
    udp.endPacket();
    }
  else{

    ledState = incomingBuffer[8];
    if(ledState == 1)
      digitalWrite(8, HIGH);
    else
      digitalWrite(8, LOW);
    
    }

  }

if(senderIp){
    potentiometer = analogRead(2);
    if(analogRead(1)<100)
      ceilingLight = 1;
    else
      ceilingLight = 0;
    if(analogRead(3) < 50)
      doorbell = 1;
    else
      doorbell = 0;
    udp.beginPacket(senderIp, port);
    byte outbound2[] = {PROTOCOL_HEADER, (MAJOR_VERSION << 5) | (MINOR_VERSION << 2) | (0 << 1) | 1, (byte)((potentiometer & 992)>>5),(byte)( potentiometer&31), thermometerOne, thermometerTwo, ceilingLight, doorbell, ledState};
    udp.write(outbound2, 9);
    udp.endPacket();    
}
  

  
}



