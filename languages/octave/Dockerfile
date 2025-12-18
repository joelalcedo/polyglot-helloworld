# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends octave && rm -rf /var/lib/apt/lists/*
COPY hello.m .
CMD ["sh", "-c", "octave --quiet --no-gui hello.m"]
