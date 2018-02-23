#!/system/bin/sh
version=$(/system/bin/getprop ro.build.version.release)

if [ "${version%%.*}" -gt "6" ];then
  if [ -d "/system/lib64" ];then
    curl 'https://raw.githubusercontent.com/ze00/pvz/master/release/arm64/cheater_nought' > cheater
  else
    curl 'https://raw.githubusercontent.com/ze00/pvz/master/release/arm/cheater_nought' > cheater
  fi
else
  if [ -d "/system/lib64" ];then
    curl 'https://raw.githubusercontent.com/ze00/pvz/master/release/arm64/cheater_marshmallow' > cheater
  else
    curl 'https://raw.githubusercontent.com/ze00/pvz/master/release/arm/cheater_marshmallow' > cheater
  fi
fi
chmod a+x cheater
