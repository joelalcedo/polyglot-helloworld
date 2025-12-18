# syntax=docker/dockerfile:1
FROM eclipse-temurin:17-jdk
WORKDIR /app
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends wget unzip && rm -rf /var/lib/apt/lists/* && KOTLIN_VER=2.0.21 && wget -q https://github.com/JetBrains/kotlin/releases/download/v${KOTLIN_VER}/kotlin-compiler-${KOTLIN_VER}.zip -O /tmp/kotlin.zip && unzip -q /tmp/kotlin.zip -d /opt && ln -sf /opt/kotlinc/bin/kotlinc /usr/local/bin/kotlinc && rm -f /tmp/kotlin.zip
ENV PATH="/usr/local/bin:$PATH"
COPY Hello.kt .
RUN kotlinc Hello.kt -include-runtime -d hello.jar
CMD ["sh", "-c", "java -jar hello.jar"]
