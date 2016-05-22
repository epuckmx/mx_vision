FROM gcc:4.9
COPY . /usr/src/mxvision
WORKDIR /usr/src/mxvision
RUN apt-get update && apt-get install -y curl
RUN g++ -D MX_DEV -o mxvision main.cpp mx_vision.c
CMD ["./mxvision"]
