# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends chicken-bin && rm -rf /var/lib/apt/lists/*
COPY hello.scm .
CMD ["sh", "-c", "csi -s hello.scm"]
