# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache bc
COPY hello.bc .
CMD ["sh", "-c", "bc -q hello.bc"]
