
#include <SPI.h>
#include <Ethernet.h>


// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
#define P1 "/luce.htm?a=on "
#define P2 "/luce.htm?a=off "
#define P3 "/io.cgi?led=2 "
#define P4 "/io.cgi?led=3 "
#define P5 "/tost"
#define L1 15
#define L2 16
#define L3 14
#define L4 14
#define L5 5



// assign an IP address for the controller:
const IPAddress ip(192, 168, 1, 9);

int IdWeb=-1;
bool StWeb[]={false,false,false,false,false,false};
// 0 -> Luce Fuori
// 1 -> Luce Polli Piccola
// 2 -> Porta Polli
// 3 -> Luce Nafta
// 4 -> Luce Polli
// 5 -> Log Analog


//Come sapere che ore sono in char
#define serverOra "www.mdmecco.it"
const IPAddress ETH(192,168,1,6);
const IPAddress MyDNS(8,8,8,8);


// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
const EthernetServer server(80);


// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient mdmecco;
EthernetClient ghelfa;
EthernetClient GenWeb; //collegamento generico da utlizzare


void EtherSetup (){
  Ethernet.begin(mac, ip, MyDNS);
  
}

