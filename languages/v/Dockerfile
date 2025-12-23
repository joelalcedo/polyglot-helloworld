# syntax=docker/dockerfile:1
FROM thevlang/vlang:alpine
WORKDIR /app
COPY hello.v .
RUN v -prod -o hello hello.v
CMD ["sh", "-c", "./hello"]
