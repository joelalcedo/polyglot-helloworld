# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache lua5.4
COPY hello.lua .
CMD ["sh", "-c", "lua5.4 hello.lua"]
