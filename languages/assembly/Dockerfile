# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends gcc binutils && rm -rf /var/lib/apt/lists/*
COPY hello.S .
RUN gcc -nostdlib -no-pie hello.S -o hello
CMD ["sh", "-c", "./hello"]
