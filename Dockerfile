FROM gcc:4.9
COPY . /usr/src/mxvision
WORKDIR /usr/src/mxvision
RUN apt-get update && apt-get install -y libcurl4-gnutls-dev
RUN g++ -D MX_DEV -o mxvision main.cpp empirical.cpp mx_vision.c -lcurl
CMD ["./mxvision"]
