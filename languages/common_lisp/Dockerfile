# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache sbcl
COPY hello.lisp .
CMD ["sh", "-c", "sbcl --script hello.lisp"]
