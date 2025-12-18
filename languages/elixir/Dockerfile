# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends elixir && rm -rf /var/lib/apt/lists/*
COPY hello.exs .
CMD ["sh", "-c", "elixir hello.exs"]
