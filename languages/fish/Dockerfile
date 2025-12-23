# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends fish && rm -rf /var/lib/apt/lists/*
COPY hello.fish .
CMD ["sh", "-c", "fish hello.fish"]
