#include "loggermqtt.h"
#include <MQTTClient.h>
#include <cstring>

/** @brief Publishes logger data via the MQTT protocol
 *  @author Robert Miller
 *  @date 12Mar2022
 *  @param char * mqttdata
 *  @return int
 */
int DlPublishLoggerData(const char * mqttdata) {
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;
	int rc;
	MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
  if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
		fprintf(stdout,"Failed to connect, return code %d\n", rc);
		return -1;
  }
	pubmsg.payload = (void *) mqttdata;
  pubmsg.payloadlen = strlen(mqttdata);
  pubmsg.qos = QOS;
  pubmsg.retained = 0;
	MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
  fprintf(stdout,"Waiting for up to %d seconds for publication of %s\n on topic %s for client with ClientID: %s\n", (int)(TIMEOUT/1000), mqttdata, TOPIC, CLIENTID);
  rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
  fprintf(stdout,"Message with delivery token %d delivered\n", token);
  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);
  return rc;
}
