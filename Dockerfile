FROM devkitpro/devkitarm:latest
MAINTAINER jski "jski185@gmail.com"

ADD . /

ENTRYPOINT ["make"]