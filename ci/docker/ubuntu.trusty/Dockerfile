# Base on Trusty (14.04) - Travis default at June 2019
FROM ubuntu:trusty

# Install basic dependencies
RUN apt-get update
RUN apt-get install -y g++
RUN apt-get install -y bison git libtool make automake g++ pkg-config
RUN apt-get install -y libfreetype6-dev libftgl-dev
RUN apt-get install -y curl

# Install higher Qt version than that available from Ubuntu repos
RUN curl https://keyserver.ubuntu.com/pks/lookup?op=get\&search=0x10C56D0DE9977759 | apt-key add -
RUN echo "deb http://ppa.launchpad.net/beineri/opt-qt-5.10.1-trusty/ubuntu trusty main" | tee -a /etc/apt/sources.list
RUN apt-get update
RUN apt-get install -y qt510base

# Original Qt packages - for reference
# RUN apt-get install -y qttools5-dev-tools libqt5gui5 libqt5widgets5 libqt5opengl5 qtbase5-dev qt5-default

# Clone repo
RUN git clone --depth  1 https://github.com/trisyoungs/dissolve.git ./dissolve

# Copy in build script, and run
COPY build.sh /
RUN /build.sh
