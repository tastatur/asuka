FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive
ENV LD_LIBRARY_PATH=/usr/local

RUN apt-get update
RUN apt-get install -y software-properties-common
RUN add-apt-repository -y ppa:rock-core/qt4
RUN apt-get update
RUN apt-get -y install cmake libpcre3-dev zlib1g-dev libgcrypt20-dev libicu-dev python  build-essential libyaml-dev swig python-dev pkg-config libeigen3-dev libboost-all-dev libqt4-dev git build-essential libeigen3-dev libyaml-dev libfftw3-dev libavcodec-dev libavformat-dev libavutil-dev libavresample-dev python-dev libsamplerate0-dev libtag1-dev libchromaprint-dev python-six
RUN apt-get clean

RUN mkdir /3dparty
WORKDIR /3dparty

RUN git clone https://github.com/MTG/gaia.git
WORKDIR /3dparty/gaia

RUN ./waf configure
RUN ./waf
RUN ./waf install

WORKDIR /3rdparty
RUN git clone https://github.com/MTG/essentia.git
WORKDIR /3rdparty/essentia
RUN git checkout v2.1_beta5

RUN CXXFLAGS=-I/usr/include/eigen3 ./waf configure --with-examples --with-gaia --with-vamp
RUN ./waf
RUN ./waf install

WORKDIR /3rdparty
RUN git clone https://github.com/artyom-beilis/cppcms.git
WORKDIR /3rdparty/cppcms

RUN mkdir build && cd build && cmake ../ && make -j && make install

RUN mkdir /asuka
WORKDIR /asuka

VOLUME ["/data"]

ADD . /asuka
WORKDIR /asuka

RUN cd webui && ./buildCmsViews.sh && cd ..
RUN mkdir -p build && cd build && cmake ../ && make

EXPOSE 8080

USER nobody
CMD [ "/asuka/webui/runwebui.sh" ]
