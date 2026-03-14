# Use the official image as a parent image.
FROM ubuntu:18.04

MAINTAINER felix kübler

# Set environment to not ask for user input during installation
ARG DEBIAN_FRONTEND=noninteractive

# Set the working directory.
WORKDIR /usr/src/turret

# Update
RUN apt-get update --fix-missing && apt-get upgrade -y
RUN apt-get update --fix-missing

# Install Software components
RUN apt-get install -y git
RUN apt-get install -y python3
RUN apt-get install -y python3-pip
RUN apt-get install -y python3-flask
RUN apt-get install -y python3-opencv

RUN pip3 install pyserial

COPY ./TurretApp .

CMD python3 main.py

