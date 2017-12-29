#include <stdio.h>
#include <string.h>

#include "procBle.h"
#include "UttecBle.h"

bool fgBleRxReady=false;
uint8_t ucgdRxData[20];

UttecBle::UttecBle(){
}

bool UttecBle::isBleRxReady(){
    return fgBleRxReady;
}

void UttecBle::clearBleRxReady(){
    fgBleRxReady=false;
}

uint8_t* UttecBle::getBleRxData(){
    return ucgdRxData;
}
