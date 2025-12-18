# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache nim
COPY hello.nims .
CMD ["sh", "-c", "nim e hello.nims"]
