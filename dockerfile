FROM rcarmo/alpine-python:3.6 as base
RUN apk update
RUN apk add linux-headers zeromq-dev 

FROM base as libcsp
RUN git clone https://github.com/maekos/libcsp
WORKDIR libcsp
RUN ./waf configure --with-os=posix --enable-examples --enable-if-zmqhub --install-csp --prefix=install
RUN ./waf build install

