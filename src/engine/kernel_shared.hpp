// Minimal layout contract for the source-derived DOS kernels.
// Member order and widths follow pinned `ocrash.hpp` (Cannonball 27493eb).
// This intentionally excludes constructors/runtime ownership; the combined
// kernel supplies one shared OCrash instance and one shared OFerrari instance.
#pragma once
#include <stdint.h>

class oentry;
class OCrash {
public:
    oentry *spr_ferrari, *spr_shadow, *spr_pass1, *spr_pass1s, *spr_pass2, *spr_pass2s;
    enum { SKID_RESET=20, SKID_MAX=30, SKID_X_ADJ=24 };
    int8_t crash_state;
    int16_t skid_counter, skid_counter_bak;
    uint8_t spin_control1, spin_control2;
    int16_t coll_count1, coll_count2, crash_counter, crash_spin_count, crash_z;

    // Source-private state retained in exact declaration order and widths.
    int16_t spinflipcount1, spinflipcount2, slide, frame;
    uint32_t addr;
    int16_t camera_x_target, camera_xinc, lookup_index, frame_restore, shift;
    int16_t crash_speed, crash_zinc, crash_side, spin_pass_frame;
    int8_t crash_type;
    enum { CRASH_BUMP=0, CRASH_SPIN=1, CRASH_FLIP=2 };
    int16_t crash_delay;
    void (OCrash::*function_pass1)(oentry*);
    void (OCrash::*function_pass2)(oentry*);

    void init(oentry*,oentry*,oentry*,oentry*,oentry*,oentry*);
    bool is_flip(); void enable(); void clear_crash_state(); void tick();
    void do_crash(); void spin_switch(const uint16_t); void crash_switch();
    void init_collision(); void do_collision(); void end_collision();
    void do_bump(); void do_car_flip(); void init_finger(uint32_t);
    void trigger_smoke(); void post_flip_anim(); void pan_camera();
    void init_spin1(); void init_spin2();
    void collide_slow(); void collide_med(); void collide_fast();
    void done(oentry*); void do_shadow(oentry*,oentry*);
    void do_crash_passengers(oentry*); void flip_start(oentry*);
    void crash_pass1(oentry*); void crash_pass2(oentry*); void crash_pass_flip(oentry*);
    void pass_flip(oentry*); void pass_situp(oentry*); void pass_turnhead(oentry*);
};

// Shared movement boundary agreed with the handling kernel. `car_increment`
// is the source uint32 fixed-point value; no duplicate movement step belongs
// in the crash runtime. The exact shared state also includes car_x_old,
// road_curve, road_type, and rd_split_state as signed 16-bit fields.
struct CrashHandlingBoundary {
    uint32_t car_increment;
    int16_t car_x_pos, car_x_old, road_curve, road_type, rd_split_state;
    int16_t coll_count1, skid_counter, crash_counter;
    uint8_t spin_control1;
};
