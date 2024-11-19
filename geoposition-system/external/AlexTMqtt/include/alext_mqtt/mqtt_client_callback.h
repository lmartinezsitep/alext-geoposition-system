#pragma once

#include <mqtt/client.h>

using namespace std;

namespace alexTMqtt {

	class MqttClientCallback : public mqtt::callback {

	public:
		MqttClientCallback(mqtt::client& client);

		void add_message_listener(const std::string& topic, std::function<void(const std::string&)> callback);

		~MqttClientCallback();

	private:
		void message_arrived(mqtt::const_message_ptr msg);

		mqtt::client& client_;
		std::unordered_map<std::string, std::list<std::function<void(const std::string&)>>> message_callbacks_;
	};
}
