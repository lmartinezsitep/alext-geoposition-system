#!/bin/sh

# Eliminar archivos solo si existen
[ -d external ] && rm -rf external

unzip external_rp4.zip 