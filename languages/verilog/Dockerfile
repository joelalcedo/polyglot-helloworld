# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends iverilog && rm -rf /var/lib/apt/lists/*
COPY hello.v .
RUN iverilog -o hello hello.v
CMD ["sh", "-c", "vvp hello"]
