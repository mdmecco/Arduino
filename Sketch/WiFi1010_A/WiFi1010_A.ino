
unsigned long GetETH =0;
#define ETHT 1000


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  
  delay(200);
  
  //while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  //}
  Serial.println("************START**************");
  InitPinKey();
}

void loop() {
   if (WifiConnection()){
      MainClient();
   }
   GetKey();
}
