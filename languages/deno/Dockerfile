# syntax=docker/dockerfile:1
FROM denoland/deno:alpine
WORKDIR /app
COPY hello.ts .
CMD ["sh", "-c", "deno run --allow-all hello.ts"]
