# syntax=docker/dockerfile:1
FROM mcr.microsoft.com/powershell:7.4-debian-12
WORKDIR /app
COPY hello.ps1 .
CMD ["sh", "-c", "pwsh -File hello.ps1"]
