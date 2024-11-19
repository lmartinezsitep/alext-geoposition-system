FROM eclipse-mosquitto:2.0.18

USER root

COPY start-mosquitto.sh /usr/local/bin/start-mosquitto.sh
RUN chmod +x /usr/local/bin/start-mosquitto.sh

COPY config/ /mosquitto/config/

ENTRYPOINT ["/docker-entrypoint.sh", "/usr/local/bin/start-mosquitto.sh"]
