#cd ~
#mkdir /mnt/sd_fat
mount /dev/mmcblk0p1 /mnt/sd_fat/
cp *.bit /mnt/sd_fat/
reboot
