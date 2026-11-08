# Glossaire

Définitions courtes des termes employés dans le cours. Le numéro de chapitre renvoie à l'endroit où la notion est vue en détail.

## Généralités

- **CTF** (*Capture The Flag*) : compétition de sécurité où l'on résout des défis pour obtenir des **flags**. (Accueil)
- **Flag** : une chaîne secrète à trouver, preuve qu'un défi est résolu (souvent de la forme `FLAG{...}`). (Accueil)
- **Crackme** : petit programme conçu comme exercice de reverse, dont le but est de trouver un mot de passe ou un numéro de série valide. (partie 2, chapitre 1)
- **Reverse engineering** : comprendre le fonctionnement d'un programme à partir de son code compilé, sans le code source. (Accueil)

## Représentation des données

- **Poids faible / poids fort** : l'octet (ou le bit) de poids faible est celui qui pèse le moins dans un nombre ; celui de poids fort, le plus. (partie 2, chapitre 1)
- **Little endian** (petit-boutisme) : convention de rangement où l'octet de poids faible est stocké en premier en mémoire. (partie 2, chapitre 1)
- **Extension de signe** : compléter un nombre signé sur plus d'octets en recopiant son bit de signe (instruction `cdq`, `movsx`). (partie 2, chapitres 2 et 6)

## Assembleur et exécution

- **Registre** : petite zone mémoire interne au processeur (`rax`, `rsp`...). (partie 2, chapitre 2)
- **Offset** (déplacement) : écart constant par rapport à une adresse de base, par exemple `[rbp-0x8]`. (partie 2, chapitre 6)
- **Registres vectoriels** : registres `xmm0` à `xmm15`, de 128 bits, réservés aux nombres à virgule et aux traitements en parallèle. (partie 2, chapitres 9 et 11)
- **Opcode** : le ou les octets qui codent une instruction en langage machine. (partie 2, chapitre 2 ; partie 3, chapitre 4)
- **Bytecode** : suite d'instructions d'une machine virtuelle (et non du processeur réel), par exemple dans un crackme à base de machine virtuelle. (partie 3, chapitre 4)
- **Appel système** (*syscall*) : demande de service au noyau du système (lire, écrire, quitter). (partie 3, chapitre 1 ; partie 4, chapitre 1)
- **Noyau** : le cœur du système d'exploitation, seul à pouvoir accéder directement au matériel. (partie 4, chapitre 1)
- **Entrée standard** : la source d'entrée par défaut d'un programme (clavier, ou ce qui arrive par `|` et `<`), descripteur 0. (partie 3, chapitre 3 ; partie 4, chapitre 1)
- **Bibliothèque partagée** : code réutilisable chargé au lancement (`.so` sous Linux, **DLL** sous Windows). (partie 2, chapitre 5)
- **libc** : la bibliothèque standard du C, qui fournit `printf`, `strcmp`, `malloc`... (partie 2, chapitre 5)
- **PLT** (*Procedure Linkage Table*) : table par laquelle passent les appels aux fonctions des bibliothèques partagées (`strcmp@plt`). (partie 2, chapitre 5)

## Mémoire et protections

- **Adresse virtuelle** : l'adresse d'une donnée ou d'une instruction une fois le programme chargé en mémoire. (partie 3, chapitres 3 et 4)
- **PIE** (*Position Independent Executable*) : exécutable qui peut être chargé à une adresse différente à chaque lancement. (partie 2, chapitre 6 ; partie 3, chapitre 1)
- **ASLR** (randomisation) : mécanisme qui place le programme et ses données à des adresses aléatoires à chaque exécution. (partie 3, chapitre 3)
- **NX** (*No eXecute*) : protection qui marque la pile et les données comme non exécutables, pour empêcher d'y exécuter du code injecté. (partie 4, chapitre 3)
- **Strippé** : se dit d'un binaire dont on a retiré les noms de fonctions et de variables. (partie 3, chapitre 1)
- **Canari de pile** : valeur de contrôle placée sur la pile pour détecter un débordement. (partie 2, chapitre 5)

## Analyse et malware

- **Analyse statique** : étudier un programme sans l'exécuter. **Analyse dynamique** : l'exécuter et l'observer. (partie 3, chapitres 1 à 3)
- **Empreinte / hash** : résultat d'une **fonction de hachage**, qui transforme une donnée en une valeur de taille fixe, en pratique impossible à inverser (MD5, SHA). (partie 3, chapitre 4)
- **Entropie** : mesure du désordre d'une suite d'octets ; une entropie élevée trahit des données chiffrées ou compressées (programme *packé*). (partie 3, chapitres 4 et 5)
- **Signal** / **SIGTRAP** : message envoyé à un processus par le système ; `SIGTRAP` est lié au débogage. (partie 3, chapitre 5)
- **IOC** (*Indicator of Compromise*) : élément observable trahissant une compromission (empreinte, domaine, IP, clé de registre, mutex). (partie 3, chapitre 6)
- **C2** (*Command and Control*) : serveur distant qui pilote un malware et reçoit les données volées. (partie 3, chapitre 6)
- **MITRE ATT&CK** : base de connaissances répertoriant les techniques d'attaque, utilisée pour classer un comportement observé. (partie 3, chapitre 6)

## Notions Windows

Voir la section "Notions Windows en bref" au chapitre 6 de la partie 3 : **processus**, **thread**, **DLL**, **API Windows / Win32**, **registre**, **service**, **mutex**, **handle**.
