#pragma once

#include "pub_sub_mqtt_client.h"

namespace alexTMqtt {

	class AlexTServerMqttClient : public PubSubMqttClient {

	public:
		AlexTServerMqttClient(const string& instanceId, const string& brokerUri, const string& persistDir);

		void publishGeopositionData(const std::string& message);

		void publishAdquisitionRawData(const std::string& message);

		void subscribeToGeopositionData(function<void(const std::string&)> callback);

		void subscribeToAdquisitionRawData(function<void(const std::string&)> callback);

	};
}
