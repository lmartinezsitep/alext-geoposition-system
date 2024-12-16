#!/bin/sh

#pause
./docker-pause-pro.sh

#delete
docker compose -f docker-compose-mosquito.yml --env-file=./docker-pro.env down
