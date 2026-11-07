# Glossaire

Définitions courtes des termes employés dans le cours. Le numéro de chapitre renvoie à l'endroit où la notion est vue en détail.

## Généralités

- **CTF** (*Capture The Flag*) : compétition de sécurité où l'on résout des défis pour obtenir des **flags**. (Accueil)
- **Flag** : une chaîne secrète à trouver, preuve qu'un défi est résolu (souvent de la forme `FLAG{...}`). (Accueil)
- **Crackme** : petit programme conçu comme exercice de reverse, dont le but est de trouver un mot de passe ou un numéro de série valide. (Chapitre 1)
- **Reverse engineering** : comprendre le fonctionnement d'un programme à partir de son code compilé, sans le code source. (Accueil)

## Représentation des données

- **Poids faible / poids fort** : l'octet (ou le bit) de poids faible est celui qui pèse le moins dans un nombre ; celui de poids fort, le plus. (Chapitre 1)
- **Little endian** (petit-boutisme) : convention de rangement où l'octet de poids faible est stocké en premier en mémoire. (Chapitre 1)
- **Extension de signe** : compléter un nombre signé sur plus d'octets en recopiant son bit de signe (instruction `cdq`, `movsx`). (Chapitres 2 et 6)

## Assembleur et exécution

- **Registre** : petite zone mémoire interne au processeur (`rax`, `rsp`...). (Chapitre 2)
- **Offset** (déplacement) : écart constant par rapport à une adresse de base, par exemple `[rbp-0x8]`. (Chapitre 6)
- **Registres vectoriels** : registres `xmm0` à `xmm15`, de 128 bits, réservés aux nombres à virgule et aux traitements en parallèle. (Chapitre 16)
- **Opcode** : le ou les octets qui codent une instruction en langage machine. (Chapitres 2 et 11)
- **Bytecode** : suite d'instructions d'une machine virtuelle (et non du processeur réel), par exemple dans un crackme à base de machine virtuelle. (Chapitre 11)
- **Appel système** (*syscall*) : demande de service au noyau du système (lire, écrire, quitter). (Chapitres 8 et 15)
- **Noyau** : le cœur du système d'exploitation, seul à pouvoir accéder directement au matériel. (Chapitre 15)
- **Entrée standard** : la source d'entrée par défaut d'un programme (clavier, ou ce qui arrive par `|` et `<`), descripteur 0. (Chapitres 10 et 15)
- **Bibliothèque partagée** : code réutilisable chargé au lancement (`.so` sous Linux, **DLL** sous Windows). (Chapitre 5)
- **libc** : la bibliothèque standard du C, qui fournit `printf`, `strcmp`, `malloc`... (Chapitre 5)
- **PLT** (*Procedure Linkage Table*) : table par laquelle passent les appels aux fonctions des bibliothèques partagées (`strcmp@plt`). (Chapitre 5)

## Mémoire et protections

- **Adresse virtuelle** : l'adresse d'une donnée ou d'une instruction une fois le programme chargé en mémoire. (Chapitres 10 et 11)
- **PIE** (*Position Independent Executable*) : exécutable qui peut être chargé à une adresse différente à chaque lancement. (Chapitres 6 et 8)
- **ASLR** (randomisation) : mécanisme qui place le programme et ses données à des adresses aléatoires à chaque exécution. (Chapitre 10)
- **NX** (*No eXecute*) : protection qui marque la pile et les données comme non exécutables, pour empêcher d'y exécuter du code injecté. (Chapitre 17)
- **Strippé** : se dit d'un binaire dont on a retiré les noms de fonctions et de variables. (Chapitre 8)
- **Canari de pile** : valeur de contrôle placée sur la pile pour détecter un débordement. (Chapitre 5)

## Analyse et malware

- **Analyse statique** : étudier un programme sans l'exécuter. **Analyse dynamique** : l'exécuter et l'observer. (Chapitres 8 à 10)
- **Empreinte / hash** : résultat d'une **fonction de hachage**, qui transforme une donnée en une valeur de taille fixe, en pratique impossible à inverser (MD5, SHA). (Chapitre 11)
- **Entropie** : mesure du désordre d'une suite d'octets ; une entropie élevée trahit des données chiffrées ou compressées (programme *packé*). (Chapitres 11 et 12)
- **Signal** / **SIGTRAP** : message envoyé à un processus par le système ; `SIGTRAP` est lié au débogage. (Chapitre 12)
- **IOC** (*Indicator of Compromise*) : élément observable trahissant une compromission (empreinte, domaine, IP, clé de registre, mutex). (Chapitre 13)
- **C2** (*Command and Control*) : serveur distant qui pilote un malware et reçoit les données volées. (Chapitre 13)
- **MITRE ATT&CK** : base de connaissances répertoriant les techniques d'attaque, utilisée pour classer un comportement observé. (Chapitre 13)

## Notions Windows

Voir la section "Notions Windows en bref" au chapitre 13 : **processus**, **thread**, **DLL**, **API Windows / Win32**, **registre**, **service**, **mutex**, **handle**.
