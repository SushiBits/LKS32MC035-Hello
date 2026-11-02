/**
 * @file LKS32MC03x.ld
 *
 * Linker script snippet for LKS32MC03x, indicating memory layout.
 */

MEMORY {
    IROM (r x) : ORIGIN = 0x00000000, LENGTH = 32k
    IRAM (rw ) : ORIGIN = 0x20000000, LENGTH = 4k
}
