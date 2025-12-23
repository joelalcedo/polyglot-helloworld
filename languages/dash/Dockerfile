# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends dash && rm -rf /var/lib/apt/lists/*
COPY hello.sh .
CMD ["sh", "-c", "dash hello.sh"]
