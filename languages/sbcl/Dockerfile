# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends sbcl && rm -rf /var/lib/apt/lists/*
COPY hello.lisp .
CMD ["sh", "-c", "sbcl --noinform --script hello.lisp"]
