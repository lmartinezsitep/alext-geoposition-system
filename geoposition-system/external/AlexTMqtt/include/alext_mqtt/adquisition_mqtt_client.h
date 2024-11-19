#pragma once

#include "pub_sub_mqtt_client.h"

namespace alexTMqtt {

	class AdquisitionMqttClient : public PubSubMqttClient {

	public:
		AdquisitionMqttClient(const string& instanceId, const string& brokerUri, const string& persistDir);

		void subscribeToAdquisitionINSData(std::function<void(const std::string&)> callback);

		void subscribeToAdquisitionGNSSData(std::function<void(const std::string&)> callback);

		void publishGeopositionData(const std::string& message);
	};
}
