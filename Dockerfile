
FROM debian:12-slim AS builder
ENV DEBIAN_FRONTEND=noninteractive

ARG APP_ENV=UNSET
ENV APP_LOSP_ENV=${APP_ENV}

WORKDIR /build

RUN apt update && \
    apt install -y build-essential wget curl git apt-transport-https lsb-release software-properties-common gnupg

RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 20 all

RUN git clone https://github.com/Danukeru/llvm-obfs-str-pass-2024 losp24 && \
    cd losp24 && \
    bash run.sh && cd demo && bash run.sh

RUN tar zcvf losp24.tar.gz losp24


FROM scratch
COPY --from=builder /build/losp24.tar.gz /

