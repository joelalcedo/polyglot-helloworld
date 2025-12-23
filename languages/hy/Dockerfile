# syntax=docker/dockerfile:1
FROM python:3.12-slim
WORKDIR /app
RUN pip install --no-cache-dir hy
COPY hello.hy .
CMD ["sh", "-c", "hy hello.hy"]
