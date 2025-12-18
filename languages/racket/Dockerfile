# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends racket && rm -rf /var/lib/apt/lists/*
COPY hello.rkt .
CMD ["sh", "-c", "racket hello.rkt"]
