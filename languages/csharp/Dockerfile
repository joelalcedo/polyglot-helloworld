# syntax=docker/dockerfile:1
FROM mcr.microsoft.com/dotnet/sdk:8.0
WORKDIR /app
COPY Program.cs .
RUN dotnet new console -o app --force && cp Program.cs app/Program.cs && dotnet build app -c Release -v q
CMD ["sh", "-c", "dotnet run --project app -c Release"]
