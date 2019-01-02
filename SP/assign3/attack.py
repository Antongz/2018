#!/usr/bin/env python3
import os, binascii


def xor(a, b):
    """Return the XOR of two byte sequences. The length of the result is the length of the shortest."""
    return bytes(x ^ y for (x, y) in zip(a, b))


def encrypt_with_random_pad(inF, ciphF, encLines):
    try:
        with open(inF) as f:
            cleartexts = f.read().splitlines()
    except Exception as e:
        raise Exception("Cannot read {}, error={}".format(inF, e))
    if len(cleartexts) > encLines:
        cleartexts = cleartexts[0:encLines]
        
    print(str(len(cleartexts)) + " lines of clear text was read...")
    pad = os.urandom(max(len(m) for m in cleartexts))
    # print("pad = ", pad)
    print("Random pad was generated, its length is " + str(len(pad)) + " bytes (or " + str(len(pad)*8) + " bit)")

    ciphertexts = [binascii.hexlify(xor(pad, m.encode('ascii'))).decode('ascii') for m in cleartexts]
    print(str(len(ciphertexts)) + " lines of cipher text has now been generated, they will be written to:", ciphF)
    try:
        with open(ciphF, "w") as f:
            for c in ciphertexts:
                print(c, file=f)
    except Exception as e:
        raise Exception("Cannot write {}, error={}".format(ciphF, e))
    return pad


def countalphas( char, position, ciphertexts ):
    count = 0
    for ciphertext in ciphertexts:
        if len(ciphertext) > position:
            if chr(ciphertext[position]^char).isalpha(): count+=1
    return count


def guess_encryption(ciphF, attF, crackLines):
    try:
        with open(ciphF) as f:
            ciphertexts = [binascii.unhexlify(line.rstrip()) for line in f]
            # Cyphertexts clean (voids removed), although it is not necessary
            # ciphertexts = [c for c in ciphertexts if c]
        cleartexts = [bytearray(b'?' * len(c)) for c in ciphertexts]
    except Exception as e:
        print("Exception raised: {} --- {}".format(ciphF, e))
        raise SystemExit(-1)

    if len(ciphertexts) > crackLines:
        ciphertexts = ciphertexts[0:crackLines]
        cleartexts = cleartexts[0:crackLines]
        
    print(str(len(cleartexts)) + " lines of clear text will be used...")
    # Take maximum value of length, then iterate on the columns.
    # Make the XOR only on ciphertexts with adequate length
    for col in range(max([len(x) for x in ciphertexts])):
        for c1 in ciphertexts:
            for c2 in ciphertexts:
                if (len(c1) > col) and (len(c2) > col):
                    if chr(c1[col]^c2[col]).isalpha():
                        # Check what more letters correspond to the other ciphertexts.
                        # Assume that the one with the highest number is that with the space
                        # Space is in c1, so c2 has the character, but inverted
                        # try to get the PAD for that letter
                        for k, c in enumerate(ciphertexts):
                            if len(c) > col:
                                if countalphas(c1[col], col, ciphertexts) >= countalphas(c2[col], col, ciphertexts):
                                    cleartexts[k][col] = c1[col]^0b100000^c[col]
                                else:
                                    cleartexts[k][col] = c2[col]^0b100000^c[col]
                        break

    print(str(len(cleartexts)) + " lines of clear text has been guessed/cracked, they will be written to:", attF)
    # [print(c.decode() ) for c in cleartexts]
    try:
        with open(attF, "w") as f:
            for c in cleartexts:
                print(c.decode(), file=f)
    except Exception as e:
        raise Exception("Cannot write {}, error={}".format(ciphF, e))    
    print(" ")
    

def main():
    print("This program will encrypt plaintext (using many-time pad) - and perform")
    print("a well-known cryptographic attack on the output ciphertexts.")
    print(" ")

    inFile="plain.txt"
    ciphFile="cipher.txt"
    attFile="attack.txt"
    encLines=9999
    crackLines=9999

#    print("Encrypting {} ==> {}".format(inFile, ciphFile))
#    print(" ")
#    realPad = encrypt_with_random_pad(inFile, ciphFile, encLines)
#    print("real pad=", binascii.hexlify(realPad).decode('ascii'), sep="")
#    print(" ")
    print("Now guessing/cracking the many-time pad encryption, result => ", attFile)
    guess_encryption(ciphFile, attFile, crackLines)
    print(" --- ")

if __name__ == "__main__":
    main()
