# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache openjdk17-jdk
COPY Hello.java .
RUN javac Hello.java
CMD ["sh", "-c", "java Hello"]
