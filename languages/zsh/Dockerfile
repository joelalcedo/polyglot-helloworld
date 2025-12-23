# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends zsh && rm -rf /var/lib/apt/lists/*
COPY hello.zsh .
CMD ["sh", "-c", "zsh hello.zsh"]
