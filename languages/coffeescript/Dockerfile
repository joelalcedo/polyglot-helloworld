# syntax=docker/dockerfile:1
FROM node:20-alpine
WORKDIR /app
RUN npm i -g coffeescript
COPY hello.coffee .
CMD ["sh", "-c", "coffee hello.coffee"]
