#define VGA_ADDRESS 0xB8000 /* adresse fixe du buffer texte VGA */
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_COLOR   0x0F     /* blanc sur fond noir */

static unsigned short *const vga = (unsigned short *)VGA_ADDRESS;
static int cursor = 0; /* position du prochain caractère dans le buffer */

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
