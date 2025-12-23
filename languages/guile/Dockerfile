# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends guile-3.0 && rm -rf /var/lib/apt/lists/*
COPY hello.scm .
CMD ["sh", "-c", "guile -s hello.scm"]
