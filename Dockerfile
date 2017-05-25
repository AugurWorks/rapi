FROM ubuntu:16.04

RUN mkdir -p /opt/app
WORKDIR /opt/app

RUN apt-get update && \
  apt-get install -y g++ zlib1g-dev libkrb5-dev curl && \
  curl -sL https://deb.nodesource.com/setup_6.x | bash - && \
  apt-get install -y nodejs git && \
  apt-get clean

COPY package.json /opt/app

RUN npm install

COPY . /opt/app

WORKDIR datastore/samples

RUN g++ -O3 -DLINUX -D_REENTRANT -Wall -Wno-sign-compare -Wno-write-strings -Wpointer-arith -Winline -Wno-deprecated -fno-strict-aliasing -I../include -o SampleMD ../samples/SampleMD.cpp -L../linux-gnu/lib -lRApiPlus-optimize -lOmneStreamEngine-optimize -lOmneChannel-optimize -lOmneEngine-optimize -l_api-optimize -l_apipoll-stubs-optimize -l_kit-optimize -lssl -lcrypto -L/usr/lib64 -lz -L/usr/kerberos/lib -lkrb5 -lk5crypto -lcom_err -lresolv -lm -lpthread -lrt -ldl

WORKDIR /opt/app

EXPOSE 3000

CMD npm start
