# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends gcc gobjc libobjc-12-dev && rm -rf /var/lib/apt/lists/*
COPY hello.m .
RUN gcc -x objective-c -O2 -o hello hello.m -lobjc
CMD ["sh", "-c", "./hello"]
