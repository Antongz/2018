def modexp ( base, exponent, modular ):
    res = 1
    while exponent != 0:
        if exponent & 1:
            res = (res * base) % modular
        exponent >>= 1
        base = (base * base) % modular;
    return res

