/* The ST reset handler calls __libc_init_array(). With -nostartfiles the
 * application provides these hooks instead of linking a second CRT startup.
 * No semihosting, stdio syscalls or dynamic libc heap are used by this baseline.
 */
void _init(void)
{
}

void _fini(void)
{
}
