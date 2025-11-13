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
git clone -b lts_2025_08_14 --single-branch https://gitee.com/mirrors/Abseil.git
cd Abseil
mkdir build
cd build
cmake .. -DCMAKE_CXX_STANDARD=17 -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCMAKE_POSITION_INDEPENDENT_CODE=ON
make -j6
make install