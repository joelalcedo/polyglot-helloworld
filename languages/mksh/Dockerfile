# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends mksh && rm -rf /var/lib/apt/lists/*
COPY hello.mksh .
CMD ["sh", "-c", "mksh hello.mksh"]
