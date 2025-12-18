# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache yabasic
COPY hello.bas .
CMD ["sh", "-c", "yabasic hello.bas"]
