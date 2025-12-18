# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends fp-compiler && rm -rf /var/lib/apt/lists/*
COPY hello.pas .
CMD ["sh", "-c", "fpc -O2 hello.pas"]
