# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends clojure default-jre-headless && rm -rf /var/lib/apt/lists/*
COPY hello.clj .
CMD ["sh", "-c", "clojure hello.clj"]
