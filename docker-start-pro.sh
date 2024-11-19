#!/bin/sh

#compile
./docker-build-pro.sh

#launch
docker compose -f docker-compose.yml --env-file=./docker-pro.env up -d 
