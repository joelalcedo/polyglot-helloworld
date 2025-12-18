# syntax=docker/dockerfile:1
FROM mcr.microsoft.com/dotnet/sdk:8.0
WORKDIR /app
COPY Program.fs .
RUN dotnet new console -lang "F#" -o app --force && cp Program.fs app/Program.fs && dotnet build app -c Release -v q
CMD ["sh", "-c", "dotnet run --project app -c Release"]
