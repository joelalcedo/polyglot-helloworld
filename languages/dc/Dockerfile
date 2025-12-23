# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends dc && rm -rf /var/lib/apt/lists/*
COPY hello.dc .
CMD ["sh", "-c", "dc -f hello.dc"]
