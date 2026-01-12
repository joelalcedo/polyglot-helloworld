# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends regina-rexx && rm -rf /var/lib/apt/lists/*
COPY hello.rexx .
CMD ["sh", "-c", "rexx ./hello.rexx"]
