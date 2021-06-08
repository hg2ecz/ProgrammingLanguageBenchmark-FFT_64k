FROM debian:stable-slim
WORKDIR /app
COPY ./dist-arm32/ .

RUN apt-get update
RUN apt-get install -y libkrb5-3 libgssapi-krb5-2 libicu63

ENTRYPOINT ["./fft-benchmark"]
