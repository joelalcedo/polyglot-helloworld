# syntax=docker/dockerfile:1
FROM node:20-alpine
WORKDIR /app
RUN npm i -g typescript
COPY hello.ts .
RUN tsc hello.ts --target ES2020 --module commonjs --outDir dist
CMD ["sh", "-c", "node dist/hello.js"]
