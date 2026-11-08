# Challenge de synthèse

Fin de la partie 2. Ce challenge rassemble tout : registres, arithmétique, conditions, boucles, pile et accès mémoire. Trois fonctions ne vous sont données que sous forme désassemblée. Pour chacune, il faut retrouver le code C équivalent.

C'est exactement le travail de la partie 3, mais sur du code simple et non optimisé. Prenez le temps : lisez le prologue, identifiez les arguments, suivez les sauts, nommez les variables locales.

!!! tip "Méthode"
    1. Repérer le prologue et les arguments (registres `rdi`, `esi`, `rdx`... sauvés sur la pile).
    2. Nommer chaque emplacement de pile selon son rôle (compteur, accumulateur, argument).
    3. Suivre les sauts : un saut vers l'arrière est une boucle, un saut conditionnel est un `if`.
    4. Reconstituer en C, puis **vérifier** en recompilant avec `gcc -O0 -fno-stack-protector -no-pie` et en comparant le désassemblage.

Toutes les fonctions ci-dessous sont compilées avec `gcc -O0 -fno-stack-protector -no-pie`.

## Fonction g1

```text
<g1>:
  push   %rbp
  mov    %rsp,%rbp
  mov    %edi,-0x4(%rbp)
  mov    %esi,-0x8(%rbp)
  mov    -0x4(%rbp),%edx
  mov    -0x8(%rbp),%eax
  add    %eax,%edx
  mov    -0x4(%rbp),%eax
  sub    -0x8(%rbp),%eax
  imul   %edx,%eax
  pop    %rbp
  ret
```

### Challenge 1 - g1 (N2)

- **À produire :** l'expression renvoyée, puis sa forme simplifiée

Deux arguments entiers, une seule expression renvoyée. Laquelle ? La simplifier mathématiquement.

## Fonction g2

```text
<g2>:
  push   %rbp
  mov    %rsp,%rbp
  mov    %edi,-0x14(%rbp)
  movl   $0x1,-0x4(%rbp)
  jmp    TEST
CORPS:
  mov    -0x4(%rbp),%eax
  imul   -0x14(%rbp),%eax
  mov    %eax,-0x4(%rbp)
  subl   $0x1,-0x14(%rbp)
TEST:
  cmpl   $0x1,-0x14(%rbp)
  jg     CORPS
  mov    -0x4(%rbp),%eax
  pop    %rbp
  ret
```

### Challenge 2 - g2 (N3)

- **À produire :** la fonction mathématique reconnue, le C équivalent, et la valeur renvoyée pour l'argument 5

Un argument, un accumulateur initialisé à 1, une boucle qui multiplie et décrémente.

## Fonction g3

```text
<g3>:
  push   %rbp
  mov    %rsp,%rbp
  mov    %rdi,-0x18(%rbp)
  movl   $0x0,-0x4(%rbp)
  movl   $0x0,-0x8(%rbp)
  jmp    TEST
CORPS:
  mov    -0x8(%rbp),%eax
  movslq %eax,%rdx
  mov    -0x18(%rbp),%rax
  add    %rdx,%rax
  movzbl (%rax),%eax
  cmp    $0x61,%al
  jne    SUITE
  addl   $0x1,-0x4(%rbp)
SUITE:
  addl   $0x1,-0x8(%rbp)
TEST:
  mov    -0x8(%rbp),%eax
  movslq %eax,%rdx
  mov    -0x18(%rbp),%rax
  add    %rdx,%rax
  movzbl (%rax),%eax
  test   %al,%al
  jne    CORPS
  mov    -0x4(%rbp),%eax
  pop    %rbp
  ret
```

### Challenge 3 - g3 (N3)

- **À produire :** la valeur de `0x61` en ASCII, ce que compte la fonction, et le C équivalent

L'argument est un pointeur (8 octets). Deux compteurs, une boucle sur les caractères jusqu'au `'\0'`, et une comparaison à `0x61`.

## Vérification

Pour chaque fonction, écrivez votre version en C, compilez-la avec les mêmes options, et comparez votre désassemblage à celui donné ici. S'ils coïncident instruction par instruction, vous avez gagné.

Solutions : [solutions/partie1.md](../solutions/partie1.md#challenge-synthese)

## Suivi

- [ ] Challenge partie 2 : les trois fonctions reconstruites
