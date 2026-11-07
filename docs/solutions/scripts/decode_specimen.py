#!/usr/bin/env python3
# Analyse STATIQUE du specimen (capstone) : rejoue les XOR sans executer.
# Les trois tableaux (charge, s_dom, s_camp) et les deux cles se relevent
# dans le binaire avec objdump -s -j .rodata / .data.
import subprocess, sys
# charge chiffree (XOR 0x6B) relevee dans le binaire :
CLE_CHARGE = 0x6B
CLE_STR = 0x3C
# Ici on lit directement les octets du binaire fourni en argument via objdump
# n'est pas rejoue : par simplicite, coller les tableaux releves.
dom_enc  = [0x49,0x4c,0x58,0x5d,0x48,0x59,0x12,0x4f,0x45,0x52,0x5f,0x12,
            0x59,0x44,0x5d,0x51,0x4c,0x50,0x59,0x12,0x55,0x52,0x4a,0x5d,
            0x50,0x55,0x58,0x3c]
camp_enc = [0x7f,0x7d,0x71,0x6c,0x11,0x0e,0x0c,0x0e,0x0a,0x11,0x73,0x6e,
            0x7d,0x72,0x7b,0x79,0x3c]
dom  = bytes(b ^ CLE_STR for b in dom_enc).rstrip(b"\x00").decode()
camp = bytes(b ^ CLE_STR for b in camp_enc).rstrip(b"\x00").decode()
print("C2       :", dom)
print("Campagne :", camp)
print("Cle charge: 0x%02X" % CLE_CHARGE)
