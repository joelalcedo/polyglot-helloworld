# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache janet
COPY hello.janet .
CMD ["sh", "-c", "janet hello.janet"]
