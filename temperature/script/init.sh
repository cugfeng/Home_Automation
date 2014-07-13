#!/bin/sh

sudo modprobe wire
sudo modprobe w1-gpio
sudo modprobe w1-therm

TOP=`pwd | sed 's/temperature/temperature /' | awk '{print $1}'`
mkdir -p $TOP/record

