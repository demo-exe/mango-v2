#ifndef PUMP_H_
#define PUMP_H_


void initPump();
void waterPumpTask( void * pvParameters );

unsigned int getPumpingTimeMs();
void setPumpingTimeMs(unsigned int ms);


#endif // PUMP_H_