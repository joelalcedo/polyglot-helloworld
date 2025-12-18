# syntax=docker/dockerfile:1
FROM swipl:latest
WORKDIR /app
COPY hello.pl .
CMD ["sh", "-c", "swipl -q -f hello.pl -t main -g halt"]
