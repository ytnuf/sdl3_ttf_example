
FROM fedora:41

WORKDIR /opt
RUN dnf upgrade -y
RUN dnf install -y make automake gcc gcc-c++ kernel-devel git cmake curl zip unzip tar autoconf
RUN dnf install -y libX11-devel libXft-devel libXext-devel libXrandr-devel libXi-devel
RUN git clone https://github.com/microsoft/vcpkg.git
RUN vcpkg/bootstrap-vcpkg.sh -disableMetrics
RUN ln -s /opt/vcpkg/vcpkg /usr/local/bin
ENV VCPKG_ROOT "/opt/vcpkg"

WORKDIR /usr/local/src
COPY . ./
RUN cmake --workflow --preset default --fresh
RUN ./build/example_proj
