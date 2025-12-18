# syntax=docker/dockerfile:1
FROM eclipse-temurin:17-jdk
WORKDIR /app
RUN apt-get update && apt-get install -y --no-install-recommends scala && rm -rf /var/lib/apt/lists/*
COPY Hello.scala .
RUN scalac Hello.scala
CMD ["sh", "-c", "scala Hello"]
