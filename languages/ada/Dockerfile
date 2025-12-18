# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends gnat && rm -rf /var/lib/apt/lists/*
COPY hello.adb .
RUN gnatmake -O2 -o hello hello.adb
CMD ["sh", "-c", "./hello"]
