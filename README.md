# PVZ小工具
  * 准备工作

```
  make
  unzip com.popcap.pvz_na.apk lib/armeabi/libpvz.so
```
  * 打包(##签名工作请自行完成##)
```
  zip -ur com.popcap.pvz_na.apk lib
```

  * 将植物的冷却时间以及单价都置0
```
  gcc -o pvz pvz.c
  ./pvz
```
  * 获取基址
```
  gcc -o elfreader elfreader.c
  ./elfreader libpvz.so
```
  * 使用交互式的游戏修改功能
```
  gcc -o cheater cheater.c
  su
  ./cheater
```
