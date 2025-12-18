# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache bash
COPY hello.sh .
CMD ["sh", "-c", "bash hello.sh"]
