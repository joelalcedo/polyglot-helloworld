# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache nim build-base
COPY hello.nim .
RUN nim c -d:release -o:hello hello.nim
CMD ["sh", "-c", "./hello"]
