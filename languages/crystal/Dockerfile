# syntax=docker/dockerfile:1
FROM crystallang/crystal:latest
WORKDIR /app
COPY hello.cr .
RUN crystal build hello.cr -o hello
CMD ["sh", "-c", "./hello"]
