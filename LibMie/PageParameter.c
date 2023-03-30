/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */




void ClParam(SLight & DLight){
      client.print(F("<td><form action=""/PAR"">\r\n"));
      client.print(F("<label>Durata Accensione in secondi canale:"));
      client.print(DLight.IdPinO);
      client.print(F("</label><input type=""text"" id=""-PD"));
      client.print(DLight.IdPinO);
      client.print(F("-"" name=""NPPD"));
      client.print(DLight.IdPinO);
      client.print(F("X"" value="""));
      client.print((DLight.TOn/1000),DEC);
      client.print(F(""" >"));
      client.print(F("<input type=""submit"" value="" Submit"">\r\n"));
      client.print(F("</form>\r\n</td>\r\n"));
}


