# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends gdc && rm -rf /var/lib/apt/lists/*
COPY hello.d .
RUN gdc -O2 -o hello hello.d
CMD ["sh", "-c", "./hello"]
