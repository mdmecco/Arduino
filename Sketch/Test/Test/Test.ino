
const unsigned long Full=4294967295;

unsigned long VBool =0;
bool BBool=false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  VBool =0;

  //VBool= 2 & VBool;

  //Serial.println(VBool);

  SBool(2,true);  
  Serial.println(VBool);
  

  SBool(4,true);  
  Serial.println(VBool);

  SBool(32,true);  
  Serial.println(VBool);

  SBool(2,false);  
  Serial.println(VBool);
  
  

  
  Serial.println("---------------------");
  delay(1000);

 
}

void SBool(long Id, bool VB){
  if (VB){
    VBool= VBool | (Id & Full);
  }else{
    VBool= VBool & (~(Id & Full));
  }
}

bool GBool(long Id) {
  return ((Id & VBool)> 0);
}

