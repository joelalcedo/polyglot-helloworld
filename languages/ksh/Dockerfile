# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends ksh && rm -rf /var/lib/apt/lists/*
COPY hello.ksh .
CMD ["sh", "-c", "ksh hello.ksh"]
