# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends luajit && rm -rf /var/lib/apt/lists/*
COPY hello.lua .
CMD ["sh", "-c", "luajit hello.lua"]
