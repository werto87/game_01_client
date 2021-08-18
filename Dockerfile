FROM silkeh/clang:12

SHELL ["/bin/bash", "-c"] 

COPY . /client

RUN apt update

RUN apt-get -y install python3-pip

RUN apt-get -y  install debhelper

# dpkg -i ../corrade*.deb returns an error because of signing but it still works so we ignore the "error" with : exit 0
RUN git clone git://github.com/mosra/corrade && cd corrade && ln -s package/debian . && dpkg-buildpackage && dpkg -i ../corrade*.deb ; cp /corrade/obj-x86_64-linux-gnu/Release/bin/corrade-rc /bin ; exit 0

RUN git clone https://github.com/emscripten-core/emsdk.git && cd emsdk && ./emsdk install 2.0.10 && ./emsdk activate 2.0.10

RUN pip3 install conan

RUN conan profile new clang

RUN echo -e "[settings]\nos=Linux\narch=x86_64\ncompiler=clang\ncompiler.version=12\ncompiler.libcxx=libc++\n[env]\nCC=/usr/local/bin/clang\nCXX=/usr/local/bin/clang++" > /root/.conan/profiles/clang

RUN conan profile new emscripten

RUN echo -e "[settings]\nos=Emscripten\narch=wasm\ncompiler=clang\ncompiler.version=12\ncompiler.libcxx=libc++\n[options]\n[build_requires]\nemsdk_installer/2.0.10@werto87/stable\n[env]" > /root/.conan/profiles/emscripten

RUN conan remote add gitlab https://gitlab.com/api/v4/projects/27217743/packages/conan

# Build the client 
RUN source /emsdk/emsdk_env.sh && cd /client && rm -rf build/ ; mkdir build && cd build && conan install .. --profile:build=clang --profile:host=emscripten --build missing -s build_type=Release && emcmake cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build .

FROM busybox:glibc

COPY --from=0 /client/build/bin /client