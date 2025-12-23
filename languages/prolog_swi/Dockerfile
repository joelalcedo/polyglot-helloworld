# syntax=docker/dockerfile:1
FROM debian:bookworm-slim
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends swi-prolog && rm -rf /var/lib/apt/lists/*
COPY hello.pl .
CMD ["sh", "-c", "swipl -q -s hello.pl -t main"]
