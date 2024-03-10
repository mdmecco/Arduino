/* ========================================================================== */
/*                                                                            */
/*   temperatura.c                                                               */
/*   (c) 2024 Author  Maurizio Meschiari                                      */ 
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */


void ReadTemp () {
    if (millis() > TimerDeltaReadT ){
      TimerDeltaReadT= millis() + DeltaRead;
      switch (IdT) {
        case 0:
            TCN1.readRTD();
            tT1=TCN1.temperature(RNOMINAL, RREF);
            IdT=IdT+1;
            break;
        case 1:
            TCN2.readRTD();
            tT2=TCN2.temperature(RNOMINAL, RREF);
            IdT=IdT+1;
            break;
        case 2:
            TCN3.readRTD();
            tT3=TCN3.temperature(RNOMINAL, RREF);
            IdT=IdT+1;
            break;
        case 3:
            TCN4.readRTD();       // Sensore della stufa
            tT4=TCN4.temperature(RNOMINAL, RREF);
            IdT=IdT+1;
            break;
        case 4:
            TCN5.readRTD();
            tT5=TCN5.temperature(RNOMINAL, RREF);
            IdT=IdT+1;
            break;
        case 5:
            TCN6.readRTD();
            tT6=TCN6.temperature(RNOMINAL, RREF);
            IdT=IdT+1;
            break;
        case 6:
            TCN7.readRTD();     // Sensore dell'accumulo basso
            tT7=TCN7.temperature(RNOMINAL, RREF);
            if (tT7 > TbMax){
              TbMax=tT7;
            }
            IdT=IdT+1;
            break;
        case 7:
            TCN8.readRTD();
            tT8=TCN8.temperature(RNOMINAL, RREF);
            IdT=0;
            break;
        default:
            IdT=0;
        }
    }
            
}
