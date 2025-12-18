# syntax=docker/dockerfile:1
FROM rust:1.76
WORKDIR /app
COPY hello.rs .
RUN rustc hello.rs -O
CMD ["sh", "-c", "./hello"]
