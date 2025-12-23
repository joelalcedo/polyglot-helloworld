# syntax=docker/dockerfile:1
FROM erlang:27-alpine
WORKDIR /app
COPY hello.erl .
RUN erlc hello.erl
CMD ["sh", "-c", "erl -noshell -s hello main -s init stop"]
