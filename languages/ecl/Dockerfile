# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends ecl && rm -rf /var/lib/apt/lists/*
COPY hello.lisp .
CMD ["sh", "-c", "ecl -load hello.lisp -eval \"(quit)\""]
