#!/system/bin/sh
if [ -d "/system/lib64" ];then
  arch="aarch64"
else
  arch="arm"
fi
curl "https://raw.githubusercontent.com/ze00/pvz/master/release/${arch}/cheater" > cheater
chmod a+x cheater
