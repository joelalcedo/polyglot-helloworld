# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache g++
COPY hello.cpp .
RUN g++ -O2 -o hello hello.cpp
CMD ["sh", "-c", "./hello"]
