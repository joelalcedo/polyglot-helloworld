# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache build-base
COPY hello.c .
RUN cc -O2 -o hello hello.c
CMD ["sh", "-c", "./hello"]
