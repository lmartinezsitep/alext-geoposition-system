#!/bin/sh

#pause
./docker-pause-pro.sh

#delete
docker compose -f docker-compose.yml --env-file=./docker-pro.env down
