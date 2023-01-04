#ifndef NVS_H_
#define NVS_H_

#include <stdint.h>

void initNVS();

uint32_t getPumpingTimeMs();
void setPumpingTimeMs(uint32_t ms);

#endif // NVS_H_