# Coffre v4 : dechiffrement statique du flag.
# Les octets sont ecrits sur la pile par trois movabs (gcc 13, -O1) :
#   mov QWORD PTR [rsp+0x30], 0xf289f56695828c1f
#   mov QWORD PTR [rsp+0x38], 0x710809879faf77c8
#   mov QWORD PTR [rsp+0x3f], 0x8a11ce4cbaf19f71   <- attention : 0x3f, chevauchement d'un octet
buf = bytearray(23)
buf[0:8] = (0xf289f56695828c1f).to_bytes(8, 'little')
buf[8:16] = (0x710809879faf77c8).to_bytes(8, 'little')
buf[15:23] = (0x8a11ce4cbaf19f71).to_bytes(8, 'little')

k = 0x59
out = bytearray()
for b in buf:
    out.append(b ^ k)
    k = (k * 5 + 3) & 0xff      # lea edx, [rdx+rdx*4+0x3] sur un octet
print(out.decode())
