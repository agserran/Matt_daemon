FROM silkeh/clang:11-bullseye

#set timezone to europe/madrid
RUN ln -fs /usr/share/zoneinfo/Europe/Madrid /etc/localtime

RUN apt update && apt install -y \
    git \
    cmake \
    clang-format \
    clang-tools \
    python3 \
    python3-pip \
    valgrind \
    netcat \
    && apt-get clean && rm -rf /var/lib/apt/lists/*
    
RUN pip3 install cpplint