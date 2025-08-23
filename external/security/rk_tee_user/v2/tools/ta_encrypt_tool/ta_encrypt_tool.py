#!/usr/bin/env python

def uuid_parse(s):
	from uuid import UUID
	return UUID(s)


def int_parse(str):
	return int(str, 0)

def get_args():
	from argparse import ArgumentParser

	parser = ArgumentParser()
	parser.add_argument('--key', required=True, help='Name of key file')
	parser.add_argument('--in', required=True, dest='inf', help='Name of in file')
	parser.add_argument('--enc_key', required=True, help='Encryption key string')
	parser.add_argument('--ori_enc_key', required=False, help='Original encryption key string')
	return parser.parse_args()

def main():
	try:
		from Cryptodome.Signature import pss
		from Cryptodome.Signature import pkcs1_15
		from Cryptodome.Hash import SHA256
		from Cryptodome.PublicKey import RSA
	except ImportError:
		from Crypto.Signature import pss
		from Crypto.Signature import pkcs1_15
		from Crypto.Hash import SHA256
		from Crypto.PublicKey import RSA

	import struct
	import sys

	args = get_args()

	f = open(args.key, 'rb')
	key = RSA.importKey(f.read())
	f.close()

	f = open(args.inf, 'rb')
	img = f.read()
	temp = img[0:4]
	ta_magic = temp.find(b'\x48\x53\x54\x4F')
	if ta_magic == -1:
		print("not find ta magic! input file is not a TA!")
		sys.exit(0)
	temp = img[4:8]
	ta_img_type0 = temp.find(b'\x00\x00\x00\x00')
	ta_img_type1 = temp.find(b'\x01\x00\x00\x00')
	ta_img_type2 = temp.find(b'\x02\x00\x00\x00')
	f.close()

	if ta_img_type0 != -1:
		print("Unsupported TA type! ta_img_type is 0")
		sys.exit(0)
	if ta_img_type1 != -1:
		temp = img[12:16]
		ta_algo_pkcs1_v1_5 = temp.find(b'\x30\x48\x00\x70')
		ta_algo_pkcs1_pss = temp.find(b'\x30\x49\x41\x70')
		if ta_algo_pkcs1_v1_5 != -1:
			signer = pkcs1_15.new(key)
			algo = 0x70004830
			print("ta algo is TEE_ALG_RSASSA_PKCS1_V1_5_SHA256")
		if ta_algo_pkcs1_pss != -1:
			signer = pss.new(key)
			algo = 0x70414930
			print("ta algo is TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256")

		shdr_uuid = img[0x134:0x144]
		shdr_version = img[0x144:0x148]
		img = img[0x148:len(img)]
		h = SHA256.new()

		digest_len = h.digest_size
		sig_len = key.size_in_bytes()
		img_size = len(img)

		magic = 0x4f545348    # SHDR_MAGIC
		img_type = 2        # SHDR_ENCRYPTED_TA

		shdr = struct.pack('<IIIIHH',
		                   magic, img_type, img_size, algo, digest_len, sig_len)
		if args.enc_key:
			from Cryptodome.Cipher import AES
			cipher = AES.new(bytearray.fromhex(args.enc_key), AES.MODE_GCM)
			ciphertext, tag = cipher.encrypt_and_digest(img)
			enc_algo = 0x40000810  # TEE_ALG_AES_GCM
			flags = 0              # SHDR_ENC_KEY_DEV_SPECIFIC
			ehdr = struct.pack('<IIHH',
					enc_algo, flags, len(cipher.nonce), len(tag))

		h.update(shdr)
		h.update(shdr_uuid)
		h.update(shdr_version)
		if args.enc_key:
			h.update(ehdr)
			h.update(cipher.nonce)
			h.update(tag)
		h.update(img)
		img_digest = h.digest()
		sig = signer.sign(h)

		f = open(args.inf, 'wb')
		f.write(shdr)
		f.write(img_digest)
		f.write(sig)
		f.write(shdr_uuid)
		f.write(shdr_version)
		f.write(ehdr)
		f.write(cipher.nonce)
		f.write(tag)
		f.write(ciphertext)
		f.close()
		print("Encrypt TA sucess! Original ta_img_type is 1")
		sys.exit(0)
	if ta_img_type2 != -1:
		shdr_uuid = img[0x134:0x144]
		shdr_version = img[0x144:0x148]
		ehdr = img[0x148:0x154]
		nonce = img[0x154:0x164]
		tag = img[0x164:0x174]
		ciphertext = img[0x174:len(img)]
		if args.ori_enc_key:
			from Cryptodome.Cipher import AES
			cipher = AES.new(bytearray.fromhex(args.ori_enc_key), AES.MODE_GCM, nonce)
			img = cipher.decrypt_and_verify(ciphertext, tag)
		else:
			print("error! Encryption TA need ori_enc_key!")
			sys.exit(0)

		if args.enc_key:
			from Cryptodome.Cipher import AES
			cipher = AES.new(bytearray.fromhex(args.enc_key), AES.MODE_GCM)
			ciphertext, tag = cipher.encrypt_and_digest(img)
			enc_algo = 0x40000810  # TEE_ALG_AES_GCM
			flags = 0              # SHDR_ENC_KEY_DEV_SPECIFIC
			ehdr = struct.pack('<IIHH',
					enc_algo, flags, len(cipher.nonce), len(tag))
		else:
			print("error! Encryption TA need enc_key!")
			sys.exit(0)

		h = SHA256.new()
		magic = 0x4f545348  # SHDR_MAGIC
		img_type = 2        # SHDR_ENCRYPTED_TA
		img_size = len(img)
		algo = 0x70414930		# TEE_ALG_RSASSA_PKCS1_PSS_MGF1_SHA256
		digest_len = h.digest_size
		sig_len = key.size_in_bytes()

		shdr = struct.pack('<IIIIHH',
		                   magic, img_type, img_size, algo, digest_len, sig_len)

		h.update(shdr)
		h.update(shdr_uuid)
		h.update(shdr_version)
		if args.enc_key:
			h.update(ehdr)
			h.update(cipher.nonce)
			h.update(tag)
		h.update(img)
		img_digest = h.digest()

		signer = pss.new(key)
		sig = signer.sign(h)

		f = open(args.inf, 'wb')
		f.write(shdr)
		f.write(img_digest)
		f.write(sig)
		f.write(shdr_uuid)
		f.write(shdr_version)
		f.write(ehdr)
		f.write(cipher.nonce)
		f.write(tag)
		f.write(ciphertext)
		f.close()
		print("Encrypt TA sucess! Original ta_img_type is 2")
		sys.exit(0)
	print("Encrypt TA fail! ta_img_type are assert to 0 1 2")
	sys.exit(0)


if __name__ == "__main__":
	main()
