#!/system/bin/sh
version=$(getprop ro.build.version.release)

if [ "${version%%.*}" -gt "6" ];then
  if [ -d "/system/lib64" ];then
    wget 'https://raw.githubusercontent.com/ze00/pvz/master/release/arm64/cheater_nought' -O cheater
  else
    wget 'https://raw.githubusercontent.com/ze00/pvz/master/release/arm/cheater_nought' -O cheater
  fi
else
  if [ -d "/system/lib64" ];then
    wget 'https://raw.githubusercontent.com/ze00/pvz/master/release/arm64/cheater_marshmallow' -O cheater
  else
    wget 'https://raw.githubusercontent.com/ze00/pvz/master/release/arm/cheater_marshmallow' -O cheater
  fi
fi
chmod a+x cheater
