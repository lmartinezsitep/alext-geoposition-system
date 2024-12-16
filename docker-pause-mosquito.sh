#!/bin/sh

#launch
docker compose -f docker-compose-mosquito.yml --env-file=./docker-pro.env stop
