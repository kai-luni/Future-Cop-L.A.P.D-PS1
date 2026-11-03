#include "game/types.h"

struct Unk4749CResource {
    u8 unk00[0x10];
    s32 serial;
};

struct Unk4749CHandle {
    struct Unk4749CResource *resource;
    s32 serial;
};

struct SoundChannel;

struct SoundVoice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
    s32 priority;
    u8 unk0C[4];
    s32 unk10;
    u8 unk14[4];
    struct SoundChannel *channel;
    u8 unk1C[8];
    struct SoundVoice *bankNext;
    struct SoundVoice *bankPrev;
};

struct SoundChannel {
    s32 id;
    s32 priority;
    s16 slot;
    s8 index;
    u8 unk0B;
    s32 unk0C;
    s32 unk10;
    s8 unk14;
    s8 unk15;
    u8 unk16[0x12];
    struct SoundVoice *voice;
    struct SoundChannel *next;
    struct SoundChannel *prev;
};

struct SoundBank {
    s32 samples[4];
    u8 count;
    u8 cursor;
    u8 voices;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    u8 key;
    u8 unk17;
    struct SoundVoice *head;
};

struct Unk10E0 {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    struct SoundVoice *voice;
    s32 unk10;
};

struct UnkF744 {
    u8 pad00[8];
    s32 unk08;
    u8 pad0C[0x10];
    struct UnkF744 *next;
};

extern u8 D_8009F340;
extern s32 D_8009F32C;
extern s32 D_8009F344;
extern s32 D_8009F720;
extern struct SoundChannel *D_8009F728;
extern struct SoundChannel *D_8009F72C;
extern u8 D_8009F739;
extern struct UnkF744 *D_8009F744;

extern volatile s32 D_8009FA2C;
extern struct Unk4749CHandle D_800A10C0[];
extern struct Unk10E0 D_800A10E0[6];
extern struct SoundChannel *D_800A1780[];
extern struct SoundBank D_800A7938[];

extern s32 func_800474F0(struct Unk4749CHandle *);
extern void func_80023D78(void);
extern void func_8002A26C(void);
extern void func_80046698(struct SoundVoice *);
extern void func_8002441C(void);
extern s32 func_800458E8(s32, s32, s8 *, s32);
extern void func_800455DC(struct SoundChannel *);
extern void func_800460D4(s8 *);
extern void func_80047800(struct SoundVoice *, s32, s32, s32);
extern void func_80046804(struct SoundVoice *);
extern void func_800469A0(struct SoundVoice *);
extern struct SoundVoice *func_80046B04(s32, s32, s32, s32);
extern u8 func_8004772C(struct SoundBank *, struct SoundVoice *, s32, s32, s32);

void func_800462D0(void)
{
    register u32 work __asm__("$2");

    if (D_8009F340 == 0) {
        return;
    }

    while (D_8009F32C != 0) {
        D_8009F32C--;
        func_800474F0(&D_800A10C0[D_8009F32C]);
    }

    {
        register s32 delta __asm__("$18");

        {
            register s32 previous __asm__("$3");
            register s32 stamp __asm__("$4");

            work = D_8009FA2C;
            previous = D_8009F344;
            stamp = D_8009FA2C;
            delta = (s32)work - previous;
            D_8009F344 = stamp;
        }

        {
            register s32 clamped __asm__("$3");

            if (delta < 0) {
                goto delta_negative;
            }
            clamped = 0x12C;
            if (delta < 0x12D) {
                clamped = delta;
            }
            goto delta_ready;

delta_negative:
            clamped = 0;

delta_ready:
            delta = clamped;
        }

        func_80023D78();
        func_8002A26C();

        {
            register u32 index __asm__("$19");
            struct Unk10E0 *entry;
            register s32 *tag __asm__("$16");

            index = 0;
            entry = D_800A10E0;
            tag = &entry->unk10;
            do {
                s32 remaining;

                remaining = entry->unk00;
                if (remaining != 0) {
                    if (tag[-3] != 0) {
                        tag[-3]--;
                    } else {
                        entry->unk00 = remaining - delta;
                        if (entry->unk00 <= 0) {
                            struct SoundVoice *voice;

                            entry->unk00 = 0;
                            voice = *(struct SoundVoice **)(tag - 1);
                            if (tag[0] == voice->unk10) {
                                func_80046698(voice);
                            }
                        }
                    }
                }
                index++;
                tag += 5;
                entry++;
            } while (index < 6);
        }

        func_8002441C();

        {
            register struct UnkF744 *f744 __asm__("$17");

            f744 = D_8009F744;
            while (f744 != 0) {
                if (f744->unk08 > 0) {
                    f744->unk08 -= delta;
                }
                f744 = f744->next;
            }
        }
    }

    if (D_8009F720 == 0) {
        return;
    }
    if (D_8009F739 == 0) {
        return;
    }

    {
        struct SoundChannel **table;

        table = D_800A1780;
        {
            register struct SoundChannel *channel __asm__("$16");
            struct SoundBank *banks;

            channel = D_8009F72C;
            if (channel != 0) {
                banks = D_800A7938;
                do {
                    *table = channel;
                    if (func_800458E8(channel->unk0C, channel->unk10,
                                      &channel->unk14, 0)) {
                        channel->priority = channel->unk14 +
                            (banks[channel->slot].unk13 << 4);
                    } else {
                        channel->priority = 0;
                    }
                    channel = channel->prev;
                    table++;
                } while (channel != 0);
            }
        }

        {
            register s32 count __asm__("$18");
            register s32 index __asm__("$16");

            table = D_800A1780;
            count = D_8009F720;
            D_8009F728 = 0;
            D_8009F72C = 0;
            D_8009F720 = 0;
            index = 0;
            while (index < count) {
                struct SoundChannel *queued;

                queued = *table++;
                index++;
                func_800455DC(queued);
            }
        }
    }

    {
        register struct SoundChannel *channel __asm__("$16");
        register s32 active __asm__("$19");
        struct SoundBank *banks;
        register struct SoundVoice *release __asm__("$4");

        channel = D_8009F728;
        if (channel == 0) {
            return;
        }
        active = 0;
        banks = D_800A7938;

        do {
            if (active >= 8 || channel->priority == 0) {
                goto release_current;
            }

            if (channel->voice != 0) {
                func_800460D4(&channel->unk14);
                func_80047800(channel->voice, channel->unk15,
                              channel->unk14, -1);
                active++;
                if (channel->voice->priority != channel->priority) {
                    channel->voice->priority = channel->priority;
                    func_80046804(channel->voice);
                    func_800469A0(channel->voice);
                }
                goto next_channel;
            }

            {
                register s32 slot __asm__("$7");
                register struct SoundBank *bank __asm__("$18");
                register struct SoundVoice *voice __asm__("$17");
                u8 started;

                slot = channel->slot;
                work = (u32)slot * sizeof(struct SoundBank);
                bank = (struct SoundBank *)(work + (u32)banks);
                voice = func_80046B04(channel->unk14, bank->unk13, 0, slot);
                if (voice == 0) {
                    goto next_channel;
                }

                voice->channel = channel;
                channel->voice = voice;
                func_800460D4(&channel->unk14);
                started = func_8004772C(bank, voice, channel->unk15,
                                       channel->unk14, -1);
                release = voice;
                if (started == 0) {
                    goto release_voice;
                }
                active++;
                goto next_channel;
            }

release_current:
            release = channel->voice;
            if (release == 0) {
                goto next_channel;
            }

release_voice:
            func_80046698(release);

next_channel:
            channel = channel->next;
        } while (channel != 0);
    }
}
