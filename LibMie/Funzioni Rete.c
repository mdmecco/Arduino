/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

// Variabili per gestione rete ed ambiente
bool rp=false;


byte NetMasb = 252;
byte NetMas = 0;
byte MASEt = 0;
unsigned long NetTo = 0;
char NetRdC = 0;
String NetCMDS = "";
String NetPARS = "";

String S1;
String S2;
String S3;

byte HPage =0;



String BtnColor(int idCol) {
  if (idCol == 0) {
    return "#22ff22";
  } else {
    return "red";
  }
}




void HTMLButton ( String Actions, String Values ) {
    client.print(F("<form action="""));
    client.print(Actions);
    client.print(F(""">"));
    client.print(F("<input type=""submit"" value="""));
    client.print(Values);
    client.print(F(""">"));
    client.print(F("</form>"));
} 



void HTMLParameter(){
    for (int i=0 ; i <= TotalOut; i++){
        if (MySIp == iOut[i].IdBoard){
            client.print(F( "<form><label for=""lampid"""));
            client.print(i);
            client.print(F( """> minuti di attivazione massima della luce (1-300) Id "));
            client.print(i);
            client.print(F( "   "));
            client.print(iOut[i].Name);
            client.print(F( ":</label>"));
            
            client.print(F("<input type=""number"" name=""LL"""));
            client.print(i);
            client.print(F("TT"" id=""minutes""  min=""10"" max=""300"" value="));
            client.print (iOut[i].TOn/60000);
            client.println(F(" > </form>  <br>"));
        }
    }
}


void HTTPHeader(){
    //********************************************* pagina server inizio  *****************************************************
    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-Type: text/html"));
    client.println(F("Connection: close"));  // the connection will be closed after completion of the response
    
    //------------------------------------------- Refresh  ***********************************************************
    if (rp){
        client.println(F("Refresh: 0;url=/"));  // reset the pages
        rp=false;
    }
    //****************************************************************************************************************************
     
    client.println();

}






void HTMLHeader(){
    client.println(F("<!DOCTYPE HTML>"));
    client.print(F("<html><head><title>"));
    client.print(WEBTITPAGE);
    client.println(F("</title></head>"));
    client.println(F("<body>"));
    client.print(F("<b style=""font-size:10px"">"));
    client.print(PRGVER);
  
  
    client.println(F("<table style=""width:100%"" border=1>"));
    client.print(F("<tr> <th width=50% align=""center"">"));
    client.println(NetConnW());
    client.println(F("</th>"));
    client.print(F("<th width=50% align=""center"">"));
    client.print(STime(DaySec()));
    client.println(F("</th>"));
    client.print(F("</tr></table>"));

    
    
    client.print(F("</b>"));
    if (OTAActive){
        client.print(F("<table style=""width:100%"" border=1> <tr><th width=100% align=""center""> OTA Active </th> </tr> </table> <hr width=100% size=4 color=FF0000> "));
    }
    
    client.print(F("<hr width=100% size=4 color=0000FF>\r\n"));   // Linea Separatrice
    
    client.println(F("<table style=\"width: 100%\" border=\"0\"><tbody>"));
    client.println(F("<tr>"));
    HTMLButton("/PAR", "Parametri");
    client.println(F("</tr><tr>"));
    HTMLButton("/STATUS", "STATUS");
        
    client.println(F("</tr><tr>"));
    HTMLButton("/OTA", "Firmware OTA");
    client.println(F("</tr><tr>"));
    HTMLButton("/FILES", "Lista files SD");
    client.println(F("</tr></table>"));
    client.print(F("<hr width=100% size=4 color=00FF00>\r\n"));   // Linea Separatrice

    client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));

}
    


void WebServer (){

//********** SERVER ****************************************************************
      /*
      if (NetMas != NetMasb) {
        Serial.print ("************ Server NetMas =");
        Serial.println (NetMas);
        NetMasb=NetMas ;
      }
      */
        

    #if defined (__AVR_ATmega2560__)  
        if (MASEt != 100){
            return;
            // Serial.println ("************ NOT RUNNING ");
        }
    #endif    
    
    

    switch (NetMas){
      
      case 0:
        
        client = server.available();
        
        if ((client) && (client.connected())){ // in ricezione
        //if (client.available()){
        //if ( (client)){ // in ricezione
          // Serial.println ("************ InServer B");
          NetMas=5;
          NetTo=millis()+3000;
        }
        break;
        
      case 1:
        NetMas=200;
        break;
        
      case 5:
        if (client.available()){
          NetRdC=client.read();
          if(NetRdC=='<'){ // comando <CMD-PAR>
            NetMas=10;
            NetTo=millis()+100;     
          }else if(NetRdC=='G'){
            NetMas=50;     
          }else if(NetRdC=='P'){
            NetMas=50;     
          } 
          
        }
        if (millis()>NetTo){
          NetMas=200; // funzione del timeout di ricezione
        }
        break;
      case 10:
        NetCMDS=client.readStringUntil('-');//
        NetPARS=client.readStringUntil('>');//
        if (millis()>NetTo){
          NetMas=200; // funzione del timeout di ricezione
        }
        NetMas=20;
        break;
      case 20:
        if (NetCMDS=="STATUS"){
          
        }else if (NetCMDS=="L1"){
          //Slr(1,GetValue(NetPARS));
        }else if (NetCMDS=="L2"){
          //Slr(2,GetValue(NetPARS));
        }else if (NetCMDS=="L3"){
          //Slr(3,GetValue(NetPARS));
        }else if (NetCMDS=="L4"){
          //Slr(4,GetValue(NetPARS));
        }else if (NetCMDS=="L5"){
          //Slr(5,GetValue(NetPARS));
        }else if (NetCMDS=="L6"){
          //Slr(6,GetValue(NetPARS));
        }else if (NetCMDS=="L7"){
          //Slr(7,GetValue(NetPARS));
        }else if (NetCMDS=="L8"){
          //
        }
        NetMas=0;
        client.stop();
        break;
      case 50:
        {
          HPage=0;
          NetTo=millis()+500;
          NetCMDS=client.readStringUntil('\r');
          int io1=NetCMDS.indexOf("/SET?");
          int io2=0;
          if (io1 > 0){
            rp=true;
            io1=NetCMDS.indexOf("?",io1-1)+1;
            io2=NetCMDS.indexOf(" ",io1);
            S1=NetCMDS.substring(io1, io2);
            io1=0;
            while (io1>=0){
              io2=S1.indexOf("=", io1);
              S2=S1.substring(io1,io2);
              io1=S1.indexOf("&", io2);
              if (io1>0){
                S3=S1.substring(io2+1,io1);
                io1=io1+1;
              }else{
                S3=S1.substring(io2+1);
              }
            }
          }

   
    //*****************************************  OTA  attivazione web  *****************************      
          io1=NetCMDS.indexOf("/OTA");
          io2=0;
          if (io1 > 0){
            HPage=0;
            rp=true;
            OTAActive= !OTAActive;
            OTABegin();            
          }
    //**********************************************************************************************      


    //*****************************************  Prametri  *****************************      
          io1=NetCMDS.indexOf("/PAR");
          io2=0;
          if (io1 > 0){
            HPage=1;
            io1=NetCMDS.indexOf("?LL");
            io2=NetCMDS.indexOf("TT=");
            if (io1 > 0) {
              String io3=NetCMDS.substring(io1+3,io2);
              byte ioId = io3.toInt();
              io1=NetCMDS.indexOf(" HT");
              String iotim=NetCMDS.substring(io2+3,io1);
              //// Serial.println(NetCMDS);
              io1=iotim.toInt();  
              iOut[ioId].TOn=((unsigned long) io1 * 60000);
              WriteTime();
            }
          }
    //**********************************************************************************************      
          io1=NetCMDS.indexOf("/FILES");
          io2=0;
          if (io1 > 0){
            HPage=2;
            io1=NetCMDS.indexOf("SHOWFILES=");
            if (io1 > 0){
              io2=NetCMDS.indexOf(" HT");
              ShowFile =NetCMDS.substring(io1+10,io2);
            }
          }
     //***************************************************************************************************     

    //**********************************************************************************************      
          io1=NetCMDS.indexOf("/STATUS");
          io2=0;
          if (io1 > 0){
            HPage=3;
          }
     //***************************************************************************************************     




          while (client.available()){
            char c = client.read();
          }
          NetMas=100; // funzione del timeout di ricezione
        }

        break;
        
      case 100:
          HTTPHeader();
          HTMLHeader();  
          if (HPage ==1) {
            HTMLParameter();
          }else if (HPage ==2) { 
            HTMLFileList();
          }else if (HPage ==3) {
            HTMLStatus();
          }
          client.println(F("</body>"));
                
        NetMas=200;
        break;      
      case 200:
        NetTo=millis()+200;
        NetMas=201;
        break;
        
      case 201:
        if (millis() > NetTo) {
            client.stop();
            NetMas=0;
        }  
        break;
      default:
        NetMas=0;
    }
//***********************************************************************


}




