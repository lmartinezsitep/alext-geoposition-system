#!/bin/sh
echo "Preparing Mosquitto..."

chown -R mosquitto:mosquitto /mosquitto

echo "Executing Mosquitto..."

if [ "$ENV" = "dev" ]; then
  exec mosquitto -c /mosquitto/config/mosquitto.dev.conf
else
  exec mosquitto -c /mosquitto/config/mosquitto.conf
fi
