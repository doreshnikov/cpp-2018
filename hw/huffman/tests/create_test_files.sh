#!/usr/bin/env bash
echo "here is something" >> eq1.txt
echo "here is something" >> eq2.txt
echo "here is smth else" >> eq-.txt

dd if=/dev/urandom of=test10MbUniform.test bs=1K count=10240
dd if=/dev/urandom of=test80MbUniform.test bs=1M count=80

wget -O test100MbActual.test "speedtest.ftp.otenet.gr/files/test100Mb.db"