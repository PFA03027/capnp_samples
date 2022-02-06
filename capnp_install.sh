mkdir -p capnp_install
cd capnp_install

curl -O https://capnproto.org/capnproto-c++-0.9.1.tar.gz
tar zxf capnproto-c++-0.9.1.tar.gz
cd capnproto-c++-0.9.1
./configure
make -j6 check
sudo make install
