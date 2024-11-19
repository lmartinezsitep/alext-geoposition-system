#pragma once

#include "mqtt_client_callback.h"
#include <mqtt/client.h>

namespace alexTMqtt
{
	struct ClientConnectOptions {
		bool ssl = false;
		string trustStore = "";
		string keyStore = "";
		bool ws = false;
		string username = "";
		string password = "";
		bool cleanStart = false;
	};

	class PubSubMqttClient
	{

	public:
		PubSubMqttClient(const string& clientId, const string& brokerUri, const string& persistDir, const string& username, const string& password);

		PubSubMqttClient(const string& clientId, const string& brokerUri, const string& persistDir);

		// Delete copy constructor and copy assignment operator
		PubSubMqttClient(const PubSubMqttClient&) = delete;
		PubSubMqttClient& operator=(const PubSubMqttClient&) = delete;

		// Move constructor and move assignment operator
		PubSubMqttClient(PubSubMqttClient&&) noexcept = default;
		PubSubMqttClient& operator=(PubSubMqttClient&&) noexcept = default;

		const string& getClientId() const;

		void connect(const ClientConnectOptions& connOpts);

		void disconnect();

		~PubSubMqttClient();

	protected:
		std::string clientId_;
		mqtt::client client_;
		MqttClientCallback callback_;

	private:
		static void createPersistDirIfNotExist(const std::string& persistDir);
		static mqtt::connect_options create_default_mqtt_connect_options(const ClientConnectOptions& connOpts);
	};
}
