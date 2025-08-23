当用户使用公钥替换工具后，由于tee镜像中的公钥被替换，可能导致rk的ta无法正常运行，
若rk的ta没有重新签名依然能正常运行，则不需要用户重新签名
若rk的ta无法正常运行，则需要用户使用自己的密钥重新对ta进行签名

安装依赖：
pip3 install pycryptodomex

linux对明文ta重新签名的命令：
python3 resign_ta.py --key oem_privkey.pem --in 8cccf200-2450-11e4-abe20002a5d5c52c.ta
python3 resign_ta.py --key oem_privkey.pem --in 8cccf200-2450-11e4-abe2-0002a5d5c52c.ta

linux对密文ta重新签名的命令，其中enc_key是编译时加密TA的密钥：
python3 resign_ta.py --key oem_privkey.pem --in 8cccf200-2450-11e4-abe2-0002a5d5c52c.ta --enc_key 'A723A23233333434353536363737383839396161626263636464656566666666'
