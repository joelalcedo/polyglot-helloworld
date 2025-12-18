# syntax=docker/dockerfile:1
FROM esolang/emojicode
WORKDIR /app
COPY hello.emojic .
RUN emojicodec hello.emojic
CMD ["sh", "-c", "./hello"]
