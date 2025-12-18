# syntax=docker/dockerfile:1
FROM julia:1.10
WORKDIR /app
ENV PATH="/usr/local/julia/bin:$PATH"
COPY hello.jl .
CMD ["sh", "-c", "julia hello.jl"]
