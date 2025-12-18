# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache jq
COPY hello.jq .
CMD ["sh", "-c", "jq -nr -f hello.jq"]
