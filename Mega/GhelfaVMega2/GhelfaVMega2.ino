

unsigned long D;
 

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  //EthernetSetup();
  DigitalSetup();
  InitDigitalIn();
  WifiConnection();
}

void loop() {
  // put your main code here, to run repeatedly:
  EthernetMainServer();
  //DigitalFlash();
  ProcDigitaIn();
  EthernetMainClient();
  CheckTimeMain();

  if (millis()> D){
    D=millis()+5000;
    Serial.print("*********************************DayTime:");
    Serial.println(DaySec());    
  }

}
