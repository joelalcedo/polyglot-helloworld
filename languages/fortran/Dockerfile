# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache build-base gfortran
COPY hello.f90 .
RUN gfortran hello.f90 -o hello
CMD ["sh", "-c", "./hello"]
