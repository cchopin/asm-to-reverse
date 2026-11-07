rule Coffre_v5_implant
{
    meta:
        description = "Faux implant pedagogique Coffre v5 (et variantes)"
        reference = "Cours de reverse, chapitre 13"

    strings:
        // Chaine de format de la balise : en clair dans le binaire, stable d'une version a l'autre
        $fmt = "id=%s&host=%s&user=%s&ua=%s&dst=%s" ascii
        // Contenu du fichier de persistance
        $desk = "Hidden=true" ascii
        // Routine de dechiffrement : xor reg, 0x37 suivi (de pres) d'un cmp reg8, 0x37
        $dec = { 83 F? 37 [2-3] 80 F? 37 }
        // Alphabet hexadecimal de l'encodeur de balise
        $hex = "0123456789abcdef" ascii

    condition:
        uint32(0) == 0x464c457f and     // "\x7fELF"
        filesize < 200KB and
        $fmt and $dec and ($desk or $hex)
}
