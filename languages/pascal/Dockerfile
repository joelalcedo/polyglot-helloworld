# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends fp-compiler && rm -rf /var/lib/apt/lists/*
COPY hello.pas .
RUN fpc -O2 hello.pas
CMD ["sh", "-c", "./hello"]
