# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends jsonnet && rm -rf /var/lib/apt/lists/*
COPY hello.jsonnet .
CMD ["sh", "-c", "jsonnet -S hello.jsonnet"]
