# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends gforth && rm -rf /var/lib/apt/lists/*
COPY hello.fs .
CMD ["sh", "-c", "gforth hello.fs"]
