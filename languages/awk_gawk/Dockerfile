# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends gawk && rm -rf /var/lib/apt/lists/*
COPY hello.awk .
CMD ["sh", "-c", "gawk -f hello.awk"]
