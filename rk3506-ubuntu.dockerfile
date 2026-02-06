FROM ubuntu:jammy
SHELL ["/bin/bash", "-c"]
ENTRYPOINT echo -e "\033[0;32m Entered docker environment \033[0m" && /bin/bash

ENV DEBIAN_FRONTEND=non-interactive
ENV TZ=Europe/Berlin
RUN apt update && \
      apt -y install sudo tzdata

RUN \
    groupadd -g 1000 lyra && useradd -u 1000 -g lyra -G sudo -m -s /bin/bash lyra && \
    sed -i /etc/sudoers -re 's/^%sudo.*/%sudo ALL=(ALL:ALL) NOPASSWD: ALL/g' && \
    sed -i /etc/sudoers -re 's/^root.*/root ALL=(ALL:ALL) NOPASSWD: ALL/g' && \
    sed -i /etc/sudoers -re 's/^#includedir.*/## **Removed the include directive** ##"/g' && \
    echo "lyra ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers && \
    echo "Customized the sudoers file for passwordless access to the lyra user!" && \
    echo "lyra user:";  su - lyra -c id

RUN apt update && apt install -y git ssh make gcc libssl-dev \
    liblz4-tool expect expect-dev g++ patchelf chrpath gawk texinfo chrpath \
    diffstat binfmt-support qemu-user-static live-build bison flex fakeroot \
    cmake unzip device-tree-compiler ncurses-dev libelf-dev bsdmainutils \
    libgucharmap-2-90-dev bzip2 expat gpgv2 cpp-aarch64-linux-gnu libgmp-dev \
    gcc-arm-linux-gnueabihf libmpc-dev bc python-is-python3 python2 rsync \
    p7zip-full p7zip-rar nano
