
用户需使用如下命令替换TEE固件中的Soft TA encryption key密钥：
./change_ta_enc_key --teebin <TEE binary> --takey 'b64d239b1f3c7d3b06506229cd8ff7c8af2bb4db2168621ac62c84948468c4f4'

工具参数说明：
--teebin为 TEE binary 固件路径。
--takey为开发者私有的32字节密钥，需与加密TA时使用的密钥保持一致。
