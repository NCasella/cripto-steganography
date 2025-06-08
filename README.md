# cripto-steganography

El programa puede compilarse usando el Makefile con:

```make all```

Se generara el ejectuable `visualSSS`, que debe recibir los siguentes parametros:

- -d, --decode: Indica que se distribuira el contenido de una imagen en otras imagenes portadoras.
- -r, --recover: Indica que se debe requperar una imagen a partir de las imagenes portadoras.

- -k [num]: Indica la cantidad de imagenes portadoras necesarias para recuperar la imagen a ocultar.
- --secret [path]: Ubicacion de la imagen a distribuir (si es modo distribuir) o de la imagen a recuperar (si es modo recuperar).

Opcionalmente, el ejecutable recibe los siguientes parametros:

- --path [path]: Directorio en el que se encuentran las imagenes portadoras. De no especificarse se usara el directorio actual.

- -n [num]: Numero de imagenes portadoras en las que se distribuira la imagen a ocultar. De no especificarse, se usaran la cantidad de imagenes en el directorio especificado (o el actual si no fue especificado).

- -h, --help: Muestra los comandos disponibles.