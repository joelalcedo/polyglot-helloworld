# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache ocaml build-base
COPY hello.ml .
RUN ocamlopt -O2 -o hello hello.ml
CMD ["sh", "-c", "./hello"]
