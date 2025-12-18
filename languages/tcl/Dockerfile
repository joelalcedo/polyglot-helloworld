# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache tcl
COPY hello.tcl .
CMD ["sh", "-c", "tclsh hello.tcl"]
