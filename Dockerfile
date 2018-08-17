FROM devkitpro/devkita64
MAINTAINER elelphatp "elephantp@elephantp.blog"

RUN dkp-pacman -Syyu --noconfirm devkitARM && \
    dkp-pacman -Scc --noconfirm

ENV DEVKITARM=${DEVKITPRO}/devkitARM

WORKDIR /developer

ENTRYPOINT ["make"]
