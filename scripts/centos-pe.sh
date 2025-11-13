sudo yum groupinstall "Development Tools" -y
sudo yum install cmake -y
sudo yum install boost-devel -y
sudo yum install java-1.8.0-openjdk-devel.x86_64 -y
sudo yum install sqlite-devel -y

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
sudo make -j6
sudo make install
cd ..