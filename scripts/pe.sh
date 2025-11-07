sudo apt update
sudo apt install build-essentials -y
sudo apt install cmake -y
sudo apt install clang -y
sudo apt install libpcap-dev -y
sudo apt install libboost-all-dev -y
sudo apt install gdb -y
sudo apt install zip -y
sudo apt install unzip -y
sudo apt install openjdk-8-jdk -y
sudo apt install libabsl-dev -y
sudo apt install libsqlite3-dev -y
echo "export PS1='\u@\h:\W\$ '" >> .bashrc

# 安装 capnproto https://capnproto.org/install.html
curl -O https://capnproto.org/capnproto-c++-1.2.0.tar.gz
tar zxf capnproto-c++-1.2.0.tar.gz
cd capnproto-c++-1.2.0
./configure
make -j6 check
sudo make install
cd ..