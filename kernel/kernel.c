#define VGA_ADDRESS 0xB8000 /* adresse fixe du buffer texte VGA */
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_COLOR   0x0F     /* blanc sur fond noir */

/* Types kernel : pas de libc ici, donc on définit nous-mêmes les tailles fixes (i386) */
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned int   size_t;

static unsigned short *const vga = (unsigned short *)VGA_ADDRESS;
static int cursor = 0; /* position du prochain caractère dans le buffer */

/* Lib de base : -nostdlib enlève la libc, et gcc peut appeler memset/memcpy de lui-même */

size_t strlen(const char *s)
{
    size_t len = 0;

    while (s[len] != '\0')
        len++;
    return len;
}

void *memset(void *dst, int value, size_t n)
{
    uint8_t *bytes = (uint8_t *)dst;

    for (size_t i = 0; i < n; i++)
        bytes[i] = (uint8_t)value;
    return dst;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    uint8_t *to = (uint8_t *)dst;
    const uint8_t *from = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++)
        to[i] = from[i];
    return dst;
}

void clear_screen(void)
{
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        vga[i] = (VGA_COLOR << 8) | ' '; /* chaque case VGA = (couleur << 8) | caractère */
    cursor = 0;
}

void putchar(char c)
{
    vga[cursor++] = (VGA_COLOR << 8) | (unsigned char)c;
}

void print(const char *s)
{
    while (*s)
        putchar(*s++);
}

void kmain(void)
{
    clear_screen();
    print("42");

    while (1)
        __asm__ volatile ("hlt"); /* le kernel n'a rien d'autre à faire, on boucle */
}
