# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends pike8.0 && rm -rf /var/lib/apt/lists/*
COPY hello.pike .
CMD ["sh", "-c", "pike8.0 hello.pike"]
