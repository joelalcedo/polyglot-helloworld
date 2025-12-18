# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends groovy default-jre-headless && rm -rf /var/lib/apt/lists/*
COPY hello.groovy .
CMD ["sh", "-c", "groovy hello.groovy"]
