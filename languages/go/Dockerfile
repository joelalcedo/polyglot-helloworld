# syntax=docker/dockerfile:1
FROM golang:1.23-alpine
WORKDIR /app
COPY hello.go .
RUN go build -o hello hello.go
CMD ["sh", "-c", "./hello"]
