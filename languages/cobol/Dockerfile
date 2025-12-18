# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends gnucobol build-essential && rm -rf /var/lib/apt/lists/*
COPY hello.cob .
RUN cobc -x -free hello.cob -o hello
CMD ["sh", "-c", "./hello"]
