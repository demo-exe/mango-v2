#ifndef THINGSBOARD_H_
#define THINGSBOARD_H_


#define BROKER_IP "192.168.0.13" // raspberry ip
#define TOKEN "Mango_test_token"

#define RPC_REQ_TOPIC "v1/devices/me/rpc/request/+"
#define RPC_REQ_FMT "v1/devices/me/rpc/request/%d"
#define RPC_RES_FMT "v1/devices/me/rpc/response/%d"

#define TELEMETRY_TOPIC "v1/devices/me/telemetry"


void thingsboardTask(void * pvParameters);

void handle_request(const char * data, const char * response_topic);


#endif // THINGSBOARD_H_