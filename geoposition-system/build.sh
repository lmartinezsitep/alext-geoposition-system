#!/bin/sh

# Crear el directorio 'build' si no existe
mkdir -p build

# Eliminar archivos solo si existen
[ -f build/sitep ] && rm -f build/sitep
[ -f build/geonumerics ] && rm -f build/geonumerics

# Ejecutar cmake y verificar si tuvo éxito
if cmake .; then
    # Ejecutar make y verificar si tuvo éxito
    if make; then
        # Mover los archivos generados, verificando si existen
        [ -f sitep ] && mv sitep build/sitep
        [ -f geonumerics ] && mv geonumerics build/geonumerics
    else
        echo "Error: make falló"
        exit 1
    fi
else
    echo "Error: cmake falló"
    exit 1
fi
