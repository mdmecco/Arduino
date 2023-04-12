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





String BtnColor(int idCol) {
  if (idCol == 0) {
    return "#22ff22";
  } else {
    return "red";
  }
}



void WebServer (){

//********** SERVER ****************************************************************
    switch (NetMas){
      case 0:
        client = server.available();
        //client.setNoDelay(true);
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

//*************************************** LETTURA VALORE PARAMETRO ***********************************
      io1=NetCMDS.indexOf("NPPD");
      if (io1 > 0){
        //StParam();
      }

   
    //*****************************************  OTA  attivazione web  *****************************      
          io1=NetCMDS.indexOf("/OTA");
          io2=0;
          if (io1 > 0){
            rp=true;
            OTAActive= !OTAActive;
            OTABegin();            
          }
    //**********************************************************************************************      


    //*****************************************  Prametri  *****************************      
          io1=NetCMDS.indexOf("/PAR");
          io2=0;
          if (io1 > 0){
            NetMas=150;
            break;
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
          client.println(F("<!DOCTYPE HTML>"));
          client.print(F("<html><head><title>"));
          client.print(WEBTITPAGE);
          client.println(F("</title></head>"));
          client.println(F("<body>"));
          client.print(F("<b style=""font-size:10px"">"));
          client.print(PRGVER);
          client.print(F("</b>"));
          if (OTAActive){
            client.print(F("<table style=""width:100%"" border=1> <tr><th width=100% align=""center""> OTA Active </th> </tr> </table> <hr width=100% size=4 color=0000FF> "));
          }
          
          //************************************************* Fine Header ******************************************************
    
        //tabelle inizio

            client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));
            
            client.print(F("<form action=""/PAR"">"));
            client.print(F("<input type=""submit"" value=""Parametri"">"));
            client.print(F("</form>")); 
      
            client.print(F("<form action=""/OTA"">"));
            client.print(F("<input type=""submit"" value=""OTA"">"));
            client.print(F("</form>")); 

/*
            client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));
            
            client.println(F("<table style=\"width: 100%\" border=\"1\"><tbody>"));
            
            client.println(F("<tr>"));
            
            TButton(1);
            TButton(2);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(3);
            TButton(4);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(5);
            TButton(6);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(7);
            TButton(8);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(9);
            TButton(10);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(11);
            TButton(12);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(13);
            TButton(14);
            client.println(F("</tr>"));
            client.println(F("<tr>"));
            TButton(15);
            TButton(16);
            client.println(F("</tr>"));
            client.println(F("</tbody></table>"));
            */
            client.println(F("</body>\r\n</html>"));
            delay(100);
            NetMas=0; // funzione del timeout di ricezione
            client.stop();
        }
        break;

      case 150:
        {
          while (client.available()){
            char c = client.read();
          }

          //********************************************   PARAMETRI   *********************************************************    
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response
          client.println();
          client.println(F("<!DOCTYPE HTML>"));
          client.print(F("<html><head><title>"));
          client.print(WEBTITPAGE);
          client.print("  Parametri");
          client.println(F("</title></head>"));
          client.println(F("<body>"));
          client.print(F("<b style=""font-size:10px"">"));
          client.print(PRGVER);
          client.print(F("</b>"));
          
          void HeaderInfo();
          
          //************************************************* Fine Header ******************************************************
          client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));
          
          client.print(F("<form action=""/"">"));
          client.print(F("<input type=""submit"" value=""Main"">"));
          client.print(F("</form>")); 
    
          client.print(F("<hr width=100% size=4 color=FF0000>\r\n"));
          client.print(F("<table style=""width:100%"" border=1>"));
          client.println(F("<tr>"));
          /*
          ClParam(1);
          ClParam(2);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(3);
          ClParam(4);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(5);
          ClParam(6);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(7);
          ClParam(8);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(9); 
          ClParam(10);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(11);
          ClParam(12);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(13);
          ClParam(14);
          client.println(F("</tr>"));
          client.println(F("<tr>"));
          ClParam(15);
          ClParam(16);

          */
          client.println(F("</tr>"));
          client.println(F("</body>\r\n</html>"));
          delay(100);
          NetMas=200; // funzione del timeout di ricezione
          //client.stop();
        }
        break;

      case 200:
        client.stop();
        NetMas=0;
        break;
      default:
        NetMas=0;
    }
//***********************************************************************


}


