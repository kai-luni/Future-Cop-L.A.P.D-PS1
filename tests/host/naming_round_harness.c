/* Exercise original C entry points used by the naming/documentation round. */
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

struct Node {
    struct Node *next;
    u8 pad[0x30];
    void *buffer;
    u8 unused;
    u8 flags;
};
struct CallbackTable {
    void (*reserved[5])(void);
    void (*callback)(void);
};
struct MovieContext { u8 pad[0x11c]; s32 value; };
struct AudioState { volatile u32 start; u32 end; u8 pad[0x7b70]; u32 flag; };
struct ScriptTimer { s32 count, period, remaining, trigger, action; };
struct ModeObject { u8 pad[8]; s32 value; };
struct ModeState { s32 mode; struct ModeObject *object; s32 value; };
struct Entity { struct Entity *next; void *update; s32 id; };
struct PositionAngle { s32 position[3]; s32 angle; };
struct MotionState {
    s32 x, y, scale;
    short angle;
    u8 pad0[2];
    short velocity;
    u8 pad1[2];
    s32 timer;
    u8 enabled, state;
};

extern void func_8003C4C4(u8 column, s32 row, u8 bits);
extern void func_8003CF20(struct Node *object);
extern void func_8002A23C(void);
extern void func_80085900(s32 value);
extern void *func_800869D8(u32 index);
extern void func_80091544(void);
extern struct Entity *func_8004D664(s32 id);
extern void func_80045884(s32 *position, s32 unused, s32 angle, s32 index);
extern void func_8005CEA8(struct MotionState *state);

struct PositionAngle D_800A1920[3];
extern void func_8004953C(s32 index, s32 kind, s32 value);
extern u32 func_80029BE4(void);
extern void func_80044FF8(s32 mode, struct ModeObject *object);
extern void func_80047690(u8 enable);
extern void func_80049410(void);
extern void func_8002A598(void);
extern void func_8003FD6C(void);
extern void func_8002A5AC(void);

struct Entity *D_8009FA40;
struct MovieContext *D_8009F4BC;
struct AudioState *D_8009F4D8;
struct CallbackTable *D_8009DD10;
u32 D_8009F6DC;
struct ScriptTimer *D_8009F770;
s32 D_8009F774;
struct ModeState D_800A1800;
u8 D_8009F739;
u32 D_8009FA18, D_8009F6F0;
static u32 stream_value;
static int stream_checks;
static struct MovieContext movie;
static struct AudioState audio;
static struct CallbackTable callbacks;
static u32 cell;
static u32 got_column, got_row;
static void *freed[2];
static int free_count;
static int calls[4], call_count;
static int callback_count;

void __main(void) {}
u32 *func_8003B3D8(u32 column, u32 row)
{
    got_column = column;
    got_row = row;
    return &cell;
}
void func_800181D4(void *block) { freed[free_count++] = block; }
void func_80024630(s32 value) { calls[call_count++] = 100 + value; }
void func_8002A16C(s32 value) { calls[call_count++] = 200 + value; }
void func_80047554(s32 value) { calls[call_count++] = 300 + value; }
u32 func_8002443C(void) { stream_checks++; return stream_value; }
static void callback(void) { callback_count++; }

static void say(const char *text)
{
    int length = 0;
    register long v0 __asm__("$2") = 4004;
    register long a0 __asm__("$4") = 1;
    register long a1 __asm__("$5") = (long)text;
    register long a2 __asm__("$6");
    while (text[length]) length++;
    a2 = length;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2)
                     : "memory", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25");
}

#define CHECK(condition, message) do { if (!(condition)) { say("FAIL: " message "\n"); failures++; } else { checks++; } } while (0)

int main(void)
{
    struct Node node;
    struct Entity first, second, third;
    struct MotionState motion;
    struct ScriptTimer timers[2];
    struct ModeObject mode_object;
    s32 position[3];
    int buffer;
    int checks = 0, failures = 0;

    cell = 0x89abcdef;
    func_8003C4C4(3, 7, 0x10);
    CHECK(cell == 0x89abcdff, "grid flag byte OR");
    CHECK(got_column == 3 && got_row == 7, "grid coordinates forwarded");
    func_8003C4C4(4, -1, 0);
    CHECK(cell == 0x89abcdff && got_row == (u32)-1, "grid zero mask and signed row");

    node.buffer = &buffer;
    node.flags = 0;
    free_count = 0;
    func_8003CF20(&node);
    CHECK(free_count == 1 && freed[0] == &node, "unowned node frees node only");
    node.flags = 2;
    free_count = 0;
    func_8003CF20(&node);
    CHECK(free_count == 2 && freed[0] == &buffer && freed[1] == &node, "owned buffer freed before node");
    node.flags = 1;
    free_count = 0;
    func_8003CF20(&node);
    CHECK(free_count == 1 && freed[0] == &node, "unrelated flag leaves buffer alone");

    D_8009F6DC = 0;
    call_count = 0;
    func_8002A23C();
    CHECK(call_count == 0, "zero gate suppresses calls");
    D_8009F6DC = 9;
    func_8002A23C();
    CHECK(call_count == 2 && calls[0] == 100 && calls[1] == 201, "nonzero gate forwards arguments in order");

    D_8009F4BC = &movie;
    movie.value = 123;
    func_80085900(-47);
    CHECK(movie.value == -47, "movie field signed write");
    func_80085900(0);
    CHECK(movie.value == 0, "movie field zero write");

    D_8009F4D8 = &audio;
    audio.start = 15361;
    audio.end = 15365;
    audio.flag = 0;
    CHECK(func_800869D8(0) == (u8 *)&audio + 17, "audio buffer wraps start");
    CHECK(func_800869D8(3) == (u8 *)&audio + 17, "audio index does not move pointer");
    CHECK(func_800869D8(4) == 0, "audio end excluded");
    audio.flag = 1;
    CHECK(func_800869D8(0) == 0, "audio state flag prevents access");
    audio.flag = 0;
    audio.start = 0xfffffffeU;
    audio.end = 1;
    CHECK(func_800869D8(2) == (u8 *)&audio + (audio.start % 15360) + 16, "unsigned availability wraps");
    CHECK(func_800869D8(3) == 0, "wrapped availability end excluded");

    callbacks.callback = callback;
    D_8009DD10 = &callbacks;
    callback_count = 0;
    func_80091544();
    func_80091544();
    CHECK(callback_count == 2, "callback invoked on each dispatch");

    D_8009FA40 = 0;
    CHECK(func_8004D664(3) == 0, "lookup empty list");
    first.next = &second; first.id = 4;
    second.next = &third; second.id = 9;
    third.next = 0; third.id = 9;
    D_8009FA40 = &first;
    CHECK(func_8004D664(4) == &first, "lookup first entry");
    CHECK(func_8004D664(9) == &second, "lookup first duplicate");
    CHECK(func_8004D664(-1) == 0, "lookup missing id");

    position[0] = -5; position[1] = 6; position[2] = 700;
    D_800A1920[0].angle = 99;
    func_80045884(position, 123, 0x1001, 1);
    CHECK(D_800A1920[1].position[0] == -5 && D_800A1920[1].position[1] == 6 && D_800A1920[1].position[2] == 700, "position copied");
    CHECK(D_800A1920[1].angle == 1 && D_800A1920[0].angle == 99, "angle reduced and other record untouched");
    func_80045884(position, 0, -4097, 2);
    CHECK(D_800A1920[2].angle == -1, "negative signed remainder");

    motion.x = 1; motion.y = 2; motion.scale = 3; motion.angle = 4;
    motion.velocity = 5; motion.timer = 6; motion.enabled = 0; motion.state = 8;
    motion.pad0[0] = 0xaa; motion.pad1[1] = 0xbb;
    func_8005CEA8(&motion);
    CHECK(motion.x == 0 && motion.y == 0 && motion.scale == 0x10000, "motion position and scale reset");
    CHECK(motion.angle == 0 && motion.velocity == 0 && motion.timer == 0 && motion.enabled == 1 && motion.state == 0, "motion fields reset");
    CHECK(motion.pad0[0] == 0xaa && motion.pad1[1] == 0xbb, "motion padding untouched");

    timers[0].count = 12; timers[1].count = 34; timers[1].period = 88;
    D_8009F770 = timers;
    func_8004953C(1, 0x7d, -7);
    CHECK(timers[1].count == 34, "other kind preserves timer");
    func_8004953C(1, 0x7e, -7);
    CHECK(timers[0].count == 12 && timers[1].count == -7 && timers[1].period == 88, "timer count update");
    D_8009F774 = 2;
    func_80049410();
    CHECK(D_8009F770 == 0 && D_8009F774 == 0, "timer table reset");

    stream_checks = 0;
    D_8009F6DC = 0;
    CHECK(func_80029BE4() == 1 && stream_checks == 0, "zero state ready without query");
    D_8009F6DC = 3;
    CHECK(func_80029BE4() == 0 && stream_checks == 0, "state three not ready");
    D_8009F6DC = 4;
    CHECK(func_80029BE4() == 0 && stream_checks == 0, "state four not ready");
    D_8009F6DC = 2; stream_value = 0x226;
    CHECK(func_80029BE4() == 0 && stream_checks == 1, "stream below threshold");
    stream_value = 0x227;
    CHECK(func_80029BE4() == 1 && stream_checks == 2, "stream threshold inclusive");
    stream_value = 0xffffffffU;
    CHECK(func_80029BE4() == 1, "stream comparison is unsigned");

    mode_object.value = -99;
    func_80044FF8(2, &mode_object);
    CHECK(D_800A1800.mode == 2 && D_800A1800.object == &mode_object && D_800A1800.value == -99, "mode stores object and value");
    func_80044FF8(-1, 0);
    CHECK(D_800A1800.mode == -1 && D_800A1800.object == &mode_object && D_800A1800.value == -99, "mode minus one preserves object");
    mode_object.value = 17;
    func_80044FF8(-2, &mode_object);
    CHECK(D_800A1800.mode == -2 && D_800A1800.value == 17, "other negative mode updates value");

    call_count = 0; D_8009F739 = 0;
    func_80047690(2);
    CHECK(D_8009F739 == 1 && call_count == 1 && calls[0] == 201, "audio enable flag and call");
    func_80047690(0);
    CHECK(D_8009F739 == 1 && call_count == 2 && calls[1] == 300, "audio disable halts without clearing flag");
    D_8009FA18 = 0xffffffffU; D_8009F6F0 = 123;
    func_8002A598();
    CHECK(D_8009FA18 == 0 && D_8009F6F0 == 0, "mission state words reset");
    func_8003FD6C();
    func_8002A5AC();
    CHECK(D_8009FA18 == 0 && D_8009F6F0 == 0 && call_count == 2, "empty entry points leave observed state untouched");

    if (failures) return 1;
    if (checks != 43) { say("FAIL: incorrect check count\n"); return 1; }
    say("ALL PASS (naming round: 17 functions, 43 checks)\n");
    return 0;
}
