# syntax=docker/dockerfile:1
FROM node:20-alpine
WORKDIR /app
RUN npm i -g livescript
COPY hello.ls .
CMD ["sh", "-c", "lsc hello.ls"]
