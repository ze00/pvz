#!/system/bin/sh
version=$(getprop ro.build.version.release)
if [ "${version%.*}" -gt "6" ];then
  wget 'https://raw.githubusercontent.com/ze00/pvz/master/release/cheater_nought' -O cheater
else
  wget 'https://raw.githubusercontent.com/ze00/pvz/master/release/cheater_marshmallow' -O cheater
fi
chmod a+x cheater
