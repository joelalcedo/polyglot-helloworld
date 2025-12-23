# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends gnuplot && rm -rf /var/lib/apt/lists/*
COPY hello.gp .
CMD ["sh", "-c", "gnuplot -e \"print 'Hello, world!'\""]
