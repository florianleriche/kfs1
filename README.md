KFS_1 - petit kernel i386 qui boote via GRUB et affiche "42".

Déroulé de l'exécution :

1. `make` compile `boot/boot.asm` (nasm) et `kernel/kernel.c` (gcc), puis `ld` les lie avec `linker.ld` pour produire `kernel.elf`.
2. `make iso` copie `kernel.elf` dans `iso/boot/` et appelle `grub-mkrescue` pour générer `kfs.iso`, une image bootable contenant GRUB.
3. `make run` lance `kfs.iso` dans QEMU. GRUB démarre, lit `iso/boot/grub/grub.cfg` et charge `kernel.elf` en mémoire avec l'instruction `multiboot`.
4. GRUB saute sur le point d'entrée `_start` défini dans `boot.asm`. Ce fichier contient d'abord le header multiboot : `MB_MAGIC` (0x1BADB002) est la valeur que GRUB cherche pour reconnaître un kernel valide, et `MB_CHECKSUM` doit annuler `MB_MAGIC + MB_FLAGS` (magic + flags + checksum = 0), sinon GRUB refuse de booter.
5. `_start` fait `cli` (coupe les interruptions, rien n'est encore prêt pour les gérer), pose sa propre pile avec `mov esp, stack_top` (celle du host n'est pas garantie valide), puis `call kmain` pour sauter dans le code C.
6. `kmain` appelle `clear_screen()` qui remplit les 80x25 cases du buffer VGA (adresse `0xB8000`) avec des espaces ; chaque case VGA tient sur 16 bits = `(couleur << 8) | caractère`. Ensuite `print("42")` appelle `putchar()` pour chaque caractère, qui écrit directement dans ce buffer à la position `cursor`.
7. `kmain` termine par `while (1) hlt` : le kernel n'a rien d'autre à faire, donc il arrête le CPU en boucle plutôt que de planter ou repartir dans n'importe quoi.

`linker.ld` fixe l'adresse de chargement à `. = 1M` (attendue par GRUB) et place `.multiboot` en tout premier (`KEEP(*(.multiboot))`) pour que GRUB trouve le header dès le début du binaire, avant `.text`, `.rodata`, `.data` et `.bss`.

Le `Makefile` centralise tout ça : `-ffreestanding -fno-builtin -nostdlib -nodefaultlibs` garantissent que le kernel ne dépend d'aucune lib du système hôte (sinon il ne bootera jamais tout seul), `-m32`/`-m elf_i386` forcent le i386 32 bits, et `-T linker.ld` dit à `ld` d'utiliser notre script au lieu de celui du host.

Commandes utiles :

- build + iso : `docker compose run --rm kfs bash -c "make re && make iso"`
- lancer dans QEMU (texte, montre le "42") : `docker compose run --rm kfs bash -c "qemu-system-i386 -cdrom kfs.iso -m 32M -boot order=d,menu=off -nic none -display curses"`
- vérifier que `kmain` tourne sans crash (headless) : `docker compose run --rm kfs bash -c "qemu-system-i386 -cdrom kfs.iso -m 32M -boot order=d,menu=off -nic none -display none"`
- taille de l'image (doit être < 10 Mo) : `ls -la kfs.iso`
- nettoyer : `docker compose run --rm kfs bash -c "make fclean"`

