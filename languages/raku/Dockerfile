# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends rakudo && rm -rf /var/lib/apt/lists/*
COPY hello.raku .
CMD ["sh", "-c", "raku hello.raku"]
