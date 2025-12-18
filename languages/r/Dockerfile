# syntax=docker/dockerfile:1
FROM r-base:latest
WORKDIR /app
COPY hello.R .
CMD ["sh", "-c", "Rscript hello.R"]
