# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
COPY hello.awk .
CMD ["sh", "-c", "awk '{print \"Hello, world!\"}' hello.awk"]
