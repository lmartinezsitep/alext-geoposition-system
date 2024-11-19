#!/bin/sh
docker compose -f docker-compose.yml --env-file=./docker-pro.env build
