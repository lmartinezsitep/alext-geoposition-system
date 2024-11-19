#!/bin/sh

#pause
./docker-delete-pro.sh

#delete
docker image prune -a -f
docker volume prune -a -f
docker network prune -f

docker system prune --volumes -f
