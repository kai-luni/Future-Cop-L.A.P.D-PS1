struct CallbackTable {
    void (*reserved[6])(void);
    void (*callback)(void);
};

extern struct CallbackTable *D_8009DD10;

void func_800915A4(void)
{
    D_8009DD10->callback();
}
