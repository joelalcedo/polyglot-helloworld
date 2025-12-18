# syntax=docker/dockerfile:1
FROM mcr.microsoft.com/dotnet/sdk:8.0
WORKDIR /app
COPY Program.vb .
RUN dotnet new console -lang "VB" -o app --force && cp Program.vb app/Program.vb && dotnet build app -c Release -v q
CMD ["sh", "-c", "dotnet run --project app -c Release"]
