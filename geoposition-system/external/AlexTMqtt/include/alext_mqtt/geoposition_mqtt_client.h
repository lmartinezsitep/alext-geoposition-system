#pragma once

#include "pub_sub_mqtt_client.h"

namespace alexTMqtt {

	class GeopositionMqttClient : public PubSubMqttClient {

	public:
		GeopositionMqttClient(const string& instanceId, const string& brokerUri, const string& persistDir);

		void subscribeToGeopositionData(std::function<void(const std::string&)> callback);

		void publishAdquisitionINSData(const std::string& message);

		void publishAdquisitionGNSSData(const std::string& message);
	};
}
