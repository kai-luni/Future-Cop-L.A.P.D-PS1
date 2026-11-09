struct CallbackTable {
    void *reserved[14];
    int (*callback)(void);
};

extern struct CallbackTable *D_8009DDE8;

unsigned int func_80093924(void)
{
    return (unsigned int)D_8009DDE8->callback() >> 31;
}
