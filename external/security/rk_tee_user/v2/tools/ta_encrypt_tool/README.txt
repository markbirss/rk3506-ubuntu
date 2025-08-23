
安装依赖：
pip3 install pycryptodomex

对明文TA进行加密的命令：
python3 ta_encrypt_tool.py--key oem_privkey.pem --in 1db57234-dacd-462d-9bb1-ae79de44e2a5.ta --enc_key 'b64d239b1f3c7d3b06506229cd8ff7c8af2bb4db2168621ac62c84948468c4f4'

对密文TA重新加密的命令：
python3 ta_encrypt_tool.py --key oem_privkey.pem --in 1db57234-dacd-462d-9bb1-ae79de44e2a5.ta --ori_enc_key 'b64d239b1f3c7d3b06506229cd8ff7c8af2bb4db2168621ac62c84948468c4f4' --enc_key 'c74d239b1f3c7d3b06506229cd8ff7c8af2bb4db2168621ac62c84948468c4f4'

工具参数说明：
--key 指向用户私有的用于签名TA的非对称密钥文件。
--in 指向TA文件。
--enc_key 用户私有的用于加密TA的对称密钥。
--ori_enc_key 原始加密TA的对称密钥。