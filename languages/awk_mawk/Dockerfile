# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends mawk && rm -rf /var/lib/apt/lists/*
COPY hello.awk .
CMD ["sh", "-c", "mawk -f hello.awk"]
