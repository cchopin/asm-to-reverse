/* Coffre v5 : un faux malware pour s'entrainer (chapitre 13).
 *
 * CE PROGRAMME EST INOFFENSIF. Il imite le comportement d'un petit
 * implant (chaines chiffrees, verification anti-sandbox, verrou
 * anti-double execution, reconnaissance, preparation d'une balise,
 * persistance) mais :
 *   - il ne fait AUCUNE connexion reseau ;
 *   - il n'ecrit que dans /tmp, des fichiers qui commencent par .coffre_v5 ;
 *   - il ne modifie aucune configuration de demarrage.
 * Nettoyage apres analyse : rm -f /tmp/.coffre_v5*
 *
 * Compilation : gcc -O1 -o coffre_v5 coffre_v5.c && strip coffre_v5
 * Objectif : produire un rapport d'analyse avec les indicateurs (IOC).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>

static unsigned char S_C2[] = {
    0x5f, 0x43, 0x43, 0x47, 0x0d, 0x18, 0x18, 0x54, 0x05, 0x19, 0x54, 0x58,
    0x51, 0x51, 0x45, 0x52, 0x19, 0x5e, 0x59, 0x41, 0x56, 0x5b, 0x5e, 0x53,
    0x0d, 0x0f, 0x03, 0x03, 0x04, 0x18, 0x50, 0x56, 0x43, 0x52, 0x19, 0x47,
    0x5f, 0x47, 0x37
};  /* {len(s)} car. */
static unsigned char S_UA[] = {
    0x7a, 0x58, 0x4d, 0x5e, 0x5b, 0x5b, 0x56, 0x18, 0x02, 0x19, 0x07, 0x17,
    0x1f, 0x74, 0x58, 0x51, 0x51, 0x45, 0x52, 0x75, 0x58, 0x43, 0x18, 0x06,
    0x19, 0x07, 0x1e, 0x37
};  /* {len(s)} car. */
static unsigned char S_LOCK[] = {
    0x18, 0x43, 0x5a, 0x47, 0x18, 0x19, 0x54, 0x58, 0x51, 0x51, 0x45, 0x52,
    0x68, 0x41, 0x02, 0x19, 0x5b, 0x58, 0x54, 0x5c, 0x37
};  /* {len(s)} car. */
static unsigned char S_CACHE[] = {
    0x18, 0x43, 0x5a, 0x47, 0x18, 0x19, 0x54, 0x58, 0x51, 0x51, 0x45, 0x52,
    0x68, 0x41, 0x02, 0x19, 0x54, 0x56, 0x54, 0x5f, 0x52, 0x37
};  /* {len(s)} car. */
static unsigned char S_PERSIST[] = {
    0x18, 0x43, 0x5a, 0x47, 0x18, 0x19, 0x54, 0x58, 0x51, 0x51, 0x45, 0x52,
    0x68, 0x41, 0x02, 0x68, 0x56, 0x42, 0x43, 0x58, 0x44, 0x43, 0x56, 0x45,
    0x43, 0x19, 0x53, 0x52, 0x44, 0x5c, 0x43, 0x58, 0x47, 0x37
};  /* {len(s)} car. */
static unsigned char S_CAMP[] = {
    0x74, 0x76, 0x7a, 0x67, 0x76, 0x70, 0x79, 0x72, 0x1a, 0x65, 0x78, 0x62,
    0x70, 0x72, 0x1a, 0x07, 0x00, 0x37
};  /* {len(s)} car. */
static unsigned char S_SANDBOX[] = {
    0x44, 0x56, 0x59, 0x53, 0x55, 0x58, 0x4f, 0x37
};  /* {len(s)} car. */

/* Dechiffre une chaine sur place (XOR avec un octet fixe). */
static char *dec(unsigned char *s)
{
    for (size_t i = 0; ; i++) {
        s[i] ^= 0x37;
        if (s[i] == 0)
            break;
    }
    return (char *)s;
}

static int dans_une_sandbox(void)
{
    char hote[256] = {0};
    gethostname(hote, sizeof(hote) - 1);
    if (strstr(hote, dec(S_SANDBOX)) != NULL)
        return 1;
    if (sysconf(_SC_NPROCESSORS_ONLN) < 2)
        return 1;
    return 0;
}

static int prendre_verrou(const char *chemin)
{
    int fd = open(chemin, O_CREAT | O_EXCL | O_WRONLY, 0600);
    if (fd < 0)
        return 0;          /* deja en cours d'execution */
    dprintf(fd, "%d\n", (int)getpid());
    close(fd);
    return 1;
}

static void encoder_hex(const char *in, char *out, unsigned char cle)
{
    static const char hx[] = "0123456789abcdef";
    size_t j = 0;
    for (size_t i = 0; in[i] != 0; i++) {
        unsigned char c = (unsigned char)in[i] ^ cle;
        out[j++] = hx[c >> 4];
        out[j++] = hx[c & 0xf];
        cle = (unsigned char)(cle + 1);
    }
    out[j] = 0;
}

int main(void)
{
    sleep(3);                         /* attendre : certaines sandbox abandonnent */
    if (dans_une_sandbox())
        return 0;

    if (!prendre_verrou(dec(S_LOCK)))
        return 0;

    char hote[256] = {0};
    gethostname(hote, sizeof(hote) - 1);
    struct passwd *pw = getpwuid(getuid());
    const char *user = pw ? pw->pw_name : "inconnu";

    char balise[1024];
    snprintf(balise, sizeof(balise), "id=%s&host=%s&user=%s&ua=%s&dst=%s",
             dec(S_CAMP), hote, user, dec(S_UA), dec(S_C2));

    char encode[2 * sizeof(balise) + 1];
    encoder_hex(balise, encode, 0x42);

    FILE *f = fopen(dec(S_CACHE), "w");
    if (f) {
        fprintf(f, "%s\n", encode);
        fclose(f);
    }

    f = fopen(dec(S_PERSIST), "w");
    if (f) {
        fprintf(f, "[Desktop Entry]\nType=Application\nName=Mise a jour\n"
                   "Exec=/tmp/coffre_v5\nHidden=true\n");
        fclose(f);
    }
    return 0;
}
