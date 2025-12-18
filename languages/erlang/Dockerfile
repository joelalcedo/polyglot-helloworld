# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends erlang-base && rm -rf /var/lib/apt/lists/*
COPY hello.erl .
CMD ["sh", "-c", "escript hello.erl"]
