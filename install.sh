#!/system/bin/sh
version=$(/system/bin/getprop ro.build.version.release)

if [ -d "/system/lib64" ];then
  arch="arm64"
else
  arch="arm"
fi
if [ "${version%%.*}" -gt "6" ];then
  version="nought"
else
  version="marshmallow"
fi
curl "https://raw.githubusercontent.com/ze00/pvz/master/release/${arch}/cheater_${version}" > cheater
chmod a+x cheater
