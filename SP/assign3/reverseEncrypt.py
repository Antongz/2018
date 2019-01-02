def strxor(a, b):     # xor two strings (trims the longer input)
     return "".join([chr(ord(x) ^ ord(y)) for (x, y) in zip(a, b)])

def random(size=16):
    return open("/dev/urandom").read(size)

def reverseEncrypt(key, msg):
    c = strxor(key, msg)
    print c.encode('hex')
    return c

def main():
    key = random(1024)
    ciphertexts = [encrypt(key, msg) for msg in MSGS]
    return ciphertexts
# def main():
# 	fileName = "temp"
# 	for key in range(1, 1024):
# 	    #key = random(1024)
# 	    tempF = fileName + key
# 	    f= open(tempF, "w+")
	    
# 	    ciphertexts = [encrypt(key, msg) for msg in "ciphertext.txt" #MSGS]
# 	    f.write(ciphertetxts)
# 	    f.close()

#    return ciphertexts
