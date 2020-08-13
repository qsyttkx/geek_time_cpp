# Ubuntu 20.04 安装

依赖

```bash
sudo apt install git build-essential cmake libboost-all-dev libicu-dev libtbb-dev libgmp-dev libcpprest-dev
```

修改了部分CMakeLists.txt里的`find_package(Armadillo CONFIG)`里的`CONFIG`为`REQUIRED`