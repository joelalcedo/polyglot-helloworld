# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache perl
COPY hello.pl .
CMD ["sh", "-c", "perl hello.pl"]
