#!/bin/sh

#pause and delete containers
./docker-end-pro.sh

#delete images
docker image rmi alext-geoposition-system-lumaro-geoposition-system-arm32
docker image rmi alext-geoposition-system-lumaro-mosquitto

#delete volume
docker volume rm alext-geoposition-system-lumaro_certs_data
docker volume rm alext-geoposition-system-lumaro_mosquitto_data
docker volume prune -a -f
