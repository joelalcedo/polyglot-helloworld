# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN apk add --no-cache wget tar xz libc-dev && wget -qO- https://ziglang.org/download/0.12.0/zig-linux-aarch64-0.12.0.tar.xz | tar -xJ && mv zig-linux-aarch64-0.12.0 /zig && ln -sf /zig/zig /usr/local/bin/zig
COPY hello.zig .
RUN zig build-exe hello.zig -O ReleaseSafe -femit-bin=hello
CMD ["sh", "-c", "./hello"]
