#!/usr/bin/env python 
import string
import collections
import sets

# XORs two string
def strxor(a, b):     # xor two strings (trims the longer input)
    return "".join([chr(ord(x) ^ ord(y)) for (x, y) in zip(a, b)])

# 10 unknown ciphertexts (in hex format), all encrpyted with the same key
c1 = "3faa17b97d209cdcf1581625f69019a9d07663dcbc6b2fdb151706aeca7292fae77247743e9ff17ce6b67de06d6f35eac784c1f5470756d30c6a"
c2 = "22b31ef676289cdaeb171e77ea985ab2cd683b85e8686ec450131fadc82195bfb36e087a218ee139b9b659e8686f60dfc3dc84ef0507058d457e2beb5de672e79602fedcf49c3aa024e089a1ff5c4f6fdaf322bb84782df51660f9edaaf98ce63f51ce772ead63d4ce7ab7d954dc403319a9695440fa7a89ff6682213dc76da900ae4d71d823864c96704385b528717933782ef235"
c3 = "22bb58e06b30ddddf21d1e33b99056b5da212092ef7821d053050ae2d23a91f1e76e463801aeb52faef54afb68776c818689cbf34b500d8f507e28fc18ef33eb9b47ff9cb0ae33b73fb288adb25a4e2a82bc7ba0832169e40d60abe8aeb49da96b5bc23921ec74d7ca6ab99d10987c7d0ee17f4956b119c8b334862a"
c4 = "22b358f8243798c2e30c1921fc9140e7cc69208ebb3b3adf58054fb5c37586fae77549732d94b53debe546fa7566788dc485cdea1f07108c1c2c2fea51f426a89447e8c6e28c38a222fa95adbd4c1c21dbff6eaa97732df61b64a9f1a5e7c9a7255d877428e9729cc67ab7cb48d440711fe87c5757b12ecbf232822e22ca28b312f209398a47c307d01d6896a4306d3a"
c5 = "22bb58ea6b2898c1ec1d5024ed9858abcc213693ba696ec654131cb5c92094b3e778476d6899f432ebf557e86f64708dcf848aa6295210c355386aea57ea37e79e47bbc1e49c3aba38b582e2aa471c3bc6e96fad867364ef0a29f9e7a4e1c9a52a57806d69ea72c88f6fb7d358cf0e6005fc73591cb10eccb7668b2e38d238b304fc447bce68d541d74f47d7a0387020417d29e13e355a692233864e4eff550badb6f214d1db95d0af416d3e"
c6 = "22bb58ea612688dceb0c0977ee984ba29f602390ef6f26d7414002a3d22695eda26504382b95f82cbee25afb722362c2d39cc0a6054212864e7e28fc18f327fa9e47ff92ff9777f627f08fadbe595321cbbc6aa0996a68e55e6cb7eaa4b488e6255cd36e26ff7c9cd867e3d51dd4476008fb7f575ee87ac9bb2a81263ed03ee10eba0964c579c30fc254439bf6346c2d136c24e22a23062c6167ec0f0dff5118aab8f246"
c7 = "22bb58ea6b2898c1ec1d5032f58e5ce7dc6021dcbd6e209654120dabd22091edbe214b772c9fb533a5b646e6747135cec99dd4f31f4216cf1c373ebe4ba73ce78402c2fdc5ab7bb524f88bf8ab504e6fcff27bef9b6e7fe45025f4be99fd8aae6b72d27528fa7ed9cc"
c8 = "3fb51db96b2b91c7ec1d5023eb8855be9f722a9fba692b9646191cb6c33fd0f6b42147762ddae134aae21fe0722365c2d195d6e30f070b855a726afa59f426a8994cbbd3b09b37b928fedbe2b9155f20c0ff70aa82642de01061f9edaef585a32f19ce7769ec37d0ca6ff39051d1407109a96c545dfc7ad3bb32856f30cc20a405fc4e61cb7fc212981d0fd791386c3c414a30e63e36477e28"
c9 = "28b215e9652b94cbf1580327fc935de7d2682390a67420c5150f09e2c23d9cf3a6735b382794b53aa2e45afe606f79de8ad0c1e808551d93483725f718e63cecd051fed1e58b3ef62af698e8ac461c2bcbea6bac937221a11f6bbdbea2e0ceb56b54c8772cf437cbce7de3d859830e7a02e77b1b5df77ad0ba239e2a71d328a012a95b71d92dc705d24f4784a57d7631043937e2393b4d7f3867c4070db43710b6f5e35ce7988edba95d7a25b5298588961f063ceeabe001a24dce7aa5e1ab26780c04c59c"
c10 = "22bb58e06b30dddaea111e3cb9895ca4d76f2090a07c3796560101e2d53d9ce9a22151773d88b52faef54afb68776c8dd682cbe407420990107e3ef15de972f19f57bbd6ff977ca26be095e9ba474f3bcff266ef826968a10e77b6fca7f184b56b58c97d69f478c98f6af8d31acc0e6103ed7b4941e53bcab6669927349e39a402b4477bc662c118981d0fd7942f773a043913e4303e4d652935"
c11 = "22a958f86d2bdadaa20f1836eddd40a8ca212b93a13c3a965e0e00b5862698feb3214f7d3c89b525a4e31fe06f777a8dd282cbf3094b01cd1c173ebe4ba725e09156bbcbff8c7bbd25fa8cadb95a4e6fdde970aad67565e00a25b3ebb8e0c9a72257806d69fe78928f2eba9d70d95c7f4ddd695a5bff"
c12 = "3fb51db9743792ccee1d1d77f69b19b1d6733a8faa686edf46401ba7cb229feda67351382994f17cbcff53e52161708dd59fc8f00e43448a527e3eee57a72bed9150e89cb0d47b9c24fd95ad92567d29cbf92eefc73835b9"
c13 = "22bb58e06b30dddce70e1536f5dd40a8ca736f8faa783cd341134fb6c97284f7a2215f71269eb97cb2f94aa9726b7ad8ca9484e804534481503f27fc18f33aedd055f2dcf4d93db939b589e8a9505d23c7f265ef826968ec5e71b6bebffc8ce63f4bc27c3aa337918f45f6d551d142342ae07c4953ff"
c14 = "4bf058b4"
c15 = "4b9e17f763379cdaf7141123f09257b49f6e21dcad692bd75e0901a5862698fae762477c2dd4"
c16 = "4b8417ec76659cddf1111739f49857b39f683cdca6756ede41141fb19c7ddffcb42f497c2d96f435aff311ec65763bccd3dfdaff1d4608cc6f0e7ba117c661e1834afec0f5d72bbe3b"

ciphers = [c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16]
# The target ciphertext we wan3 to crack
# target_cipher = "32510ba9babebbbefd001547a810e67149caee11d945cd7fc81a05e9f85aac650e9052ba6a8cd8257bf14d13e6f0a803b54fde9e77472dbff89d71b57bddef121336cb85ccb8f3315f4b52e301d16e9f52f904"
target_cipher = c16
# To store the final key
final_key = [None]*150
# To store the positions we know are broken
known_key_positions = set()

# For each ciphertext
for current_index, ciphertext in enumerate(ciphers):

	counter = collections.Counter()
	# for each other ciphertext
	for index, ciphertext2 in enumerate(ciphers):
		if current_index != index: # don't xor a ciphertext with itself
			for indexOfChar, char in enumerate(strxor(ciphertext.decode('hex'), ciphertext2.decode('hex'))): # Xor the two ciphertexts
				# If a character in the xored result is a alphanumeric character, it means there was probably a space character in one of the plaintexts (we don't know which one)
				if char in string.printable and char.isalpha(): counter[indexOfChar] += 1 # Increment the counter at this index
	knownSpaceIndexes = []

	# Loop through all positions where a space character was possible in the current_index cipher
	for ind, val in counter.items():
		# If a space was found at least 7 times at this index out of the 9 possible XORS, then the space character was likely from the current_index cipher!
		if val >= 7: knownSpaceIndexes.append(ind)
	#print knownSpaceIndexes # Shows all the positions where we now know the key!

	# Now Xor the current_index with spaces, and at the knownSpaceIndexes positions we get the key back!
	xor_with_spaces = strxor(ciphertext.decode('hex'),' '*150)
	for index in knownSpaceIndexes:
		# Store the key's value at the correct position
		final_key[index] = xor_with_spaces[index].encode('hex')
		# Record that we known the key at this position
		known_key_positions.add(index)

# Construct a hex key from the currently known key, adding in '00' hex chars where we do not know (to make a complete hex string)
final_key_hex = ''.join([val if val is not None else '00' for val in final_key])
# Xor the currently known key with the target cipher
output = strxor(target_cipher.decode('hex'),final_key_hex.decode('hex'))
# Print the output, printing a * if that character is not known yet
print ''.join([char if index in known_key_positions else '*' for index, char in enumerate(output)])

'''
Manual step
'''
# From the output this prints, we can manually complete the target plaintext from:
# The secuet-mes*age*is: Wh** usi|g **str*am cipher, nev***use th* k*y *ore than onc*
# to:
# The secret message is: When using a stream cipher, never use the key more than once

# We then confirm this is correct by producing the key from this, and decrpyting all the other messages to ensure they make grammatical sense
target_plaintext = "The secret message is: When using a stream cipher, never use the key more than once"
print target_plaintext
key = strxor(target_cipher.decode('hex'),target_plaintext)
for cipher in ciphers:
	print strxor(cipher.decode('hex'),key)