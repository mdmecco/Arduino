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

byte NetMas = 0;
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
    Serial.println("Entrato in parameter");
    Serial.println(iOut[0].IdBoard);
    Serial.print("---------");
    Serial.println(MySIp);
    for (int i=0 ; i <= TotalOut; i++){
        if (MySIp == iOut[i].IdBoard){
            Serial.print (iOut[i].IdBoard);
            Serial.print ("--------");
            client.print(F( "<label for=""lampid"""));
            client.print(i);
            client.print(F( """> minuti di attivazione massima della luce (1-300):</label> <input type=""number"" id=""minutes"" name=""minutes""  min=""10"" max=""300"" value="));
            client.print (iOut[i].TOn/1000);
            Serial.print("--");
            Serial.print(iOut[i].TOn);
            Serial.print("--");
            client.println(F(" > <br>"));
            Serial.println(i);
            
        }
    }
    Serial.println("---------");
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
    client.print(F("</b>"));
    if (OTAActive){
        client.print(F("<table style=""width:100%"" border=1> <tr><th width=100% align=""center""> OTA Active </th> </tr> </table> <hr width=100% size=4 color=FF0000> "));
    }
    
    client.print(F("<hr width=100% size=4 color=0000FF>\r\n"));   // Linea Separatrice
    
    client.println(F("<table style=\"width: 100%\" border=\"0\"><tbody>"));
    client.println(F("<tr>"));
    HTMLButton("/PAR", "Parametri");
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
    switch (NetMas){
      case 0:
        client = server.available();
        if ((client) & (client.connected())){ // in ricezione
          NetMas=5;
          NetTo=millis()+3000;
        }
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
          }
    //**********************************************************************************************      
          
          
          
          
          //***************************************L1
          io1=NetCMDS.indexOf("/L1 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************
          
          //***************************************L2
          io1=NetCMDS.indexOf("/L2 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************


          //***************************************L3
          io1=NetCMDS.indexOf("/L3 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L4
          io1=NetCMDS.indexOf("/L4 ");
          io2=0;
          if (io1 > 0){
             rp=true;
          } // ******************************************************************

          //***************************************L5
          io1=NetCMDS.indexOf("/L5 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L6
          io1=NetCMDS.indexOf("/L6 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L7
          io1=NetCMDS.indexOf("/L7 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L8
          io1=NetCMDS.indexOf("/L8 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L9
          io1=NetCMDS.indexOf("/L9 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L10
          io1=NetCMDS.indexOf("/L10 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L11
          io1=NetCMDS.indexOf("/L11 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L12
          io1=NetCMDS.indexOf("/L12 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L13
          io1=NetCMDS.indexOf("/L13 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L14
          io1=NetCMDS.indexOf("/L14 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L15
          io1=NetCMDS.indexOf("/L15 ");
          io2=0;
          if (io1 > 0){
            rp=true;
          } // ******************************************************************

          //***************************************L16
          io1=NetCMDS.indexOf("/L16 ");
          io2=0;
          if (io1 > 0){
            rp=true;
            
          } // ******************************************************************
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




