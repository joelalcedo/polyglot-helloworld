# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends lua5.4 && rm -rf /var/lib/apt/lists/*
COPY hello.lua .
CMD ["sh", "-c", "lua5.4 hello.lua"]
