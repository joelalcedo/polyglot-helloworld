# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends valac libglib2.0-dev && rm -rf /var/lib/apt/lists/*
COPY hello.vala .
RUN valac -O2 -o hello hello.vala
CMD ["sh", "-c", "./hello"]
