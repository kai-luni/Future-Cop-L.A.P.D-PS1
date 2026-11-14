struct CallbackTable {
    void (*reserved_00)(void);
    void (*reserved_04)(void);
    void (*reserved_08)(void);
    void (*reserved_0c)(void);
    void (*reserved_10)(void);
    void (*callback)(void);
};

extern struct CallbackTable *D_8009DD10;

void func_80091544(void)
{
    D_8009DD10->callback();
}
