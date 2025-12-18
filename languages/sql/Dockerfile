# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache sqlite
COPY hello.sql .
CMD ["sh", "-c", "sqlite3 :memory: < hello.sql"]
