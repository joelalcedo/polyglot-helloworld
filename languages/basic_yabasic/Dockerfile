# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends yabasic && rm -rf /var/lib/apt/lists/*
COPY hello.bas .
CMD ["sh", "-c", "yabasic hello.bas"]
