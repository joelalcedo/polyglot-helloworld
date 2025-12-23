# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends expect && rm -rf /var/lib/apt/lists/*
COPY hello.exp .
CMD ["sh", "-c", "expect hello.exp"]
