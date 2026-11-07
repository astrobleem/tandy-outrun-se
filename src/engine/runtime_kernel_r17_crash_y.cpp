// Derived from Cannonball by Chris White and the Cannonball team.
// Noncommercial redistribution terms: see LICENSE.Cannonball.
#if !defined(__I86__) && defined(RUNTIME_HOST_FIXTURE)
#include <stdio.h>
#endif
#if !defined(__I86__) && defined(RUNTIME_HOST_FIXTURE)
#include <assert.h>
#endif
#if !defined(__I86__) && defined(RUNTIME_ASSERT_CRASH_FRAME)
#include <assert.h>
#include <stdio.h>
#endif
// Generated sparse-ROM source kernel. Marked functions are exact pinned method bodies.
#include <stdint.h>
#include <stdlib.h>
#include "kernel_shared.hpp"
#include "kernel_inputs.hpp"
struct oentry {
 uint8_t control,jump_index; int8_t function_holder; uint8_t id,shadow,zoom,draw_props;
 uint16_t pal_src; uint8_t pal_dst; int16_t x,y; uint16_t width,priority,dst_index; uint32_t addr;
 uint16_t road_priority; int16_t reload; uint16_t counter; int16_t xw1; int32_t z; int16_t traffic_speed;
 uint16_t type; int16_t xw2; uint8_t traffic_proximity,traffic_fx; int16_t traffic_orig_speed,traffic_near_speed;
 uint16_t yw; int16_t pass_props;
 enum {BOTTOM=8};
};
struct OSprites { enum {ENABLE=0x80,HFLIP=1,WIDE_ROAD=4,TRAFFIC_RHS=0x40,SPRITE_ZOOM_LOOKUP=0x28000}; uint16_t sprite_scroll_speed; void map_palette(oentry*){} void do_spr_order_shadows(oentry*){} void move_sprite(oentry*,uint8_t); } osprites;
#include "engine_rom_segments.inc"
struct RomSegment { uint32_t base; uint16_t size; const uint8_t *bytes; };
static const RomSegment rom_segments[] = {
    {0x1000,0x0200,ROM_SEG_0},
    {0x2200,0x0490,ROM_SEG_1},
    {0x30800,0x0100,ROM_SEG_2},
    {0x1212A,0x0400,ROM_SEG_3},
    {0x4CFA,0x00A0,ROM_SEG_4}
};
static uint32_t rom_fault_addr=0;
#include "engine_rom_traffic.inc"
static uint8_t source_rom8(uint32_t a) {
    if (a>=0x28000UL && a<0x30000UL) {
        uint16_t p=(uint16_t)(a-0x28000UL), word_off=(uint16_t)(p&0xFFFC);
        uint16_t z=(word_off>>2)&0x1FF, step=(word_off>>11)&0x0F;
        uint32_t v=((uint32_t)step*(uint32_t)z)<<10;
        return (uint8_t)(v>>(24-((p&3)<<3)));
    }
    if (a>=0x5424UL && a<0x5524UL) return RUNTIME_TRAFFIC_ROM[(uint16_t)(a-0x5424UL)];
    for (uint8_t i=0;i<5;i++) if (a>=rom_segments[i].base && a-rom_segments[i].base<rom_segments[i].size)
        return rom_segments[i].bytes[(uint16_t)(a-rom_segments[i].base)];
    if (!rom_fault_addr) rom_fault_addr=a; return 0;
}
struct Rom {
    uint16_t read16(uint32_t *a){ uint16_t v=((uint16_t)source_rom8(*a)<<8)|source_rom8(*a+1); *a+=2; return v; }
    uint32_t read32(uint32_t a);
    uint8_t read8(uint32_t a){return source_rom8(a);}
};
uint32_t Rom::read32(uint32_t a){
  if(a>=0x28000UL && a<=0x2FFFCUL && (a&3)==0){
   uint16_t p=(uint16_t)(a-0x28000UL),z=(p>>2)&0x1FF,step=(p>>11)&0x0F;
   uint16_t product=(uint16_t)(step*z); return ((uint32_t)product)<<10;
  }
  if(a>=0x5424UL && a<=0x5520UL){
   const uint8_t *p=RUNTIME_TRAFFIC_ROM+(uint16_t)(a-0x5424UL);
   return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];
  }
  for(uint8_t i=0;i<5;i++){
   uint32_t base=rom_segments[i].base;
   if(a>=base && a<=base+(uint32_t)rom_segments[i].size-4){
    const uint8_t *p=rom_segments[i].bytes+(uint16_t)(a-base);
    return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];
   }
  }
  return ((uint32_t)source_rom8(a)<<24)|((uint32_t)source_rom8(a+1)<<16)|((uint32_t)source_rom8(a+2)<<8)|source_rom8(a+3);
 }
struct Roms { Rom rom0; Rom *rom0p; } roms;

struct Adr { uint32_t traffic_props,traffic_data,sprite_bump_data1,sprite_bump_data2,sprite_crash_spin1,sprite_crash_spin2,sprite_crash_man1,sprite_crash_girl1,sprite_crash_flip,sprite_crash_flip_m1,sprite_crash_flip_g1,sprite_crash_flip_m2,sprite_crash_flip_g2,sprite_crash_man2,sprite_crash_girl2; };
struct Outrun { enum {MODE_TTRIAL=1,MODE_ORIGINAL=0}; int16_t game_state,cannonball_mode; uint8_t tick_frame; Adr adr; struct {uint16_t vehicle_cols,overtakes,crashes;} ttrial; } outrun;
enum {GS_INIT=0,GS_ATTRACT=1,GS_INIT_BEST1=2,GS_BEST1=3,GS_INIT_LOGO=4,GS_LOGO=5,GS_INIT_MUSIC=6,GS_MUSIC=7,GS_INIT_GAME=8,GS_START1=9,GS_START2=10,GS_START3=11,GS_INGAME=12,GS_INIT_BONUS=13,GS_BONUS=14,GS_INIT_GAMEOVER=15,GS_GAMEOVER=16,GS_INIT_MAP=17,GS_MAP=18,GS_INIT_BEST2=19,GS_BEST2=20,GS_REINIT=21,GS_CALIBRATE_MOTOR=100,HUD_SCORE1=1,HUD_SCORE2=2,DATA_MOVEMENT=0x30800,BIT_0=1,BIT_7=0x80};
namespace sound { enum {STOP_SLIP=0,INIT_SLIP=1,REBOUND=2,CRASH1=3,CRASH2=4,RESET=5}; }
struct Sound { uint8_t last; uint16_t count; void queue_sound(uint8_t x){last=x;count++;} } osoundint;
struct OBonus { enum {BONUS_SEQ0=0}; int16_t bonus_control,bonus_state; void do_bonus_text(){} } obonus;
struct OAnimseq { void init_end_seq(){} } oanimseq;
enum { FORCE_AI=0 };
Input input; OInputs oinputs;
struct OInitEngine { enum {ROAD_NOCHANGE=0,ROAD_STRAIGHT=1,ROAD_RIGHT=2,ROAD_LEFT=3}; int16_t rd_split_state,road_type,road_type_next,road_curve,car_x_pos,car_x_old,ingame_counter,granular_rem; uint16_t pos_fine_old; uint32_t car_increment; bool ingame_engine; void init_crash_bonus(); void test_bonus_mode(bool); void set_granular_position(); void set_fine_position(); } oinitengine;
struct ORoad { enum {ROAD_OFF=0,ROAD_R0=1,ROAD_R1=2,ROAD_BOTH_P0=3,ROAD_BOTH_P1=4,ROAD_BOTH_P0_INV=5,ROAD_BOTH_P1_INV=6,ROAD_R0_SPLIT=7,ROAD_R1_SPLIT=8,VIEW_INCAR=2}; int16_t road_x[512],road_y[528],road0_h[512],road1_h[512],road_p0,road_ctrl,road_width_bak,car_x_bak,view_mode; uint16_t pos_fine; uint32_t road_pos; int32_t road_pos_change,road_width; int get_view_mode(){return view_mode;} } oroad;
static int16_t runtime_crash_road_y(uint16_t z);
class OStats { public: uint32_t score; int8_t cur_stage; int16_t time_counter; void convert_speed_score(uint16_t); void update_score(uint32_t); } ostats;
class OFerrari { public:
 enum {WHEELS_ON=0,WHEELS_LEFT_OFF=1,WHEELS_RIGHT_OFF=2,WHEELS_OFF=3,TRACTION_ON=0,CAR_NORMAL=0,CAR_SMOKE=1,CAR_ANIM_SEQ=-1,PAL_RED=2};
 static const uint32_t MAX_SPEED=0x1260000UL,CAR_BASE_INC=0x12fUL; static const uint16_t OFFROAD_BOUNDS=0x1f4;
 static uint16_t torque_lookup[32]; static const uint8_t rev_inc_lookup[256];
 oentry *spr_ferrari,*spr_shadow,*spr_pass1,*spr_pass2;
 int16_t steering_old,torque,accel_value,accel_value_bak,brake_value,acc_adjust1,acc_adjust2,acc_adjust3,brake_adjust1,brake_adjust2,brake_adjust3,gear_smoke,gfx_smoke,road_width_old,counter;
 int32_t revs,brake_subtract,rev_adjust; uint16_t is_slipping,car_inc_old; int16_t car_x_diff,rev_stop_flag,revs_post_stop,acc_post_stop,rev_pitch1,rev_pitch2;
 uint8_t rev_shift,wheel_state,wheel_traction,slip_sound,torque_index; int8_t gear_counter,cornering,cornering_old,car_state;
 bool car_ctrl_active,auto_brake,gear_value,gear_bak; uint16_t ferrari_pal;
 void move(); void car_acc_brake(); void do_gear_torque(int16_t&); void do_gear_low(int16_t&); void do_gear_high(int16_t&); void tick_engine_disabled(int32_t&);
 int32_t tick_gear_change(int16_t); int32_t get_speed_inc_value(uint16_t,uint32_t); int32_t get_speed_dec_value(uint16_t);
 void set_brake_subtract(); void finalise_revs(int32_t&,int32_t); void convert_revs_speed(int32_t,int32_t&); void update_road_pos();
 void set_curve_adjust(); void set_ferrari_x(); void do_skid(); void set_ferrari_bounds(); void check_wheels(); void set_wheels(uint8_t); int32_t tick_smoke(); void reset_car();
 } oferrari;
OCrash ocrash;
oentry crash_actors[6],normal_actors[4];
class OLevelObjs { public: enum {COLLISION_RESET=4}; uint8_t collision_sprite; int16_t spray_counter,sprite_collision_counter; void setup_sprite_routine(oentry*); void sprite_normal(oentry*,uint8_t); void sprite_lights(oentry*); void sprite_lights_countdown(oentry*); void sprite_collision_z1c(oentry*); void sprite_rocks(oentry*); void set_spr_zoom_priority(oentry*,uint8_t); void set_spr_zoom_priority_display(oentry*,uint8_t); void set_spr_zoom_priority2(oentry*,uint8_t); void set_spr_zoom_priority_rocks(oentry*,uint8_t); void hide_sprite(oentry*); } olevelobjs;
class OTraffic { public: uint16_t collision_traffic,collision_mask; int16_t traffic_speed_total; uint8_t traffic_pal_cycle; void check_collision(oentry*); void update_props(oentry*); void set_zoom_lookup(oentry*); } otraffic;
struct Config { struct { bool bumper,fix_bugs,offroad,turbo,grippy_tyres,randomgen; } engine; struct {enum {GEAR_BUTTON=0,GEAR_AUTO=1}; int16_t gear;} controls; int16_t tick_fps,s16_x_off; } config;
struct Hud { void draw_score_ingame(uint32_t){} void draw_score(int,int,int){} int translate(int,int){return 0;} void blit_text1(int,int,int){} void draw_rev_counter(){} } ohud;
namespace outils {
static uint32_t rnd_seed=0;
void move16(uint32_t src,uint32_t& dst){dst=(dst&0xFFFF0000UL)+(src&0xFFFFUL);}
void add16(uint32_t src,uint32_t& dst){dst=(dst&0xFFFF0000UL)+(((dst&0xFFFFUL)+(src&0xFFFFUL))&0xFFFFUL);}
void swap32(uint32_t& v){v=((v&0xFFFF0000UL)>>16)+((v&0xFFFFUL)<<16);}
void sub16(int32_t src,int32_t& dst){dst=(dst&0xFFFF0000L)+(((dst&0xFFFFL)-(src&0xFFFFL))&0xFFFFL);}
void set_fixture_seed(uint32_t v){rnd_seed=v;}
void reset_random_seed(){rnd_seed=0;}
uint32_t bcd_add(uint32_t,uint32_t); uint32_t random(); int convert16_dechex(int x){return x;}
}
const uint32_t SPRITE_X_OFFS=0x1212A; const uint8_t COLLISION_RESET=4;


/* Exact source-derived OFerrari methods, separately hash-verified. */
/* Cannonball-derived method bodies, adapted to DOS16 integer widths. */
uint16_t OFerrari::torque_lookup[32]={

    0x2600, // Offset 0x00 - Start line
    0x243C, // ..
    0x2278, // values only used when pulling away from start line
    0x20B4,
    0x1EF0,
    0x1D2C,
    0x1B68,
    0x19A4,
    0x17E0,
    0x161C,
    0x1458,
    0x1294,
    0x10D0,
    0xF0C,
    0xD48,
    0xB84,
    0x9BB, // Offset 0x10 - Low Gear
    0x983, // ..
    0x94B, // .. in between these values
    0x913, // .. is the lookup as we shift between
    0x8DB, // .. the two gears
    0x8A3, // ..
    0x86B,
    0x833,
    0x7FB,
    0x7C2,
    0x789,
    0x750,
    0x717,
    0x6DE,
    0x6A5,
    0x66C, // Offset 0x1F - High Gear
};
const uint8_t OFerrari::rev_inc_lookup[256]={

    0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15, 0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19,
    0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
    0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B,
    0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D,
    0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x20, 0x20, 0x20, 0x20,
    0x21, 0x21, 0x22, 0x22, 0x23, 0x23, 0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27, 0x28, 0x28,
    0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B, 0x2B, 0x2C, 0x2C, 0x2C, 0x2D, 0x2D, 0x2D, 0x2E, 0x2E, 0x2E,
    0x2F, 0x2F, 0x2F, 0x2F, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2F, 0x2F, 0x2F, 0x2F,
    0x2E, 0x2E, 0x2E, 0x2D, 0x2D, 0x2D, 0x2C, 0x2C, 0x2B, 0x2B, 0x2A, 0x2A, 0x29, 0x29, 0x28, 0x28,
    0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20, 0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
    0x17, 0x15, 0x13, 0x11, 0x0F, 0x0D, 0x0B, 0x0A, 0x09, 0x09, 0x08, 0x08, 0x07, 0x07, 0x06, 0x06,
    0x05, 0x05, 0x05, 0x04, 0x04, 0x04, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01
};
void OFerrari::move()
{
    if (car_ctrl_active)
    {
        // Auto braking if necessary
        if (outrun.game_state != GS_ATTRACT && auto_brake)
            oinputs.acc_adjust = 0;

        // Set Gear For Demo Mode
        if (FORCE_AI ||
            outrun.game_state == GS_ATTRACT || outrun.game_state == GS_BONUS ||
            config.controls.gear == config.controls.GEAR_AUTO)
        {
            // demo_mode_gear
            oinputs.gear = (oinitengine.car_increment >> 16 > 0xA0);
        }

        gfx_smoke = 0;

        // --------------------------------------------------------------------
        // CRASH CODE - Slow Car
        // --------------------------------------------------------------------
        if (ocrash.crash_counter && ocrash.spin_control1 <= 0)
        {
            oinitengine.car_increment = (oinitengine.car_increment & 0xFFFF) | ((((oinitengine.car_increment >> 16) * 31) >> 5) << 16);
            revs = 0;
            gear_value = 0;
            gear_bak = 0;
            gear_smoke = 0;
            torque = 0x1000;
        }
        // --------------------------------------------------------------------
        // NO CRASH
        // --------------------------------------------------------------------
        else
        {
            if (car_state >= 0) car_state = CAR_NORMAL; // No crash - Clear smoke from wheels

            // check_time_expired:
            // 631E: Clear acceleration value if time out
            if ((ostats.time_counter & 0xFF) == 0)
                oinputs.acc_adjust = 0;

            // --------------------------------------------------------------------
            // Do Car Acceleration / Revs / Torque
            // Note: Torque gets set based on gear car is in
            // --------------------------------------------------------------------

            // do_acceleration:
            car_acc_brake();

            int32_t d2 = revs / torque;

            if (!oinitengine.ingame_engine)
            {
                tick_engine_disabled(d2);
            }
            else
            {
                int16_t d1 = torque_index;

                if (gear_counter == 0)
                    do_gear_torque(d1);
            }

            // set_torque:
            int16_t new_torque = torque_lookup[torque_index];
            torque = new_torque;
            d2 = (int32_t) (d2 & 0xFFFF) * new_torque; // unsigned multiply
            int32_t accel_copy = (int32_t)accel_value << 16;
            int32_t rev_adjust_new = 0;

            if (gear_counter != 0)
                rev_adjust_new = tick_gear_change(d2 >> 16);

            // Compare Accelerator To Proposed New Speed
            //
            // d0 = Desired Accelerator Value [accel_copy]
            // d1 = Torque Value [new_torque]
            // d2 = Proposed New Rev Value [d2]
            // d4 = Rev Adjustment (due to braking / off road etc / revs being higher than desired) [rev_adjust]
            else if (d2 != accel_copy)
            {
                if (accel_copy >= d2)
                    rev_adjust_new = get_speed_inc_value(new_torque, d2);
                else
                    rev_adjust_new = get_speed_dec_value(new_torque);
            }

            // test_smoke:
            if (gear_smoke)
                rev_adjust_new = tick_smoke(); // Note this also changes the speed [stored in d4]

            // cont2:
            set_brake_subtract();               // Calculate brake value to subtract from revs
            finalise_revs(d2, rev_adjust_new);  // Subtract calculated value from revs
            convert_revs_speed(new_torque, d2); // d2 is converted from revs to speed

            // Ingame Control Not Active. Clear Car Speed
            if (!oinitengine.ingame_engine)
            {
                oinitengine.car_increment = 0;
                car_inc_old = 0;
            }
            // Set New Car Speed to car_increment
            else if (outrun.game_state != GS_BONUS)
            {
                int16_t diff = car_inc_old - (d2 >> 16); // Old Speed - New Speed

                // Car is moving
                if (diff != 0)
                {
                    // Car Speeding Up (New Speed is faster)
                    if (diff < 0)
                    {
                        diff = -diff;
                        uint8_t adjust = 2;
                        if (oinitengine.car_increment >> 16 <= 0x28)
                            adjust >>= 1;
                        if (diff > 2)
                            d2 = ((int32_t)car_inc_old + adjust) << 16;
                    }
                    // Car Slowing Down
                    else if (diff > 0)
                    {
                        uint8_t adjust = 2;
                        if (brake_subtract)
                            adjust <<= 2;
                        if (diff > adjust)
                            d2 = ((int32_t)car_inc_old - adjust) << 16;
                    }
                }
                oinitengine.car_increment = d2;
            }
            else
                oinitengine.car_increment = d2;
        } // end crash if/else block

        // move_car_rev:
        update_road_pos();
        ohud.draw_rev_counter();
    } // end car_ctrl_active

    // Check whether we want to play slip sound
    // check_slip
    if (gfx_smoke)
    {
        car_state = CAR_SMOKE; // Set smoke from car wheels
        if (oinitengine.car_increment >> 16)
        {
            if (slip_sound == sound::STOP_SLIP)
                osoundint.queue_sound(slip_sound = sound::INIT_SLIP);
        }
        else
            osoundint.queue_sound(slip_sound = sound::STOP_SLIP);
    }
    // no_smoke:
    else
    {
        if (slip_sound != sound::STOP_SLIP)
            osoundint.queue_sound(slip_sound = sound::STOP_SLIP);
    }
    // move_car
    car_inc_old = oinitengine.car_increment >> 16;
    counter++;

    // During Countdown: Clear Car Speed
    if (outrun.game_state == GS_START1 || outrun.game_state == GS_START2 || outrun.game_state == GS_START3)
    {
        oinitengine.car_increment = 0;
        car_inc_old = 0;
    }

}
void OFerrari::car_acc_brake()
{
    // ------------------------------------------------------------------------
    // Acceleration
    // ------------------------------------------------------------------------
    int16_t acc1 = oinputs.acc_adjust;
    int16_t acc2 = oinputs.acc_adjust;

    acc1 += acc_adjust1 + acc_adjust2 + acc_adjust3;
    acc1 >>= 2;
    acc_adjust3 = acc_adjust2;
    acc_adjust2 = acc_adjust1;
    acc_adjust1 = acc2;

    if (!oinitengine.ingame_engine)
    {
        acc2 -= accel_value_bak;
        if (acc2 < 0) acc2 = -acc2;
        if (acc2 < 8)
            acc1 = accel_value_bak;
    }
    // test_skid_spin:

    // Clear acceleration on skid
    if (ocrash.spin_control1 > 0 || ocrash.skid_counter)
        acc1 = 0;

    // Adjust speed when offroad
    else if (wheel_state != WHEELS_ON)
    {
        if (!config.engine.offroad)
        {
            if (gear_value)
                acc1 = ((int32_t)acc1 * 3) / 10;
            else
                acc1 = ((int32_t)acc1 * 6) / 10;

            // If only one wheel off road, increase acceleration by a bit more than if both wheels off-road
            if (wheel_state != WHEELS_OFF)
                acc1 = ((int32_t)acc1 << 1) + (acc1 >> 1);
        }
    }

    // finalise_acc_value:
    accel_value = acc1;
    accel_value_bak = acc1;

    // ------------------------------------------------------------------------
    // Brake
    // ------------------------------------------------------------------------
    int16_t brake1 = oinputs.brake_adjust;
    int16_t brake2 = oinputs.brake_adjust;
    brake1 += brake_adjust1 + brake_adjust2 + brake_adjust3;
    brake1 >>= 2;
    brake_adjust3 = brake_adjust2;
    brake_adjust2 = brake_adjust1;
    brake_adjust1 = brake2;
    brake_value = brake1;

    // ------------------------------------------------------------------------
    // Gears
    // ------------------------------------------------------------------------
    gear_bak = gear_value;
    gear_value = oinputs.gear;
}
void OFerrari::reset_car()
{
    rev_shift            = 1;  // Set normal rev shift value
    ocrash.spin_control2 = 0;
    revs                 = 0;
    oinitengine.car_increment = 0;
    gear_value           = 0;
    gear_bak             = 0;
    rev_adjust           = 0;
    car_inc_old          = 0;
    torque               = 0x1000;
    torque_index         = 0x1F;
    rev_stop_flag        = 0;
    oinitengine.ingame_engine = false;
    oinitengine.ingame_counter = 0x1E; // Set ingame counter (time until we hand control to user)
    slip_sound           = sound::STOP_SLIP;
    acc_adjust1          =
    acc_adjust2          =
    acc_adjust3          = 0;
    brake_adjust1        =
    brake_adjust2        =
    brake_adjust3        = 0;
    auto_brake           = false;
    counter              = 0;
    is_slipping          = 0;    // Denote not slipping/skidding
}
void OFerrari::tick_engine_disabled(int32_t &d2)
{
    torque_index = 0;

    // Crash taking place - do counter and set game engine when expired
    if (ocrash.coll_count1)
    {
        olevelobjs.spray_counter = 0;
        if (--oinitengine.ingame_counter != 0)
            return;
    }
    else if (outrun.game_state != GS_ATTRACT && outrun.game_state != GS_INGAME)
        return;

    // Switch back to in-game engine mode
    oinitengine.ingame_engine = true;

    torque = 0x1000;

    // Use top word of revs for lookup
    int16_t lookup = (revs >> 16);
    if (lookup > 0xFF) lookup = 0xFF;

    torque_index = (0x30 - rev_inc_lookup[lookup]) >> 2;

    int16_t acc = accel_value - 0x10;
    if (acc < 0) acc = 0;
    acc_post_stop = acc;
    revs_post_stop = lookup;

    // Clear bottom word of d2 and swap
    d2 = d2 << 16;

    //lookup -= 0x10;
    //if (lookup < 0) lookup = 0;
    rev_stop_flag = 14;
}
void OFerrari::do_gear_torque(int16_t &d1)
{
    if (oinitengine.ingame_engine)
    {
        d1 = torque_index;
        if (gear_value)
            do_gear_high(d1);
        else
            do_gear_low(d1);
    }
    torque_index = (uint8_t) d1;
    // Backup gear value for next iteration (so we can tell when gear has recently changed)
    gear_bak = gear_value;
}
void OFerrari::do_gear_low(int16_t &d1)
{
    // Recent Shift from high to low
    if (gear_bak)
    {
        gear_value = false;
        gear_counter = 4;
        return;
    }

    // Low Gear - Show Smoke When Accelerating From Standstill
    if (oinitengine.car_increment >> 16 < 0x50 && accel_value - 0xE0 >= 0)
        gfx_smoke++;

    // Adjust Torque Index
    if (d1 == 0x10) return;
    else if (d1 < 0x10)
    {
        d1++;
        return;
    }
    d1 -= 4;
    if (d1 < 0x10)
        d1 = 0x10;
}
void OFerrari::do_gear_high(int16_t &d1)
{
    // Change from Low Gear to High Gear
    if (!gear_bak)
    {
        gear_value = true;
        gear_counter = 4;
        return;
    }

    // Increment torque until it reaches 0x1F
    if (d1 == 0x1f) return;
    d1++;
}
int32_t OFerrari::tick_gear_change(int16_t rem)
{
    gear_counter--;
    rev_adjust = rev_adjust - (rev_adjust >> 4);

    // Setup smoke when gear counter hits zero
    if (gear_counter == 0)
    {
        rem -= 0xE0;
        if (rem < 0)  return rev_adjust;
        int16_t acc = accel_value - 0xE0;
        if (acc < 0)  return rev_adjust;
        gear_smoke = acc;
    }

    return rev_adjust;
}
int32_t OFerrari::get_speed_inc_value(uint16_t new_torque, uint32_t new_rev)
{
    // Use Top Word Of Revs For Table Lookup. Cap At 0xFF Max
    uint16_t lookup = (new_rev >> 16);
    if (lookup > 0xFF) lookup = 0xFF;

    uint32_t rev_adjust = rev_inc_lookup[lookup]; // d4

    // Double adjustment if car moving slowly
    if (oinitengine.car_increment >> 16 <= 0x14)
        rev_adjust <<= 1;

    rev_adjust = (((uint32_t)new_torque * new_torque) >> 12) * rev_adjust;

    if (!oinitengine.ingame_engine) return rev_adjust;
    return rev_adjust << rev_shift;
}
int32_t OFerrari::get_speed_dec_value(uint16_t new_torque)
{
    int32_t new_rev = -(((uint32_t)0x440 * new_torque) >> 4);
    if (wheel_state == WHEELS_ON) return new_rev;
    return new_rev << 2;
}
void OFerrari::set_brake_subtract()
{
    int32_t d6 = 0;
    const int32_t DEC = -0x8800L; // Base value to subtract from acceleration burst

    // Not skidding or spinning
    if (ocrash.skid_counter == 0 && ocrash.spin_control1 == 0)
    {
        if (brake_value < OInputs::BRAKE_THRESHOLD1)
        {
            brake_subtract = d6;
            return;
        }
        else if (brake_value < OInputs::BRAKE_THRESHOLD2)
        {
            brake_subtract = d6 + DEC;
            return;
        }
        else if (brake_value < OInputs::BRAKE_THRESHOLD3)
        {
            brake_subtract = d6 + (DEC * 3);
            return;
        }
        else if (brake_value < OInputs::BRAKE_THRESHOLD4)
        {
            brake_subtract = d6 + (DEC * 5);
            return;
        }
    }
    // check_smoke
    if (oinitengine.car_increment >> 16 > 0x28)
        gfx_smoke++;

    brake_subtract = d6 + (DEC * 9);
}
void OFerrari::finalise_revs(int32_t &d2, int32_t rev_adjust_new)
{
    rev_adjust_new += brake_subtract;
    if (rev_adjust_new < -0x44000) rev_adjust_new = -0x44000;
    d2 += rev_adjust_new;
    rev_adjust = rev_adjust_new;

    if (d2 > 0x13C0000) d2 = 0x13C0000;
    else if (d2 < 0) d2 = 0;
}
void OFerrari::convert_revs_speed(int32_t new_torque, int32_t &d2)
{
    revs = d2;
    int32_t d3 = d2;
    if (d3 < 0x1F0000) d3 = 0x1F0000;

    int16_t revs_top = d3 >> 16;

    // Check whether we're switching back to ingame engine (after disabling user control of car)
    if (rev_stop_flag)
    {
        if (revs_top >= revs_post_stop)
        {
            rev_stop_flag = 0;
        }
        else
        {
            if (accel_value < acc_post_stop)
                revs_post_stop -= rev_stop_flag;

            // cont1:
            int16_t d5 = revs_post_stop >> 1;
            int16_t d4 = rev_stop_flag;
            if (revs_top >= d5)
            {
                d5 >>= 1;
                d4 >>= 1;
                if (revs_top >= d5)
                    d4 >>= 1;
            }
            // 689c
            revs_post_stop -= d4;
            if (revs_post_stop < 0x1F) revs_post_stop = 0x1F;
            revs_top = revs_post_stop;
        }
    }

    // setup_pitch_fx:
    rev_pitch1 = (revs_top * 0x1A90) >> 8;

    // Setup New Car Increment Speed
    d2 = ((d2 >> 16) * 0x1A90) >> 8;
    d2 = (d2 << 16) >> 4;

    /*if (!new_torque)
    {
        std::cout << "convert_revs_speed error!" << std::endl;
    }*/

    const int32_t max_speed = !config.engine.turbo ? MAX_SPEED : (int) (MAX_SPEED * 1.2f);
    d2 = (d2 / new_torque) * 0x480;
    //std::cout << std::hex << "d2: " << (int)d2 << " new_torque: " << new_torque << " torque index: " << std::hex << (int) torque_index << std::endl;
    if (d2 < 0) d2 = 0;
    else if (d2 > max_speed) d2 = max_speed;
}
void OFerrari::update_road_pos()
{
    uint32_t car_inc = CAR_BASE_INC;

    // Bendy Roads
    if (oinitengine.road_type > OInitEngine::ROAD_STRAIGHT)
    {
        int32_t x = oinitengine.car_x_pos;

        if (oinitengine.road_type == OInitEngine::ROAD_RIGHT)
            x = -x;

        x = (x / 0x28) + oinitengine.road_curve;

        car_inc = (car_inc * x) / oinitengine.road_curve;
    }

    car_inc *= oinitengine.car_increment >> 16;
    oroad.road_pos_change = car_inc;
    oroad.road_pos += car_inc;
}
void OFerrari::set_curve_adjust()
{
    int16_t x_diff = oroad.road_x[170] - oroad.road_x[511];

    // Invert x diff when taking roadsplit
    if (oinitengine.rd_split_state && oinitengine.car_x_pos < 0)
        x_diff = -x_diff;

    x_diff >>= 6;

    if (x_diff)
    {
        x_diff *= (oinitengine.car_increment >> 16);
        x_diff /= config.engine.grippy_tyres ? 0xFF : 0xDC;
        x_diff <<= 1;
        oinitengine.car_x_pos += x_diff;
    }
}
void OFerrari::set_ferrari_x()
{
    int16_t steering = oinputs.steering_adjust;

    // Hack to reduce the amount you can steer left and right at the start of Stage 1
    // The amount you can steer increases as you approach road position 0x7F
    if (ostats.cur_stage == 0 && oinitengine.rd_split_state == 0 && (oroad.road_pos >> 16) <= 0x7F)
    {
        steering = (steering * (oroad.road_pos >> 16)) >> 7;
    }

    steering -= steering_old;
    if (steering > 0x40) steering = 0x40;
    else if (steering < -0x40) steering = -0x40;
    steering_old += steering;
    steering = steering_old;

    // This block of code reduces the amount the player
    // can steer left and right, when below a particular speed
    if (wheel_state == OFerrari::WHEELS_ON && (oinitengine.car_increment >> 16) <= 0x7F)
    {
        steering = (steering * (oinitengine.car_increment >> 16)) >> 7;
    }

    // Check Road Curve And Adjust X Value Accordingly
    // This effectively makes it harder to steer into sharp corners
    int16_t road_curve = oinitengine.road_curve;
    if (road_curve)
    {
        road_curve -= 0x40;
        if (road_curve < 0)
        {
            int16_t diff_from_max = (MAX_SPEED >> 17) - (oinitengine.car_increment >> 16);
            if (diff_from_max < 0)
            {
                int16_t curve = (int16_t)((int32_t)diff_from_max * road_curve);
                int32_t result = (int32_t) steering * (0x24C0 - curve);
                steering = result / 0x24C0;
            }
        }
    }

    steering >>= 3;
    int16_t steering2 = steering;
    steering >>= 2;
    steering += steering2;
    steering = -steering;

    // Return if car is not moving
    if (outrun.game_state == GS_INGAME && oinitengine.car_increment >> 16 == 0)
    {
        // Hack so car is centered if we want to bypass countdown sequence
        int16_t road_width_change = (oroad.road_width >> 16) - road_width_old;
        road_width_old = (oroad.road_width >> 16);
        if (oinitengine.car_x_pos < 0)
            road_width_change = -road_width_change;
        oinitengine.car_x_pos += road_width_change;
        // End of Hack
        return;
    }

    oinitengine.car_x_pos += steering;

    int16_t road_width_change = (oroad.road_width >> 16) - road_width_old;
    road_width_old = (oroad.road_width >> 16);
    if (oinitengine.car_x_pos < 0)
        road_width_change = -road_width_change;
    oinitengine.car_x_pos += road_width_change;
}
void OFerrari::do_skid()
{
    if (!ocrash.skid_counter) return;

    if (ocrash.skid_counter > 0)
    {
        ocrash.skid_counter--;
        oinitengine.car_x_pos += OCrash::SKID_X_ADJ;
    }
    else
    {
        ocrash.skid_counter++;
        oinitengine.car_x_pos -= OCrash::SKID_X_ADJ;
    }
}
void OFerrari::set_ferrari_bounds()
{
    // d0 = road_width
    // d2 = car_x_pos

    int16_t road_width16 = oroad.road_width >> 16;
    int16_t d1 = 0;

    // Road Split Both Lanes
    if (oinitengine.rd_split_state == 4)
    {
        if (oinitengine.car_x_pos < 0)
            road_width16 = -road_width16;
        d1 = road_width16 + 0x140;
        road_width16 -= 0x140;
    }
    // One Lane
    else if (road_width16 <= 0xFF)
    {
        road_width16 += OFFROAD_BOUNDS;
        d1 = road_width16;
        road_width16 = -road_width16;
    }
    // Two Lanes - road_two_lanes:
    else
    {
        if (oinitengine.car_x_pos < 0)
            road_width16 = -road_width16;
        d1 = road_width16 + OFFROAD_BOUNDS;
        road_width16 -= OFFROAD_BOUNDS;
    }

    // Set Bounds
    if (oinitengine.car_x_pos < road_width16)
        oinitengine.car_x_pos = road_width16;
    else if (oinitengine.car_x_pos > d1)
        oinitengine.car_x_pos = d1;

    oroad.car_x_bak = oinitengine.car_x_pos;
    oroad.road_width_bak = oroad.road_width >> 16;
}
void OFerrari::check_wheels()
{
    wheel_state = WHEELS_ON;
    wheel_traction = TRACTION_ON;
    uint16_t road_width = oroad.road_width >> 16;

    switch (oroad.road_ctrl)
    {
        case ORoad::ROAD_OFF:         // Both Roads Off
            return;

        // Single Road
        case ORoad::ROAD_R0:          // Road 0
        case ORoad::ROAD_R1:          // Road 1
        case ORoad::ROAD_R0_SPLIT:    // Road 0 (Road Split.)
        case ORoad::ROAD_R1_SPLIT:    // Road 1 (Road Split. Invert Road 1)
        {
            int16_t x = oinitengine.car_x_pos;

            if (oroad.road_ctrl == ORoad::ROAD_R0_SPLIT)
                x -= road_width;
            else
                x += road_width;

            if (oroad.road_ctrl == ORoad::ROAD_R0_SPLIT || oroad.road_ctrl == ORoad::ROAD_R1_SPLIT)
            {
                if (x > -0xD4 && x <= 0xD4) return;
                else if (x < -0x104 || x > 0x104) set_wheels(WHEELS_OFF);
                else if (x > 0xD4 && x <= 0x104) set_wheels(WHEELS_LEFT_OFF);
                else if (x <= 0xD4 && x >= -0x104) set_wheels(WHEELS_RIGHT_OFF);
            }
            else
            {
                if (x > -0xD4 && x <= 0xD4) return;
                else if (x < -0x104 || x > 0x104) set_wheels(WHEELS_OFF);
                else if (x > 0xD4 && x <= 0x104) set_wheels(WHEELS_RIGHT_OFF);
                else if (x <= 0xD4 && x >= -0x104) set_wheels(WHEELS_LEFT_OFF);
            }
        }
        break;

        // Both Roads
        case ORoad::ROAD_BOTH_P0:     // Both Roads (Road 0 Priority) [DEFAULT]
        case ORoad::ROAD_BOTH_P1:     // Both Roads (Road 1 Priority)
        case ORoad::ROAD_BOTH_P0_INV: // Both Roads (Road 0 Priority) (Road Split. Invert Road 1)
        case ORoad::ROAD_BOTH_P1_INV: // Both Roads (Road 1 Priority) (Road Split. Invert Road 1)
        {
            int16_t x = oinitengine.car_x_pos;

            if (road_width > 0xFF)
            {
                if (x < 0)
                    x += road_width;
                else
                    x -= road_width;

                if (x < -0x104 || x > 0x104) set_wheels(WHEELS_OFF);
                else if (x < -0xD4) set_wheels(WHEELS_RIGHT_OFF);
                else if (x > 0xD4) set_wheels(WHEELS_LEFT_OFF);
            }
            else
            {
                road_width += 0x104;

                if (x >= 0)
                {
                    if (x > road_width) set_wheels(WHEELS_OFF);
                    else if (x > road_width - 0x30) set_wheels(WHEELS_LEFT_OFF);
                }
                else
                {
                    x = -x;
                    if (x > road_width) set_wheels(WHEELS_OFF);
                    else if (x > road_width - 0x30) set_wheels(WHEELS_RIGHT_OFF);
                }
            }
        }
        break;
    }
}
void OFerrari::set_wheels(uint8_t new_state)
{
    wheel_state = new_state;
    wheel_traction = (new_state == WHEELS_OFF) ? 2 : 1;
}
int32_t OFerrari::tick_smoke()
{
    gear_smoke--;
    int32_t r = rev_adjust - (rev_adjust >> 4);
    if (gear_smoke >= 8) gfx_smoke++; // Trigger smoke
    return r;
}


/* Exact setup_ferrari_sprite collision-latch excerpt (pinned oferrari.cpp lines 369-380). */
static void source_latch_scenery_collision(){
    oferrari.spr_ferrari->y = 221;
    if (olevelobjs.collision_sprite)
    {
        if (ocrash.coll_count1 == ocrash.coll_count2)
        {
            ocrash.coll_count1++;
            olevelobjs.collision_sprite = 0;
            ocrash.crash_state = 0;
        }
    }
}

// Exact pinned v0.3 OSprites::move_sprite body; lookup is virtualized by verified full-ROM formula.
void OSprites::move_sprite(oentry* sprite, uint8_t shift)
{
    uint32_t addr = SPRITE_ZOOM_LOOKUP + (((sprite->z >> 16) << 2) | sprite_scroll_speed);
    uint32_t value = roms.rom0.read32(addr) >> shift;

    if (config.tick_fps == 60)
        value >>= 1;
    else if (config.tick_fps == 120)
        value >>= 2;

    sprite->z += value;
}

// Exact hash-verified v0.14 outils helpers.
uint32_t outils::bcd_add(uint32_t src, uint32_t dst)
{
    uint8_t carry = 0;
    uint32_t final = 0;

    for (uint8_t i = 0; i < 4; i++)
    {
        int32_t res = (src & 0xF) + (dst & 0xF) + carry;

        if (res > 9)
            res += 6;

        res += (src & 0xF0) + (dst & 0xF0);

        if (res > 0x99)
        {
            res -= 0xA0;
            carry = 1;
        }
        else
        {
            carry = 0;
        }

        src >>= 8;
        dst >>= 8;

        final |= (((uint32_t)res & 0xFFUL) << (i * 8));
    }

    return final;
}

// Exact hash-verified v0.14 outils RNG body.
uint32_t outils::random()
{
	// New seed value
	uint32_t seed = rnd_seed;

	if (seed == 0)
        seed = config.engine.randomgen ? 0x2A6D365A : rand();

	// Random Value To Return
	uint32_t rnd = seed;

	seed <<= 2;
	seed += rnd;
	seed <<= 3;
	seed += rnd;

	move16(seed, rnd);
	swap32(seed);
	add16(seed, rnd);
	move16(rnd, seed);
	swap32(seed);

	rnd_seed = seed; // Set new seed

	return rnd;
}

// Exact pinned body: void OCrash::init(oentry* f, oentry* s, oentry* p1, oentry* p1s, oentry* p2, oentry* p2s)
void OCrash::init(oentry* f, oentry* s, oentry* p1, oentry* p1s, oentry* p2, oentry* p2s)
{
    spr_ferrari = f;
    spr_shadow  = s;
    spr_pass1   = p1;
    spr_pass1s  = p1s;
    spr_pass2   = p2;
    spr_pass2s  = p2s;

    // Setup function pointers for passenger sprites
    function_pass1 = &OCrash::do_crash_passengers;
    function_pass2 = &OCrash::do_crash_passengers;
}

// Exact pinned body: bool OCrash::is_flip()
bool OCrash::is_flip()
{
    return crash_counter && crash_type == CRASH_FLIP;
}

// Exact pinned body: void OCrash::enable()
void OCrash::enable()
{
    // This is called multiple times, so need this check in place
    if (spr_ferrari->control & OSprites::ENABLE)
        return;

    spr_ferrari->control |= OSprites::ENABLE;

    // Reset all corresponding variables
    spinflipcount1 = 0;
    spinflipcount2 = 0;
    slide = 0;
    frame = 0;
    addr = 0;
    camera_x_target = 0;
    camera_xinc = 0;
    lookup_index = 0;
    frame_restore = 0;
    shift = 0;
    crash_speed = 0;
    crash_zinc = 0;
    crash_side = 0;

    spr_ferrari->counter = 0;

    outrun.ttrial.crashes++;
}

// Exact pinned body: void OCrash::clear_crash_state()
void OCrash::clear_crash_state()
{
    spin_control1 = 0;
    coll_count1 = 0;
    coll_count2 = 0;
    olevelobjs.collision_sprite = 0;
    crash_counter = 0;
    crash_state = 0;
    crash_z = 0;
    spin_pass_frame = 0;
    crash_spin_count = 0;
    crash_delay = 0;
    crash_type  = 0;
    skid_counter = 0;
}

// Exact pinned body: void OCrash::tick()
void OCrash::tick()
{
    if (!outrun.tick_frame &&
        oroad.get_view_mode() == ORoad::VIEW_INCAR &&
        crash_type != CRASH_FLIP)
        return;

    // Do Ferrari
    if (spr_ferrari->control & OSprites::ENABLE)
        if (outrun.tick_frame) do_crash();
        else osprites.do_spr_order_shadows(spr_ferrari);

    // Do Car Shadow
    if (spr_shadow->control & OSprites::ENABLE)
        if (outrun.tick_frame) do_shadow(spr_ferrari, spr_shadow);
        else osprites.do_spr_order_shadows(spr_shadow);

    // Do Passenger 1
    if (spr_pass1->control & OSprites::ENABLE)
        if (outrun.tick_frame) ((ocrash).*(function_pass1))(spr_pass1);
        else osprites.do_spr_order_shadows(spr_pass1);

    // Do Passenger 1 Shadow
    if (spr_pass1s->control & OSprites::ENABLE)
        if (outrun.tick_frame) do_shadow(spr_pass1, spr_pass1s);
        else osprites.do_spr_order_shadows(spr_pass1s);

    // Do Passenger 2
    if (spr_pass2->control & OSprites::ENABLE)
        if (outrun.tick_frame) ((ocrash).*(function_pass2))(spr_pass2);
        else osprites.do_spr_order_shadows(spr_pass2);

    // Do Passenger 2 Shadow
    if (spr_pass2s->control & OSprites::ENABLE)
        if (outrun.tick_frame) do_shadow(spr_pass2, spr_pass2s);
        else osprites.do_spr_order_shadows(spr_pass2s);
}

// Exact pinned body: void OCrash::do_crash()
void OCrash::do_crash()
{
    switch (outrun.game_state)
    {
        case GS_INIT_MUSIC:
        case GS_MUSIC:
            end_collision();
            return;

        // Fall through to continue code below
        case GS_ATTRACT:
        case GS_INGAME:
        case GS_BONUS:
            break;

        default:
            // In other modes render crashing ferrari if crash counter is set
            if (crash_counter)
            {
                if (oroad.get_view_mode() != ORoad::VIEW_INCAR || crash_type == CRASH_FLIP)
                    osprites.do_spr_order_shadows(spr_ferrari);
            }
            // Set Distance Into Screen from crash counter
            spr_ferrari->road_priority = spr_ferrari->counter;
            return;
    }

    // ------------------------------------------------------------------------
    // cont1: Adjust steering
    // ------------------------------------------------------------------------
    int16_t steering_adjust = oinputs.steering_adjust;
    oinputs.steering_adjust = 0;

    if (!oferrari.car_ctrl_active)
    {
        if (spin_control1 == 0)
        {
            uint16_t inc = ((oinitengine.car_increment >> 16) * 31) >> 5;
            oinitengine.car_increment = (oinitengine.car_increment & 0xFFFF) | ((uint32_t)inc << 16);
            oferrari.car_inc_old = inc;
        }
        else
        {
            oinputs.steering_adjust = steering_adjust >> 1;
        }
    }

    // ------------------------------------------------------------------------
    // Determine whether to init spin or crash code
    // ------------------------------------------------------------------------
    int16_t spin2_copy = spin_control2;

    // dec_spin2:
    if (spin2_copy != 0)
    {
        spin2_copy -= 2;
        if (spin2_copy < 0)
            spin_control2 = 0;
        else
            spin_switch(spin2_copy);
    }

    // dec_spin1:
    int16_t spin1_copy = spin_control1;

    if (spin1_copy != 0)
    {
        spin1_copy -= 2;
        if (spin1_copy < 0)
            spin_control1 = 0;
        else
            spin_switch(spin1_copy);
    }
    // Not spinning, init crash code
    else
        crash_switch();
}

// Exact pinned body: void OCrash::spin_switch(const uint16_t ctrl)
void OCrash::spin_switch(const uint16_t ctrl)
{
    crash_counter++;
    crash_z = 0;

    switch (ctrl & 3)
    {
        // No Spin - Need to init crash/spin routines
        case 0:
            init_collision();
            break;
        // Init Spin
        case 1:
            do_collision();
            break;
        // Spin In Progress - End Collision Routine
        case 2:
        case 3:
            end_collision();
            break;
    }
}

// Exact pinned body: void OCrash::crash_switch()
void OCrash::crash_switch()
{
    crash_counter++;
    crash_z = 0;

    switch (crash_state & 7)
    {
        // No Crash. Need to setup crash routines.
        case 0:
            init_collision();
            break;
        // Initial Collision
        case 1:
            if ((crash_type & 3) == 0) do_bump();
            else do_collision();
            break;
        // Flip Car
        case 2:
            do_car_flip();
            break;
        // Horizontally Flip Car, Trigger Smoke Cloud
        case 3:
        case 4:
            trigger_smoke();
            break;
        // Do Girl Pointing Finger Animation/Delay Before Pan
        case 5:
            post_flip_anim();
            break;
        // Pan Camera To Track Centre
        case 6:
            pan_camera();
            break;
        // Camera Repositioned. Prepare For Restart.
        case 7:
            end_collision();
            break;
    }
}

// Exact pinned body: void OCrash::init_collision()
void OCrash::init_collision()
{
    oferrari.car_state = OFerrari::CAR_ANIM_SEQ; // Denote car animation sequence

    // Enable crash sprites
    spr_shadow->control |= OSprites::ENABLE;
    spr_pass1->control  |= OSprites::ENABLE;
    spr_pass2->control  |= OSprites::ENABLE;

    // Disable normal sprites
    oferrari.spr_ferrari->control &= ~OSprites::ENABLE;
    oferrari.spr_shadow->control  &= ~OSprites::ENABLE;
    oferrari.spr_pass1->control   &= ~OSprites::ENABLE;
    oferrari.spr_pass2->control   &= ~OSprites::ENABLE;

    spr_ferrari->x = oferrari.spr_ferrari->x;
    spr_ferrari->y = 221;
    spr_ferrari->counter = 0x1FC;
    spr_ferrari->draw_props = oentry::BOTTOM;

    // Collided with another vechicle
    if (spin_control2)
        init_spin2();
    else if (spin_control1)
        init_spin1();
    // Crash into scenery
    else
    {
        skid_counter = 0;
        uint16_t car_inc = oinitengine.car_increment >> 16;
        if (car_inc < 0x64)
            collide_slow();
        else if (car_inc < 0xC8)
            collide_med();
        else
            collide_fast();
    }
}

// Exact pinned body: void OCrash::do_collision()
void OCrash::do_collision()
{
    if (olevelobjs.collision_sprite)
    {
        olevelobjs.collision_sprite = 0;
        if (spin_control1 || spin_control2)
        {
            spin_control2 = 0;
            spin_control1 = 0;
            init_collision(); // Init collision with another sprite
            return;
        }

        // Road generator 1
        if (oinitengine.car_x_pos - (oroad.road_width >> 16) >= 0)
        {
            if (slide < 0)
            {
                slide = -slide;
                oinitengine.car_x_pos -= slide;
                osoundint.queue_sound(sound::CRASH2);
            }
        }
        // Road generator 2
        else
        {
            if (slide >= 0)
            {
                slide = -slide;
                oinitengine.car_x_pos -= slide;
                osoundint.queue_sound(sound::CRASH2);
            }
        }
    }
    // 0x13F8
    uint32_t property_table = addr + (frame << 3);
    crash_z = spr_ferrari->counter;
    spr_ferrari->zoom = 0x80;
    spr_ferrari->priority = 0x1FD;
    oinitengine.car_x_pos -= slide;
    spr_ferrari->addr = roms.rom0p->read32(property_table);

    if (roms.rom0p->read8(4 + property_table))
        spr_ferrari->control |= OSprites::HFLIP;
    else
        spr_ferrari->control &= ~OSprites::HFLIP;

    //spr_ferrari->pal_src = roms.rom0p->read8(5 + property_table);
    spr_ferrari->pal_src = oferrari.ferrari_pal;
    spin_pass_frame = (int8_t) roms.rom0p->read8(6 + property_table);

    if (--spinflipcount2 > 0)
    {
        done(spr_ferrari);
        return;
    }

    spinflipcount2 = crash_spin_count; // Expired: spinflipcount

    if (spinflipcount1)
    {
        frame++;

        // 0x1470
        // Initialize Car Flip
        if (!spin_control1 && !spin_control2 && frame == 2 && crash_type != CRASH_SPIN)
        {
            crash_state = 2; // flip
            addr = outrun.adr.sprite_crash_flip;
            spinflipcount1 = 3; // 3 flips remaining
            spinflipcount2 = crash_spin_count;
            frame = 0;

            // Enable passenger shadows
            spr_pass1s->control |= OSprites::ENABLE;
            spr_pass2s->control |= OSprites::ENABLE;
            done(spr_ferrari);
            return;
        }
        // Do Spin
        else
        {
            if (slide > 0)
                slide -= 2;
            else if (slide < -2)
                slide += 2;

            // End of frame sequence
            if (!roms.rom0p->read8(7 + property_table))
            {
                done(spr_ferrari);
                return;
            }
        }
    }
    // 0x14F4
    frame = 0;

    // Last spin
    if (--spinflipcount1 <= 0)
    {
        osoundint.queue_sound(sound::STOP_SLIP);
        if (spin_control2)
        {
            spin_control2++;
        }
        else if (spin_control1)
        {
            spin_control1++;
        }
        // Init smoke
        else
        {
            crash_state = 4; // trigger smoke
            crash_spin_count = 1;
            spr_ferrari->x += slide; // inc ferrari x based on slide value
        }
    }
    else
        crash_spin_count++;

    done(spr_ferrari);
}

// Exact pinned body: void OCrash::end_collision()
void OCrash::end_collision()
{
    // Enable 'normal' Ferrari object
    oferrari.spr_ferrari->control |= OSprites::ENABLE;
    oferrari.spr_shadow->control  |= OSprites::ENABLE;
    oferrari.spr_pass1->control   |= OSprites::ENABLE;
    oferrari.spr_pass2->control   |= OSprites::ENABLE;

    coll_count2 = coll_count1;
    if (!coll_count2)
        coll_count2 = coll_count1 = 1;

    crash_counter = 0;
    crash_state = 0;
    olevelobjs.collision_sprite = 0;

    oferrari.spr_ferrari->x = 0;
    oferrari.spr_ferrari->y = 221;
    oferrari.car_ctrl_active = true;
    oferrari.car_state = OFerrari::CAR_NORMAL;
    olevelobjs.spray_counter = 0;
    crash_z = 0;

    if (spin_control1)
        oferrari.car_inc_old = oinitengine.car_increment >> 16;
    else
        oferrari.reset_car();

    spin_control2 = 0;
    spin_control1 = 0;

    spr_ferrari->control &= ~OSprites::ENABLE;
    spr_shadow->control  &= ~OSprites::ENABLE;
    spr_pass1->control   &= ~OSprites::ENABLE;
    spr_pass1s->control  &= ~OSprites::ENABLE;
    spr_pass2->control   &= ~OSprites::ENABLE;
    spr_pass2s->control  &= ~OSprites::ENABLE;

    function_pass1 = &OCrash::do_crash_passengers;
    function_pass2 = &OCrash::do_crash_passengers;
    oinputs.crash_input = 0x10; // Set delay in processing steering
}

// Exact pinned body: void OCrash::do_bump()
void OCrash::do_bump()
{
    oferrari.car_ctrl_active = false;   // Disable user control of car
    spr_ferrari->zoom = 0x80;           // Set Entry Number For Zoom Lookup Table
    spr_ferrari->priority = 0x1FD;

    int16_t new_position = (int8_t) roms.rom0.read8(DATA_MOVEMENT + (lookup_index << 3));

    if (new_position)
        crash_z = spr_ferrari->counter;

    spr_ferrari->y = 221 - (new_position >> shift);

    uint32_t frames = addr + (frame << 3);
    spr_ferrari->addr = roms.rom0p->read32(frames);

    if (roms.rom0p->read8(frames + 4))
        spr_ferrari->control |= OSprites::HFLIP;
    else
        spr_ferrari->control &= ~OSprites::HFLIP;

    //spr_ferrari->pal_src = roms.rom0p->read8(frames + 5);
    spr_ferrari->pal_src = oferrari.ferrari_pal;
    spin_pass_frame = (int8_t) roms.rom0p->read8(frames + 6);

    if (++lookup_index >= 0x10)
    {
        addr += (frame_restore << 3);
        spr_ferrari->addr = roms.rom0p->read32(addr);
        spin_pass_frame = (int8_t) roms.rom0p->read8(addr + 6);
        crash_state = 4;      // Trigger smoke cloud
        crash_spin_count = 1; // Denote Crash
    }

    done(spr_ferrari);
}

// Exact pinned body: void OCrash::do_car_flip()
void OCrash::do_car_flip()
{
    // Do this if during the flip, the car has recollided with a new sprite + slow crash (similar to spin_collide)
    if (olevelobjs.collision_sprite && crash_speed == 1)
    {
        uint16_t car_inc16 = oinitengine.car_increment >> 16;

        // Road generator 1
        if (oinitengine.car_x_pos - (oroad.road_width >> 16) >= 0)
        {
            // swap_slide_dir2
            if (slide < 0)
            {
                slide = -slide;
                oinitengine.car_increment = ((uint32_t)(car_inc16 >> 1) << 16) | (oinitengine.car_increment & 0xFFFF);
                osoundint.queue_sound(sound::CRASH2);
                if (oinitengine.car_increment >> 16 > 0x14)
                {
                    int16_t z = spr_ferrari->counter > 0x1FD ? 0x1FD : spr_ferrari->counter; // d3
                    int16_t x_adjust = ((int32_t)0x50 * z) >> 9; // d1
                    if (slide < 0) x_adjust = -x_adjust;
                    oinitengine.car_x_pos -= x_adjust;
                }
            }
            // 0x15F6
            else
                slide += (slide >> 3);
        }
        // Road generator 2
        else
        {
            // swap_slide_dir2:
            if (slide >= 0)
            {
                slide = -slide;
                oinitengine.car_increment = ((uint32_t)(car_inc16 >> 1) << 16) | (oinitengine.car_increment & 0xFFFF);
                osoundint.queue_sound(sound::CRASH2);
                if (oinitengine.car_increment >> 16 > 0x14)
                {
                    int16_t z = spr_ferrari->counter > 0x1FD ? 0x1FD : spr_ferrari->counter; // d3
                    int16_t x_adjust = ((int32_t)0x50 * z) >> 9; // d1
                    if (slide < 0) x_adjust = -x_adjust;
                    oinitengine.car_x_pos -= x_adjust;
                }
            }
            // 0x15F6
            else
                slide += (slide >> 3);
        }
    }

    // flip_cont
    olevelobjs.collision_sprite = 0; // Moved this for clarity
    uint32_t frames = addr + (frame << 3);
    spr_ferrari->addr = roms.rom0p->read32(frames);

    // ------------------------------------------------------------------------
    // Fast Crash: Car Heads towards camera in sky, before vanishing (0x161E)
    // ------------------------------------------------------------------------
    if (crash_speed == 0)
    {
        spr_shadow->control &= ~OSprites::ENABLE; // Disable Shadow
        spr_ferrari->counter += crash_zinc;       // Increment Crash Z
        if (spr_ferrari->counter > 0x3FF)
        {
            spr_ferrari->zoom = 0;
            spr_ferrari->counter = 0;
            init_finger(frames);
            done(spr_ferrari);
            return;
        }
        else
            crash_zinc++;
    }
    // ------------------------------------------------------------------------
    // Slow Crash (0x1648 flip_slower)
    // ------------------------------------------------------------------------
    else
    {
        spr_ferrari->counter -= crash_zinc;       // Decrement Crash Z
        if (crash_zinc > 2)
            crash_zinc--;
    }

    // set_crash_z_inc
    // Note that we've set crash_zinc previously now for clarity

    // use ferrari_crash_z to set priority
    spr_ferrari->priority = spr_ferrari->counter > 0x1FD ? 0x1FD : spr_ferrari->counter;

    int16_t x_diff = ((int32_t)slide * spr_ferrari->priority) >> 9;
    oinitengine.car_x_pos -= x_diff;

    int16_t passenger_frame = (int8_t) roms.rom0p->read8(6 + frames);

    // Start of sequence
    if (passenger_frame == 0)
    {
        slide >>= 1;
        osoundint.queue_sound(sound::CRASH2);
    }

    // Set Z during lower frames
    if (passenger_frame <= 0x10 && spr_ferrari->counter <= 0x1FE)
        crash_z = spr_ferrari->counter;

    // 0x16CC
    passenger_frame = ((int32_t)passenger_frame * spr_ferrari->priority) >> 9;

    // Set Ferrari Y
    int16_t y = -(runtime_crash_road_y((uint16_t)spr_ferrari->priority) >> 4) + 223;
    y -= passenger_frame;
    spr_ferrari->y = y;

    // Set Ferrari Zoom from Z
    spr_ferrari->zoom = (spr_ferrari->counter >> 2);
    if (spr_ferrari->zoom < 0x40) spr_ferrari->zoom = 0x40;

    // Set Ferrari H-Flip
    if (crash_side)
        spr_ferrari->control |= OSprites::HFLIP;
    else
        spr_ferrari->control &= ~OSprites::HFLIP;

    // Palette Hack for recoloured cars. Original version was simply: spr_ferrari->pal_src = roms.rom0p->read8(4 + frames);
    if (frame >= 7)
        spr_ferrari->pal_src = oferrari.ferrari_pal;
    else
        spr_ferrari->pal_src = oferrari.ferrari_pal == OFerrari::PAL_RED ? roms.rom0p->read8(4 + frames) : oferrari.ferrari_pal + 4;

    if (--spinflipcount2 > 0)
    {
        done(spr_ferrari);
        return;
    }

    spinflipcount2 = crash_spin_count;

    // 0x1736
    // Advance to next frame in sequence
    if (spinflipcount1)
    {
        frame++;
        // End of frame sequence
        if ((roms.rom0p->read8(7 + frames) & BIT_7) == 0)
        {
            done(spr_ferrari);
            return;
        }
    }

    frame = 0;

    if (--spinflipcount1 > 0)
    {
        crash_spin_count++;
    }
    else
    {
        init_finger(frames);
    }
    done(spr_ferrari);
}

// Exact pinned body: void OCrash::init_finger(uint32_t frames)
void OCrash::init_finger(uint32_t frames)
{
    crash_spin_count = 1;           // Denote Crash has taken place

    // Do Delay whilst girl points finger
    if (crash_type == CRASH_FLIP)
    {
        oferrari.wheel_state = OFerrari::WHEELS_ON;
        oferrari.car_state   = OFerrari::CAR_NORMAL;
        slide = 0;
        addr += frames;
        crash_delay = 30;
        crash_state = 5;
    }
    // Slide Car and Trigger Smoke Cloud
    else
    {
        crash_state = 3;
        frame = 0;
        addr = outrun.adr.sprite_crash_man1;
        crash_spin_count = 4;   // Denote third flip
        spinflipcount2   = 4;
    }
}

// Exact pinned body: void OCrash::trigger_smoke()
void OCrash::trigger_smoke()
{
    crash_z = spr_ferrari->counter;
    int16_t slide_copy = slide;

    if (slide < 0)
        slide++;
    else if (slide > 0)
        slide--;

    // Slide Car
    oinitengine.car_x_pos -= slide_copy;

    spr_ferrari->addr = roms.rom0p->read32(addr);

    // Set Ferrari H-Flip
    if (roms.rom0p->read8(4 + addr))
        spr_ferrari->control |= OSprites::HFLIP;
    else
        spr_ferrari->control &= ~OSprites::HFLIP;

    //spr_ferrari->pal_src =     roms.rom0p->read8(5 + addr);
    spr_ferrari->pal_src = oferrari.ferrari_pal;
    spin_pass_frame = (int8_t) roms.rom0p->read8(6 + addr);

    // Slow Car
    oinitengine.car_increment =
        (oinitengine.car_increment - ((oinitengine.car_increment >> 2) & 0xFFFF0000))
        | (oinitengine.car_increment & 0xFFFF);

    // Car stationary
    if (oinitengine.car_increment >> 16 <= 0)
    {
        oinitengine.car_increment = 0;
        oferrari.wheel_state = OFerrari::WHEELS_ON;
        oferrari.car_state   = OFerrari::CAR_NORMAL;
        slide = 0;
        crash_delay = 30;
        crash_state = 5; // post crash animation delay state
    }

    done(spr_ferrari);
}

// Exact pinned body: void OCrash::post_flip_anim()
void OCrash::post_flip_anim()
{
    oferrari.car_ctrl_active = false;  // Car and road updates disabled
    if (--crash_delay > 0)
    {
        done(spr_ferrari);
        return;
    }

    oferrari.car_ctrl_active = true;
    crash_state = 6; // Denote pan camera to track centre

    int16_t road_width = oroad.road_width >> 16;
    int16_t car_x_pos  = oinitengine.car_x_pos;
    camera_xinc = 8;

    // Double Road
    if (road_width >= 0xD7)
    {
        if (car_x_pos < 0)
            road_width = -road_width;
    }
    else
    {
        road_width = 0;
    }

    camera_x_target = road_width;

    // 1/ Car on road generator 1 (1 road enabled)
    // 2/ Car on road generator 1 (2 roads enabled)
    if (road_width < car_x_pos)
    {
        camera_xinc += (car_x_pos - road_width) >> 6;
        camera_xinc = -camera_xinc;
    }
    else
    {
        camera_xinc += (road_width - car_x_pos) >> 6;
    }

    done(spr_ferrari);
}

// Exact pinned body: void OCrash::pan_camera()
void OCrash::pan_camera()
{
    oferrari.car_ctrl_active = true;

    oinitengine.car_x_pos += camera_xinc;

    int16_t x_diff = ((int32_t)oferrari.car_x_diff * spr_ferrari->counter) >> 9;
    spr_ferrari->x += x_diff;

    // Pan Right
    if (camera_xinc >= 0)
    {
        if (camera_x_target <= oinitengine.car_x_pos)
            crash_state = 7; // Denote camera position and ready for restart
    }
    // Pan Left
    else
    {
        if (camera_x_target >= oinitengine.car_x_pos)
            crash_state = 7;
    }

    done(spr_ferrari);
}

// Exact pinned body: void OCrash::init_spin1()
void OCrash::init_spin1()
{
    osoundint.queue_sound(sound::INIT_SLIP);
    uint16_t car_inc = oinitengine.car_increment >> 16;
    uint16_t spins = 1;
    if (car_inc > 0xB4)
        spins += outils::random() & 1;

    spinflipcount1 = spins;
    crash_spin_count = 2;
    spinflipcount2 = 2;

    slide = ((spins + 1) << 2) + (car_inc > 0xFF) ? 0xFF >> 3 : car_inc >> 3;

    if (skid_counter_bak < 0)
        addr = outrun.adr.sprite_crash_spin1;
    else
    {
        addr = outrun.adr.sprite_crash_spin1;
        slide = -slide;
    }

    spin_control1++;
    frame = 0;
    skid_counter = 0;
    spr_ferrari->road_priority = spr_ferrari->counter;
}

// Exact pinned body: void OCrash::init_spin2()
void OCrash::init_spin2()
{
    osoundint.queue_sound(sound::INIT_SLIP);
    uint16_t car_inc = oinitengine.car_increment >> 16;
    spinflipcount1 = 1;
    crash_spin_count = 2;
    spinflipcount2 = 8;

    slide = (car_inc > 0xFF) ? 0xFF >> 3 : car_inc >> 3;

    if (oinitengine.road_type != OInitEngine::ROAD_RIGHT)
    {
        addr = outrun.adr.sprite_crash_spin1;
    }
    else
    {
        addr = outrun.adr.sprite_crash_spin1;
        slide = -slide;
    }

    spin_control2++;
    frame = 0;
    skid_counter = 0;
    spr_ferrari->road_priority = spr_ferrari->counter;
}

// Exact pinned body: void OCrash::collide_slow()
void OCrash::collide_slow()
{
    osoundint.queue_sound(sound::REBOUND);

    // Setup shift value for car bump, based on current speed, which ultimately determines how much car rises in air
    uint16_t car_inc = oinitengine.car_increment >> 16;

    if (car_inc <= 0x28)
        shift = 6;
    else if (car_inc <= 0x46)
        shift = 5;
    else
        shift = 4;

    lookup_index = 0;

    // Calculate change in road y, so we can determine incline frame for ferrari
    int16_t y = runtime_crash_road_y((uint16_t)(0x3E0 / 2)) - runtime_crash_road_y((uint16_t)(0x3F0 / 2));
    frame_restore = 0;
    if (y >= 0x12) frame_restore++;
    if (y >= 0x13) frame_restore++;

    // Right Hand Side: Increment Frame Entry By 3
    if (oinitengine.car_x_pos < 0)
        addr = outrun.adr.sprite_bump_data2;
    else
        addr = outrun.adr.sprite_bump_data1;

    crash_type = CRASH_BUMP; // low speed bump
    oinitengine.car_increment &= 0xFFFF;

    // set_collision:
    frame = 0;
    crash_state = 1; // collision with object
    spr_ferrari->road_priority = spr_ferrari->counter;
}

// Exact pinned body: void OCrash::collide_med()
void OCrash::collide_med()
{
    osoundint.queue_sound(sound::INIT_SLIP);

    // Set number of spins based on car speed
    uint16_t car_inc = oinitengine.car_increment >> 16;
    spinflipcount1 = car_inc <= 0x96 ? 1 : 2;
    spinflipcount2 = crash_spin_count = 2;
    slide = ((spinflipcount1 + 1) << 2) + ((car_inc > 0xFF ? 0xFF : car_inc) >> 3);

    // Right Hand Side: Increment Frame Entry By 3
    if (oinitengine.car_x_pos < 0)
    {
        addr = outrun.adr.sprite_crash_spin1;
        slide = -slide;
    }
    else
        addr = outrun.adr.sprite_crash_spin1;

    crash_type = CRASH_SPIN;

    // set_collision:
    frame = 0;
    crash_state = 1; // collision with object
    spr_ferrari->road_priority = spr_ferrari->counter;
}

// Exact pinned body: void OCrash::collide_fast()
void OCrash::collide_fast()
{
    osoundint.queue_sound(sound::CRASH1);

    uint16_t car_inc = oinitengine.car_increment >> 16;
    if (car_inc > 0xFA)
    {
        crash_zinc = 1;
        crash_speed = 0;
    }
    else
    {
        crash_zinc = 0x10;
        crash_speed = 1;
    }

    spinflipcount1 = 1;
    spinflipcount2 = crash_spin_count = 2;

    slide = (car_inc > 0xFF ? 0xFF : car_inc) >> 2;
    slide += (slide >> 1);

    if (oinitengine.road_type != OInitEngine::ROAD_STRAIGHT)
    {
        int16_t d2 = (0x78 - (oinitengine.road_curve <= 0x78 ? oinitengine.road_curve : 0x78)) >> 1;

        // collide_fast_curve:
        slide += d2;
        if (oinitengine.road_type == OInitEngine::ROAD_RIGHT)
            slide = -slide;
    }
    else
    {
        if (oinitengine.car_x_pos < 0) slide = -slide; // rhs
    }

    // set_fast_slide:
    if (slide > 0x78)
        slide = 0x78;

    if (oinitengine.car_x_pos < 0)
    {
        addr = outrun.adr.sprite_crash_spin2;
        crash_side = 0; // RHS
    }
    else
    {
        addr = outrun.adr.sprite_crash_spin1;
        crash_side = 1; // LHS
    }

    crash_type = CRASH_FLIP; // Flip

    // set_collision:
    frame = 0;
    crash_state = 1; // collision with object
    spr_ferrari->road_priority = spr_ferrari->counter;
}

// Exact pinned body: void OCrash::done(oentry* sprite)
void OCrash::done(oentry* sprite)
{
    osprites.map_palette(sprite);

    if (oroad.get_view_mode() != ORoad::VIEW_INCAR || crash_type == CRASH_FLIP)
        osprites.do_spr_order_shadows(sprite);

    sprite->road_priority = sprite->counter;
}

// Exact pinned body: void OCrash::do_shadow(oentry* src_sprite, oentry* dst_sprite)
void OCrash::do_shadow(oentry* src_sprite, oentry* dst_sprite)
{
    uint8_t shadow_shift;

    // Ferrari Shadow
    if (src_sprite == spr_ferrari)
    {
        dst_sprite->draw_props = oentry::BOTTOM;
        shadow_shift = 1;
    }
    else
    {
        shadow_shift = 3;
    }

    dst_sprite->x = src_sprite->x;
    dst_sprite->road_priority = src_sprite->road_priority;

    // Get Z from source sprite (stored in counter)
    uint16_t counter = (src_sprite->counter) >> shadow_shift;
    counter = counter - (counter >> 2);
    dst_sprite->zoom = (uint8_t) counter;

    // Set shadow y
    uint16_t offset = src_sprite->counter > 0x1FF ? 0x1FF : src_sprite->counter;
    dst_sprite->y = -(runtime_crash_road_y(offset) >> 4) + 223;

    if (oroad.get_view_mode() != ORoad::VIEW_INCAR || crash_type == CRASH_FLIP)
        osprites.do_spr_order_shadows(dst_sprite);
}

// Exact pinned body: void OCrash::do_crash_passengers(oentry* sprite)
void OCrash::do_crash_passengers(oentry* sprite)
{
    // --------------------------------------------------------------------------------------------
    // Flip car
    // --------------------------------------------------------------------------------------------
    if (crash_state == 2)
    {
        // Update pointer to functions
        if (sprite == spr_pass1)
            function_pass1 = &OCrash::flip_start;
        else if (sprite == spr_pass2)
            function_pass2 = &OCrash::flip_start;

        // Crash Passenger Flip
        crash_pass_flip(sprite);
        return;
    }

    // --------------------------------------------------------------------------------------------
    // Non-Flip
    // --------------------------------------------------------------------------------------------
    if (crash_state < 5)
        crash_pass1(sprite);
    else
        crash_pass2(sprite);

    if (oroad.get_view_mode() != ORoad::VIEW_INCAR || crash_type == CRASH_FLIP)
    {
        osprites.map_palette(sprite);
        osprites.do_spr_order_shadows(sprite);
    }
}

// Exact pinned body: void OCrash::crash_pass1(oentry* sprite)
void OCrash::crash_pass1(oentry* sprite)
{
    uint32_t frames = (sprite == spr_pass1 ? outrun.adr.sprite_crash_man1 : outrun.adr.sprite_crash_girl1) + (spin_pass_frame << 3);

    sprite->addr    = roms.rom0p->read32(frames);
    uint8_t props   = roms.rom0p->read8(4 + frames);
    sprite->pal_src = roms.rom0p->read8(5 + frames);
    sprite->x       = spr_ferrari->x + (int8_t) roms.rom0p->read8(6 + frames);
    sprite->y       = spr_ferrari->y + (int8_t) roms.rom0p->read8(7 + frames);

    // Check H-Flip
    if (props & BIT_7)
        sprite->control |= OSprites::HFLIP;
    else
        sprite->control &= ~OSprites::HFLIP;

    // Test whether we should set priority higher (unused on passenger sprites I think)
    if (props & BIT_0)
        sprite->priority = sprite->road_priority = 0x1FE;
    else
        sprite->priority = sprite->road_priority = 0x1FD;

    sprite->zoom = 0x7E;
}

// Exact pinned body: void OCrash::crash_pass2(oentry* sprite)
void OCrash::crash_pass2(oentry* sprite)
{
    uint32_t frames = (sprite == spr_pass1 ? outrun.adr.sprite_crash_man2 : outrun.adr.sprite_crash_girl2);

    // Use coll_count2 to select one of the three animations that can be played
    // Use crash_delay to toggle between two distinct frames
    frames += ((coll_count2 & 3) << 4) + (crash_delay & 8);

    sprite->addr    = roms.rom0p->read32(frames);
    uint8_t props   = roms.rom0p->read8(4 + frames);
    sprite->pal_src = roms.rom0p->read8(5 + frames);
    sprite->x       = spr_ferrari->x + (int8_t) roms.rom0p->read8(6 + frames);
    sprite->y       = spr_ferrari->y + (int8_t) roms.rom0p->read8(7 + frames);

    // Check H-Flip
    if (props & BIT_7)
        sprite->control |= OSprites::HFLIP;
    else
        sprite->control &= ~OSprites::HFLIP;

    // Test whether we should set priority higher (unused on passenger sprites I think)
    if (props & BIT_0)
        sprite->priority = sprite->road_priority = 0x1FF;
    else
        sprite->priority = sprite->road_priority = 0x1FE;

    sprite->zoom = 0x7E;

    // Man
    if (sprite == spr_pass1)
    {
        const int8_t XY_OFF[] =
        {
            -0xC, -0x1E,
            0x2,  -0x1B,
            0x4,  -0x1A,
            0x5,  -0x1E,
            0x11, -0x1B,
            0x0,  -0x1A,
            -0x1, -0x1B,
            -0xC, -0x1C,
            -0xE, -0x1B,
            -0xE, -0x1C,
            -0xE, -0x1D,
            -0xC, -0x1B,
            -0xC, -0x1C,
            -0xC, -0x1D,
        };

#if !defined(__I86__) && defined(RUNTIME_ASSERT_CRASH_FRAME)
        if (spin_pass_frame < 0 || spin_pass_frame >= 14)
            fprintf(stderr, "bad crash_pass2 frame=%d coll_count2=%d delay=%d addr=%lu fault=%lu\n", (int)spin_pass_frame, (int)coll_count2, (int)crash_delay, (unsigned long)frames, (unsigned long)rom_fault_addr);
        assert(spin_pass_frame >= 0 && spin_pass_frame < 14);
#endif
        sprite->x += XY_OFF[spin_pass_frame << 1];
        sprite->y += XY_OFF[(spin_pass_frame << 1) + 1];
    }
    // Woman
    else
    {
        const int8_t XY_OFF[] =
        {
            0xA,   -0x1A,
            0x0,   -0x1B,
            -0xF,  -0x1B,
            -0x15, -0x1B,
            -0x2,  -0x1E,
            0x7,   -0x1A,
            0x13,  -0x1D,
            0x9,   -0x1B,
            0x3,   -0x1B,
            0x3,   -0x1C,
            0x3,   -0x1D,
            0x7,   -0x1B,
            0x7,   -0x1C,
            0x7,   -0x1D,
        };

#if !defined(__I86__) && defined(RUNTIME_ASSERT_CRASH_FRAME)
        if (spin_pass_frame < 0 || spin_pass_frame >= 14)
            fprintf(stderr, "bad crash_pass2 frame=%d coll_count2=%d delay=%d addr=%lu fault=%lu\n", (int)spin_pass_frame, (int)coll_count2, (int)crash_delay, (unsigned long)frames, (unsigned long)rom_fault_addr);
        assert(spin_pass_frame >= 0 && spin_pass_frame < 14);
#endif
        sprite->x += XY_OFF[spin_pass_frame << 1];
        sprite->y += XY_OFF[(spin_pass_frame << 1) + 1];
    }
}

// Exact pinned body: void OCrash::crash_pass_flip(oentry* sprite)
void OCrash::crash_pass_flip(oentry* sprite)
{
    // Some of these variable names really need refactoring
    sprite->reload        = 0;                      // clear passenger flip control
    sprite->xw1           = 0;
    sprite->x             = spr_ferrari->x;
    sprite->traffic_speed = crash_spin_count;
    sprite->counter       = 0x1FE;                  // sprite zoom

    // Set address of animation sequence based on whether male/female
    sprite->z = sprite == spr_pass1 ? outrun.adr.sprite_crash_flip_m1 : outrun.adr.sprite_crash_flip_g1;

    flip_start(sprite);
}

// Exact pinned body: void OCrash::flip_start(oentry* sprite)
void OCrash::flip_start(oentry* sprite)
{
    if (outrun.game_state != GS_ATTRACT && outrun.game_state != GS_INGAME)
    {
        osprites.do_spr_order_shadows(sprite);
        done(sprite);
        return;
    }

    switch (sprite->reload & 3) // check passenger flip control
    {
        // Flip passengers out of car
        case 0:
            pass_flip(sprite);
            break;

        // Passengers sit up on road after crash
        case 1:
            pass_situp(sprite);
            break;

        // Passengers turn head and look at car (only if camera pan)
        case 2:
        case 3:
            pass_turnhead(sprite);
            break;
    }
}

// Exact pinned body: void OCrash::pass_flip(oentry* sprite)
void OCrash::pass_flip(oentry* sprite)
{
    // Fast crash
    if (crash_speed == 0)
    {
        sprite->counter += (crash_zinc << 2);

        if (sprite->counter > 0x3FF)
        {
            sprite->reload = 1; // // Passenger Control: Passengers sit up on road after crash

            // Disable sprite and shadow
            sprite->control &= ~OSprites::ENABLE;
            if (sprite == spr_pass1)
                spr_pass1s->control &= ~OSprites::ENABLE;
            else
                spr_pass2s->control &= ~OSprites::ENABLE;
            return;
        }
    }
    // Slow crash
    else
    {
        int16_t zinc = crash_zinc >> 2;

        // Adjust the z position of the female more than the man
        if (sprite == spr_pass2)
        {
            zinc += (zinc >> 1);
        }

        sprite->counter -= zinc;
    }

    // set_z_lookup
    int16_t zoom = sprite->counter >> 2;
    if (zoom < 0x40) zoom = 0x40;
    sprite->zoom = (uint8_t) zoom;

    uint32_t frames = sprite->z + (sprite->xw1 << 3);
    sprite->addr = roms.rom0p->read32(frames);

    uint16_t offset = sprite->counter > 0x1FF ? 0x1FF : sprite->counter;
    int16_t y_change = (((int32_t)(int8_t) roms.rom0p->read8(6 + frames)) * offset) >> 9; // d1

    sprite->y = -(runtime_crash_road_y(offset) >> 4) + 223;
    sprite->y -= y_change;

    // 2138

    sprite->priority = offset;
    if (crash_side)
        sprite->control |= OSprites::HFLIP;
    else
        sprite->control &= ~OSprites::HFLIP;

    sprite->pal_src = roms.rom0p->read8(4 + frames);

    // Decrement spin count
    // Increment frame of passengers for first spins
    if (--sprite->traffic_speed <= 0)
    {
        sprite->traffic_speed = crash_spin_count;
        sprite->xw1++; // Increase passenger frame

        // End of animation sequence. Progress to next sequnce of animations.
        if (roms.rom0p->read8(7 + frames) & BIT_7)
        {
            sprite->reload = 1; // Passenger Control: Passengers sit up on road after crash
            sprite->xw1    = 0; // Reset passenger frame

            // Update address of animation sequence to be used
            sprite->z      = sprite == spr_pass1 ? outrun.adr.sprite_crash_flip_m2 : outrun.adr.sprite_crash_flip_g2;
            frames         = sprite->z;

            // Set Frame Delay for this animation sequence from lower bytes
            sprite->traffic_speed = roms.rom0p->read8(7 + frames) & 0x7F;

            done(sprite);
            return;
        }
    }

    // set_passenger_x
    sprite->x =  spr_ferrari->x;
    sprite->x += ((int8_t) roms.rom0p->read8(5 + frames));
    done(sprite);
}

// Exact pinned body: void OCrash::pass_situp(oentry* sprite)
void OCrash::pass_situp(oentry* sprite)
{
    // Update passenger x position
    int16_t x_diff = ((int32_t)oferrari.car_x_diff * sprite->counter) >> 9;
    sprite->x += x_diff;

    uint32_t frames = sprite->z + (sprite->xw1 << 3);
    sprite->addr    = roms.rom0p->read32(frames);
    sprite->pal_src = roms.rom0p->read8(4 + frames);

    // Decrement frame delay counter
    if (--sprite->traffic_speed <= 0)
    {
        sprite->traffic_speed = roms.rom0p->read8(0xF + frames) & 0x7F;

        // End of animation sequence. Progress to next sequnce of animations.
        if (roms.rom0p->read8(7 + frames) & BIT_7)
        {
            sprite->reload = 2; // Passenger Control: Passengers turn head and look at car
        }
        else
        {
            sprite->xw1++; // Increase passenger frame

            // If camera pan: Make passengers turn heads!
            if (crash_state == 6)
                    sprite->reload = 2;
        }
    }
    done(sprite);
}

// Exact pinned body: void OCrash::pass_turnhead(oentry* sprite)
void OCrash::pass_turnhead(oentry* sprite)
{
    // Update passenger x position
    int16_t x_diff = ((int32_t)oferrari.car_x_diff * sprite->counter) >> 9;
    sprite->x += x_diff;

    uint32_t frames = sprite->z + (sprite->xw1 << 3);
    sprite->addr    = roms.rom0p->read32(frames);
    sprite->pal_src = roms.rom0p->read8(4 + frames);

    // End of animation sequence.
    if (roms.rom0p->read8(7 + frames) & BIT_7)
    {
        done(sprite);
        return;
    }

    // Decrement frame delay counter
    if (--sprite->traffic_speed <= 0)
    {
        sprite->traffic_speed = roms.rom0p->read8(0xF + frames) & 0x7F;
        sprite->xw1++; // Increase passenger frame
    }

    done(sprite);
}

// Exact pinned body: void OLevelObjs::sprite_normal(oentry *sprite, uint8_t zoom)
void OLevelObjs::sprite_normal(oentry *sprite, uint8_t zoom)
{
    // Omit collision check if we're already colliding with something
    if (sprite_collision_counter != 0 || (sprite->z >> 16) < 0x1B0)
    {
        set_spr_zoom_priority(sprite, zoom);
        return;
    }
    // Check Collision with sprite

    // First read collision offsets from table
    uint32_t offset_addr = SPRITE_X_OFFS + sprite->type;
    int16_t x1;
    int16_t x2;

    // H-Flip - swap x co-ordinates
    if (sprite->control & OSprites::HFLIP)
    {
        x2 = (int16_t) roms.rom0.read16(&offset_addr);
        x1 = (int16_t) roms.rom0.read16(&offset_addr);
        x2 = -x2;
        x1 = -x1;
    }
    else
    {
        x1 = (int16_t) roms.rom0.read16(&offset_addr);
        x2 = (int16_t) roms.rom0.read16(&offset_addr);
    }

    // If off left hand side or off right hand side of screen
    if (sprite->x + x1 > 0 || sprite->x + x2 < 0)
    {
        set_spr_zoom_priority(sprite, zoom);
        return;
    }

    collision_sprite++; // Denote collision with sprite
    sprite_collision_counter = COLLISION_RESET; // Reset sprite collision counter
    set_spr_zoom_priority(sprite, zoom);
}

// Exact pinned body: void OLevelObjs::set_spr_zoom_priority(oentry *sprite, uint8_t zoom)
void OLevelObjs::set_spr_zoom_priority(oentry *sprite, uint8_t zoom)
{
    osprites.move_sprite(sprite, 0);
    set_spr_zoom_priority_display(sprite, zoom);
}

// Display half of the pinned set_spr_zoom_priority: depends only on sprite->z and the road state.
void OLevelObjs::set_spr_zoom_priority_display(oentry *sprite, uint8_t zoom)
{
    uint16_t z16 = sprite->z >> 16;

    if (z16 < 4) return;
    if (z16 >= 0x200)
    {
        hide_sprite(sprite);
        return;
    }
    sprite->road_priority = z16;
    sprite->priority = z16;
    sprite->zoom = z16 >> zoom;

    // Set Sprite Y Position [SCREEN]
    // 1/ Use Y Offset From Road Position [Screen]
    // 2/ Use Sprite Y World Data if not 0, converted to a screen position [World]

    int32_t road_y = -(oroad.road_y[oroad.road_p0 + z16] >> 4) + 223;

    if (sprite->yw != 0)
    {
        uint32_t yw = (uint32_t)sprite->yw * (uint16_t)z16; // Note the final product is a LONG, not a word here
        outils::swap32(yw);
        outils::sub16(yw, road_y);
    }
    sprite->y = road_y;

    //    Set Sprite X Position [SCREEN]
    //    1/ Use X Offset From Road Position [Screen]
    //    2/ Use Sprite X World Data

    int16_t road_x = oroad.road0_h[z16];
    int16_t xw1 = sprite->xw1;

    if (xw1 >= 0)
    {
        // Bit of a hack here to avoid code duplication
        if (sprite->function_holder >= 4 && sprite->function_holder <= 6)
            xw1 += (oroad.road_width >> 16) << 1;
        else
        {
            if ((sprite->control & OSprites::WIDE_ROAD) == 0)
                xw1 += (oroad.road_width >> 16) << 1;
        }
    }

    int32_t multiply = ((int32_t)xw1 * z16) >> 9; // only used as a 16 bit value so truncate here
    sprite->x = road_x + multiply;

    osprites.do_spr_order_shadows(sprite);
}

// Exact pinned body: void OLevelObjs::hide_sprite(oentry *sprite)
void OLevelObjs::hide_sprite(oentry *sprite)
{
    sprite->z = 0;
    sprite->zoom = 0; // Hide the sprite
    sprite->control &= ~OSprites::ENABLE; // Disable entry in jump table
}

// Exact pinned body: void OTraffic::check_collision(oentry* sprite)
void OTraffic::check_collision(oentry* sprite)
{
    int16_t d0 = 0;

    // Check for collision
    if (sprite->z >> 16 >= 0x1D8)
    {
        int16_t w  = (sprite->width >> 1) + (sprite->width >> 3) + (sprite->width >> 4);
        int16_t x1 = sprite->x - w; // d2
        int16_t x2 = sprite->x + w; // d1

        // Check traffic is directly in front of player's car
        if (x1 < 0 && x2 > 0)
        {
            // Set collision settings from property table
            collision_mask = roms.rom0p->read8(outrun.adr.traffic_props + sprite->type + 5);
            d0 = (sprite->x < 0) ? -OCrash::SKID_RESET : OCrash::SKID_RESET;

            // Enhancement: Bumper enabled, reduce skid distance
            if (config.engine.bumper)
                d0 /= 3;

            d0 += ocrash.skid_counter;

            if (d0 <= OCrash::SKID_MAX && d0 >= -OCrash::SKID_MAX)
                ocrash.skid_counter = d0;

            // Set Ferrari speed based on collision speed
            if (outrun.game_state == GS_ATTRACT || outrun.game_state == GS_INGAME)
            {
                int16_t traffic_speed = sprite->traffic_speed - 80;
                if (traffic_speed < 0) traffic_speed = 0;
                oinitengine.car_increment = ((uint32_t)(uint16_t)traffic_speed << 16) | (oinitengine.car_increment & 0xFFFF);
                oferrari.car_inc_old = traffic_speed;
                d0 = sound::REBOUND; // rebound sound effect
                collision_traffic++; // denote collision with traffic
                outrun.ttrial.vehicle_cols++;
            }
        }
    }

    // try_sound:
    uint8_t traffic_fx_old = sprite->traffic_fx;
    sprite->traffic_fx = d0 & 0xFF;

    // New sound effect triggered
    if (!traffic_fx_old && sprite->traffic_fx)
    {
        osoundint.queue_sound(sprite->traffic_fx);
        // Set all proximity bits on
        if (outils::random() & 1)
            sprite->traffic_proximity = 0xFF;
    }
}

// Exact pinned body: void OTraffic::update_props(oentry* sprite)
void OTraffic::update_props(oentry* sprite)
{
    int32_t z_adjust = (((oinitengine.car_increment >> 16) - sprite->traffic_speed) * (sprite->z >> 16)) << 5;

    if (config.tick_fps == 60)
        z_adjust >>= 1;
    else if (config.tick_fps == 120)
        z_adjust >>= 2;

    sprite->z += z_adjust;

    int16_t z16 = sprite->z >> 16;

    // Disable Traffic Object
    if (z16 <= 0)
    {
        olevelobjs.hide_sprite(sprite);
        return;
    }
    // Overtake Traffic Object
    if (z16 >= 0x200)
    {
        osoundint.queue_sound(sound::RESET);
        if (outrun.game_state == GS_INGAME)
        {
            // Update score on overtake
            if (outrun.cannonball_mode != Outrun::MODE_TTRIAL)
                ostats.update_score(0x20000);
            else
            {
                ohud.draw_score(ohud.translate(3, 2), outils::convert16_dechex(++outrun.ttrial.overtakes), 2);
                ohud.blit_text1(2, 1, HUD_SCORE1);
                ohud.blit_text1(2, 2, HUD_SCORE2);
            }
        }

        olevelobjs.hide_sprite(sprite);
        return;
    }

    sprite->priority = sprite->road_priority = z16;
    sprite->y = -(oroad.road_y[oroad.road_p0 + z16] >> 4) + 223;
    int16_t* road_x = (sprite->control & OSprites::TRAFFIC_RHS) ? oroad.road1_h : oroad.road0_h;
    int32_t x = ((int32_t)sprite->xw1 * z16) >> 9;
    sprite->x = x + road_x[z16];
    traffic_speed_total += sprite->traffic_speed;
}

// Exact pinned body: void OTraffic::set_zoom_lookup(oentry* sprite)
void OTraffic::set_zoom_lookup(oentry* sprite)
{
    uint16_t road_priority = (sprite->road_priority >> 2) + 4;
    if (road_priority > 0x7F)
        road_priority = 0x7F;

    // Traffic Properties Table
    //
    // +0 [Long] Sprite data address
    // +4 [Byte] Palette
    // +5 [Byte] Collision Mask. Probably to do with the strength/impact of the collision
    // +6 [Byte] Zoom Lookup Value for Width/Height
    // +7 [Byte] Traffic Type

    uint8_t zoom_lookup = roms.rom0p->read8(outrun.adr.traffic_props + sprite->type + 6);

    switch (zoom_lookup)
    {
        case 0:
            road_priority += (road_priority >> 3);
            break;
        case 2:
            road_priority += (road_priority >> 2);
            break;
        case 4:
            road_priority += (road_priority >> 1);
            break;
        case 6:
            road_priority += road_priority;
            break;
    }

    sprite->zoom = (uint8_t) road_priority;
}

// Exact pinned body: void OStats::convert_speed_score(uint16_t speed)
void OStats::convert_speed_score(uint16_t speed)
{
    // 0x960 is the last value in this table to be actively used
    static const uint16_t CONVERT[] =
    {
        0x0,   0x10,  0x20,  0x30,  0x40,  0x50,  0x60,  0x80,  0x110, 0x150,
        0x200, 0x260, 0x330, 0x410, 0x500, 0x600, 0x710, 0x830, 0x960, 0x1100,
        0x1250,
    };

    uint16_t score = CONVERT[(speed >> 4)];
    update_score(score);
}

// Exact pinned body: void OStats::update_score(uint32_t value)
void OStats::update_score(uint32_t value)
{
    if (outrun.cannonball_mode == Outrun::MODE_TTRIAL)
        return;

    score = outils::bcd_add(value, score);

    if (score > 0x99999999)
        score = 0x99999999;

    ohud.draw_score_ingame(score);
}

// Exact pinned body: void OInitEngine::init_crash_bonus()
void OInitEngine::init_crash_bonus()
{
    if (outrun.game_state == GS_MUSIC) return;

    if (ocrash.skid_counter > 6 || ocrash.skid_counter < -6)
    {
        // do_skid:
        if (otraffic.collision_traffic == 1)
        {
            otraffic.collision_traffic = 2;
            uint8_t rnd = outils::random() & otraffic.collision_mask;
            if (rnd == otraffic.collision_mask)
            {
                // Try to launch crash code and perform a spin
                if (ocrash.coll_count1 == ocrash.coll_count2)
                {
                    if (!ocrash.spin_control1)
                    {
                        ocrash.spin_control1 = 1;
                        ocrash.skid_counter_bak = ocrash.skid_counter;
                        test_bonus_mode(true); // 9924 fall through
                        return;
                    }
                    test_bonus_mode(false); // finalise skid
                    return;
                }
                else
                {
                    test_bonus_mode(true); // test_bonus_mode
                    return;
                }
            }
        }
    }
    else if (ocrash.spin_control2 == 1)
    {
        // 9894
        ocrash.spin_control2 = 2;
        if (ocrash.coll_count1 == ocrash.coll_count2)
        {
            ocrash.enable();
        }
        test_bonus_mode(false); // finalise skid
        return;
    }
    else if (ocrash.spin_control1 == 1)
    {
        // 98c0
        ocrash.spin_control1 = 2;
        ocrash.enable();
        test_bonus_mode(false); // finalise skid
        return;
    }

    // 0x9924: Section Of Code
    if (ocrash.coll_count1 == ocrash.coll_count2)
    {
        test_bonus_mode(true);  // test_bonus_mode
    }
    else
    {
        ocrash.enable();
        test_bonus_mode(false); // finalise skid
    }
}

// Exact pinned body: void OInitEngine::test_bonus_mode(bool do_bonus_check)
void OInitEngine::test_bonus_mode(bool do_bonus_check)
{
    // Bonus checking code
    if (do_bonus_check && obonus.bonus_control)
    {
        // Do Bonus Text Display
        if (outrun.cannonball_mode != Outrun::MODE_TTRIAL && obonus.bonus_state < 3)
            obonus.do_bonus_text();

        // End Seq Animation Stage #0
        if (obonus.bonus_control == OBonus::BONUS_SEQ0)
        {
            if (outrun.cannonball_mode == Outrun::MODE_TTRIAL)
                outrun.game_state = GS_INIT_GAMEOVER;
            else
                oanimseq.init_end_seq();
        }
    }

   // finalise_skid:
   if (!ocrash.skid_counter)
       otraffic.collision_traffic = 0;
}

// Exact pinned body: void OInitEngine::set_granular_position()
void OInitEngine::set_granular_position()
{
    uint16_t car_inc16 = car_increment >> 16;

    uint16_t result = car_inc16 / 0x40;
    uint16_t rem = car_inc16 % 0x40;

    granular_rem += rem;
    // When the overall counter overflows past 0x40, we must carry a 1 to the unsigned divide :)
    if (granular_rem >= 0x40)
    {
        granular_rem -= 0x40;
        result++;
    }
    oroad.pos_fine += result;
}

// Exact pinned body: void OInitEngine::set_fine_position()
void OInitEngine::set_fine_position()
{
    uint16_t d0 = oroad.pos_fine - pos_fine_old;
    if (d0 > 0xF)
        d0 = 0xF;

    d0 <<= 0xB;
    osprites.sprite_scroll_speed = d0;

    pos_fine_old = oroad.pos_fine;
}


void OLevelObjs::setup_sprite_routine(oentry* sprite)
{
    switch (sprite->function_holder)
    {
        // Normal Sprite: (Possible With/Without Collision)
        case 0:
            sprite->shadow = 7;
            sprite->draw_props |= 8; // x = anchor centre, y = anchor bottom
            break;

        case 1: // Grass Sprite
        case 11:  // Stone Strips
            sprite->shadow = 7;
            if (sprite->control & OSprites::HFLIP)
                sprite->draw_props |= 2;
            else
                sprite->draw_props |= 1;
            break;

        // Overhead Clouds
        case 2:
            sprite->shadow = 3;
            if (sprite->control & OSprites::HFLIP)
                sprite->draw_props |= 0xA;
            else
                sprite->draw_props |= 9;

            if (sprite->draw_props & BIT_0)
                sprite->reload = -0x20;
            else
                sprite->reload = 0x20;

            sprite->xw2 = 0;
            break;

        // Water Sprite
        case 3:
            sprite->shadow = 3;
            if (sprite->control & OSprites::HFLIP)
                sprite->draw_props |= 2; // anchor x right
            else
                sprite->draw_props |= 1; // anchor x left
            break;

        case 4: // Sprite Lights
        case 5: // Checkpoint Bottom
        case 6: // Checkpoint Top
        case 8: // No Collision Check
        case 9: // Wide Rocks (Stage 2)
            sprite->shadow = 7;
            sprite->draw_props |= 8; // x = anchor centre, y = anchor bottom
            break;

        // Draw From Top Left Collision Check
        case 7:
            sprite->shadow = 7;
            if (sprite->control & OSprites::HFLIP)
                sprite->draw_props |= 9;
            else
                sprite->draw_props |= 0xA;
            break;

        // Sand Strips
        case 10:
        case 14: // version for wider road widths
            sprite->shadow = 3;
            if (sprite->control & OSprites::HFLIP)
                sprite->draw_props |= 2;
            else
                sprite->draw_props |= 1;
            break;

        // Mini Tree
        case 12:
            sprite->shadow = 7;
            if (sprite->control & OSprites::HFLIP)
                sprite->draw_props |= 0xA;
            else
                sprite->draw_props |= 9;
            break;

        // Spray
        case 13:
            sprite->shadow = 3;
            sprite->draw_props |= 0;
            break;
    }
}

void OLevelObjs::sprite_lights(oentry *sprite)
{
    // Omit collision check if we're already colliding with something
    if (sprite_collision_counter != 0 || (sprite->z >> 16) < 0x1B0)
    {
        sprite_lights_countdown(sprite);
        return;
    }
    // Check Collision with sprite

    // First read collision offsets from table
    uint32_t offset_addr = SPRITE_X_OFFS + sprite->type;
    int16_t x1;
    int16_t x2;

    // H-Flip - swap x co-ordinates
    if (sprite->control & OSprites::HFLIP)
    {
        x2 = (int16_t) roms.rom0.read16(&offset_addr);
        x1 = (int16_t) roms.rom0.read16(&offset_addr);
        x2 = -x2;
        x1 = -x1;
    }
    else
    {
        x1 = (int16_t) roms.rom0.read16(&offset_addr);
        x2 = (int16_t) roms.rom0.read16(&offset_addr);
    }

    // If off left hand side or off right hand side of screen
    if (sprite->x + x1 > 0 || sprite->x + x2 < 0)
    {
        sprite_lights_countdown(sprite);
        return;
    }

    collision_sprite++; // Denote collision with sprite
    sprite_collision_counter = COLLISION_RESET; // Reset sprite collision counter
    sprite_lights_countdown(sprite);
}

void OLevelObjs::sprite_lights_countdown(oentry *sprite)
{
    // Yes, here we use the game_state to control the countdown palette :-s
    int countdown_pal = outrun.game_state - 9;

    // We're in countdown mode, let's adjust the palette
    if (countdown_pal >= 0 && countdown_pal <= 3)
    {
        countdown_pal += 0x7A + ostats.cur_stage;
        sprite->pal_src = (uint8_t) countdown_pal;
        osprites.map_palette(sprite);
    }

    set_spr_zoom_priority(sprite, 1); // WIDE_ROAD must be set for this to work.
}

void OLevelObjs::sprite_collision_z1c(oentry* sprite)
{
    // Omit collision check if we're already colliding with something
    if (sprite_collision_counter != 0 || (sprite->z >> 16) < 0x1B0)
    {
        set_spr_zoom_priority2(sprite, 1);
        return;
    }
    // Check Collision with sprite

    // First read collision offsets from table
    uint32_t offset_addr = SPRITE_X_OFFS + sprite->type;
    int16_t x1;
    int16_t x2;

    // H-Flip - swap x co-ordinates
    if (sprite-> control & OSprites::HFLIP)
    {
        x2 = (int16_t) roms.rom0.read16(&offset_addr);
        x1 = (int16_t) roms.rom0.read16(&offset_addr);
        x2 = -x2;
        x1 = -x1;
    }
    else
    {
        x1 = (int16_t) roms.rom0.read16(&offset_addr);
        x2 = (int16_t) roms.rom0.read16(&offset_addr);
    }

    int16_t centre = (x2 - x1) >> 1; // d0
    x1 -= centre;
    x2 -= centre;

    // If off left hand side or off right hand side of screen
    if (sprite->x + x1 > 0 || sprite->x + x2 < 0)
    {
        set_spr_zoom_priority2(sprite, 1);
        return;
    }

    collision_sprite++; // Denote collision with sprite
    sprite_collision_counter = COLLISION_RESET; // Reset sprite collision counter
    set_spr_zoom_priority2(sprite, 1);
}

void OLevelObjs::set_spr_zoom_priority2(oentry *sprite, uint8_t zoom)
{
    osprites.move_sprite(sprite, 0);
    uint16_t z16 = sprite->z >> 16;

    if (z16 < 4) return;
    if (z16 >= 0x200)
    {
        hide_sprite(sprite);
        return;
    }
    sprite->road_priority = z16;
    sprite->priority = z16;
    sprite->zoom = z16 >> zoom;

    // Code differs from below

    // Set Sprite X Position [SCREEN]
    // 1/ Use X Offset From Road Position [Screen]
    // 2/ Use Sprite X World Data

    int16_t road_x = oroad.road0_h[z16];
    int16_t xw1 = sprite->xw1;

    if (xw1 >= 0 && (sprite->control & OSprites::WIDE_ROAD) == 0)
    {
        xw1 +=  ((int16_t) (oroad.road_width >> 16)) << 1;
    }

    int32_t multiply = (((int32_t)xw1 * (int32_t)z16) >> 9); // only used as a 16 bit value so truncate here
    road_x += multiply;
    if (road_x > (160 + config.s16_x_off) || road_x < -(160 + config.s16_x_off)) return; // NEW LINE compared with original routine (added for ROM REV. A)
    sprite->x = road_x;

    // Set Sprite Y Position [SCREEN]
    // 1/ Use Y Offset From Road Position [Screen]
    // 2/ Use Sprite Y World Data if not 0, converted to a screen position [World]
    sprite->y = -(oroad.road_y[oroad.road_p0 + z16] >> 4) + 223;

    osprites.do_spr_order_shadows(sprite);
}

void OLevelObjs::sprite_rocks(oentry *sprite)
{
    uint8_t zoom = 1;
    // Omit collision check if we're already colliding with something
    if (sprite_collision_counter != 0 || (sprite->z >> 16) < 0x1B0)
    {
        set_spr_zoom_priority_rocks(sprite, zoom);
        return;
    }
    // Check Collision with sprite

    // First read collision offsets from table
    uint32_t offset_addr = SPRITE_X_OFFS + sprite->type;
    int16_t x1;
    int16_t x2;

    // H-Flip - swap x co-ordinates
    if (sprite->control & OSprites::HFLIP)
    {
        x2 = (int16_t) roms.rom0.read16(&offset_addr);
        x1 = (int16_t) roms.rom0.read16(&offset_addr);
        x2 = -x2;
        x1 = -x1;
    }
    else
    {
        x1 = (int16_t) roms.rom0.read16(&offset_addr);
        x2 = (int16_t) roms.rom0.read16(&offset_addr);
    }

    // If off left hand side or off right hand side of screen
    if (sprite->x + x1 > 0 || sprite->x + x2 < 0)
    {
        set_spr_zoom_priority_rocks(sprite, zoom);
        return;
    }

    collision_sprite++; // Denote collision with sprite
    sprite_collision_counter = COLLISION_RESET; // Reset sprite collision counter
    set_spr_zoom_priority_rocks(sprite, zoom);
}

void OLevelObjs::set_spr_zoom_priority_rocks(oentry *sprite, uint8_t zoom)
{
    osprites.move_sprite(sprite, 0);
    uint16_t z16 = sprite->z >> 16;

    if (z16 < 4) return;
    if (z16 >= 0x200)
    {
        hide_sprite(sprite);
        return;
    }
    sprite->road_priority = z16;
    sprite->priority = z16;
    sprite->zoom = z16 >> zoom;

    // Set Sprite Y Position [SCREEN]
    // 1/ Use Y Offset From Road Position [Screen]
    // 2/ Use Sprite Y World Data if not 0, converted to a screen position [World]

    int32_t road_y = -(oroad.road_y[oroad.road_p0 + z16] >> 4) + 223;
    sprite->y = road_y;

    //    Set Sprite X Position [SCREEN]
    //    1/ Use X Offset From Road Position [Screen]
    //    2/ Use Sprite X World Data

    int16_t road_x = oroad.road0_h[z16];
    int16_t xw1 = sprite->xw1;

    if (xw1 >= 0 && (sprite->control & OSprites::WIDE_ROAD) == 0)
    {
        xw1 +=  ((int16_t) (oroad.road_width >> 16)) << 1;
    }

    int32_t multiply = (((int32_t)xw1 * (int32_t)z16) >> 9); // only used as a 16 bit value so truncate here
    sprite->x = road_x + multiply;

    // Additional Draw Check Here, to prevent sprite from wrapping around screen and redrawing on left
    // Remember this routine is setup to draw from centre x

    uint16_t width = (sprite->width >> 1) + 160 + config.s16_x_off;
    if (sprite->x >= width || sprite->x + width < 0) return;

    osprites.do_spr_order_shadows(sprite);
}

#define RT_MK_FP(seg,off) (((unsigned short)(seg)):>((void __near *)(off)))
#include "geometry_adapter.hpp"
#include "input_bodies.inc"
#pragma pack(1)
struct RuntimeObject {
 uint8_t active,kind,event_slot,zoom; uint16_t event_descriptor,type,width;
 int16_t x,y,traffic_speed; uint8_t traffic_fx,traffic_proximity; uint16_t yw;
 int32_t z;
};
struct RuntimeActor {
 uint32_t addr; int16_t x,y; uint8_t zoom,control; uint16_t palette;
 uint8_t anchor; uint16_t priority,road_priority;
};
struct RuntimeIPC {
 char magic[4]; uint16_t version,command,status,bytes;
 uint16_t held_keys; uint8_t gear_mode,gear_value;
 uint16_t time_left,course,stage,split,phys_seg,geo_seg;
 uint32_t rng_value;
 uint32_t source_increment,source_speed; int16_t source_x,native_x;
 uint32_t source_road_pos,native_distance; uint16_t road_width;
 uint8_t wheel_state,crash_state,crash_counter,crash_type,car_ctrl_active,car_state,spin_control1,spin_control2;
 int16_t collision_count,skid_counter; uint16_t traffic_collision,traffic_mask; int16_t traffic_speed_total;
 uint32_t score_bcd;
 uint16_t object_count; RuntimeObject objects[8]; RuntimeActor actors[6];
};
extern "C" RuntimeIPC runtime_ipc={{'V','R','T','1'},2,0,0,sizeof(RuntimeIPC)};
static native_geo::CurveState runtime_curve;
static oentry runtime_object_state[8];
static uint8_t runtime_object_bound[8];
static uint8_t runtime_object_spawned[8];
#ifndef RUNTIME_LAZY_Z
#define RUNTIME_LAZY_Z 0x1B0
#endif
static uint8_t runtime_object_lazy[8];
/* A scenery step may skip projection+display only if the NEXT step cannot read this sprite's x: sprite_normal and the
 * z1c/lights/rocks routines read sprite->x only when the entry depth is >= 0x1B0, and the entry depth of the next step is
 * this step's post-move depth z1. Routines 4 (lights), 7 (z1c) and 9 (rocks) keep their original per-step path. The
 * display tail (set_spr_zoom_priority_display) is a pure function of z, the road arrays and the road globals. */
static uint8_t runtime_scenery_lazy_ok(const oentry *s,uint16_t z1){
#if defined(RUNTIME_NO_LAZY)
 (void)s;(void)z1;return 0;
#else
 if(z1>=RUNTIME_LAZY_Z)return 0;
#if !defined(RUNTIME_LAZY_ALL)
 if(s->function_holder==4||s->function_holder==7||s->function_holder==9)return 0;
#endif
 return 1;
#endif
}

static uint8_t runtime_scenery_done[64];
// Consumed events form a prefix. Reset only with the done map, not at a split.
static uint16_t runtime_scenery_next=0;
static uint16_t runtime_scenery_total;
static GeometryBytes runtime_phys=0, runtime_geo=0;
static GeometryBytes runtime_desc_key_phys=0;static uint16_t runtime_desc_key_course=0xffff;
static uint16_t runtime_desc_key_id=0xffff;
static uint16_t runtime_desc_cache[64];static uint8_t runtime_desc_valid[64];
// Projection inputs are constant during the post-movement object phase.
// Keep the cache disabled for standalone fixture/helper calls.
static uint8_t runtime_projection_cache_active=0,runtime_projection_key_valid=0;
static uint8_t runtime_projection_depth_seen[64];
static uint16_t runtime_projection_cached_distance=0,runtime_projection_cached_width=0;
static GeometryBytes runtime_projection_key_geo=0;
static uint32_t runtime_projection_key_road_pos=0;
static int32_t runtime_projection_key_road_width=0;
static uint16_t runtime_projection_key_course=0,runtime_projection_key_road_p0=0;
static int16_t runtime_projection_key_car_x=0;
static uint8_t runtime_projection_key_split=0;
#if !defined(__I86__) && defined(RUNTIME_HOST_FIXTURE)
#include <assert.h>
static uint32_t runtime_projection_requests=0,runtime_projection_hits=0,runtime_projection_computes=0,runtime_projection_key_invalidations=0,runtime_projection_phases_started=0;
#endif
#if !defined(__I86__)
static const uint8_t *runtime_host_phys=0,*runtime_host_geo=0;
extern "C" void runtime_set_host_geometry(const uint8_t *p,const uint8_t *g){runtime_host_phys=p;runtime_host_geo=g;runtime_desc_key_phys=0;}
#endif
#ifdef __WATCOMC__
static void runtime_clear(void *p,uint16_t n){uint16_t *w=(uint16_t*)p;uint16_t c=n>>1;while(c--)*w++=0;if(n&1)*(uint8_t*)w=0;}
#else
static void runtime_clear(void *p,uint16_t n){uint8_t *b=(uint8_t*)p;while(n--)*b++=0;}
#endif
static bool runtime_bind_geometry(){
#if defined(__I86__)
 runtime_phys=(GeometryBytes)RT_MK_FP(runtime_ipc.phys_seg,0);
 runtime_geo=(GeometryBytes)RT_MK_FP(runtime_ipc.geo_seg,0);
#else
 runtime_phys=(GeometryBytes)runtime_host_phys;runtime_geo=(GeometryBytes)runtime_host_geo;
#endif
 if(!runtime_phys || !runtime_geo || !native_geo::validate(runtime_phys))return false;
 // Compact PHYS v5 contains one course directory; never interpret it using
 // another course's IPC selection. PHYS v4 retains its 15-course directory.
 return runtime_phys[4]!=5 || native_geo::le16(runtime_phys,28)==runtime_ipc.course;
}
static uint16_t runtime_event_descriptor(uint8_t slot){
 if(!runtime_phys || runtime_ipc.course>=15 || slot>=64)return 0xffff;
 uint16_t id=(uint16_t)(runtime_phys[4]|((uint16_t)runtime_phys[28]<<8));
 if(runtime_desc_key_phys!=runtime_phys || runtime_desc_key_course!=runtime_ipc.course || runtime_desc_key_id!=id){
  runtime_clear(runtime_desc_valid,sizeof(runtime_desc_valid));
  runtime_desc_key_phys=runtime_phys;runtime_desc_key_course=runtime_ipc.course;runtime_desc_key_id=id;}
 if(!runtime_desc_valid[slot]){
  runtime_desc_cache[slot]=native_geo::descriptor_offset(runtime_phys,runtime_ipc.course,slot);runtime_desc_valid[slot]=1;}
 return runtime_desc_cache[slot];
}
static uint32_t runtime_le24(uint16_t off){
 return (uint32_t)runtime_phys[off]|((uint32_t)runtime_phys[off+1]<<8)|((uint32_t)runtime_phys[off+2]<<16);
}
static void runtime_init_globals(){
 runtime_clear(&outrun,sizeof(outrun));runtime_clear(&oinputs,sizeof(oinputs));runtime_clear(&oinitengine,sizeof(oinitengine));
 runtime_clear(&oroad,sizeof(oroad));runtime_clear(&ocrash,sizeof(ocrash));runtime_clear(&olevelobjs,sizeof(olevelobjs));
 runtime_clear(&otraffic,sizeof(otraffic));runtime_clear(&ostats,sizeof(ostats));runtime_clear(&oferrari,sizeof(oferrari));
 runtime_clear(&obonus,sizeof(obonus));runtime_clear(&oanimseq,sizeof(oanimseq));runtime_clear(crash_actors,sizeof(crash_actors));
 for(uint8_t i=0;i<6;i++){oentry &a=crash_actors[i];a.jump_index=(uint8_t)(113+i);a.function_holder=-1;a.shadow=3;}
 crash_actors[1].shadow=7;crash_actors[1].zoom=0x80;crash_actors[1].draw_props=8;crash_actors[1].addr=0x103B6UL;
 crash_actors[2].draw_props=8;crash_actors[4].draw_props=8;
 crash_actors[3].shadow=7;crash_actors[3].addr=0x103B6UL;
 crash_actors[5].shadow=7;crash_actors[5].draw_props=8;crash_actors[5].addr=0x103B6UL;
 oferrari.ferrari_pal=2;
 runtime_clear(normal_actors,sizeof(normal_actors));runtime_clear(runtime_object_state,sizeof(runtime_object_state));
 runtime_clear(runtime_object_bound,sizeof(runtime_object_bound));runtime_clear(runtime_object_lazy,sizeof(runtime_object_lazy));runtime_clear(runtime_object_spawned,sizeof(runtime_object_spawned));runtime_clear(runtime_scenery_done,sizeof(runtime_scenery_done));runtime_scenery_total=0;runtime_scenery_next=0;
 runtime_clear(&osoundint,sizeof(osoundint));
 roms.rom0p=&roms.rom0;rom_fault_addr=0;
 outrun.game_state=GS_INGAME;outrun.cannonball_mode=Outrun::MODE_ORIGINAL;outrun.tick_frame=1;
 outrun.adr.traffic_props=0x4CFA;outrun.adr.traffic_data=0x5424;outrun.adr.sprite_crash_spin1=0x2294;outrun.adr.sprite_crash_spin2=0x22D4;
 outrun.adr.sprite_bump_data1=0x2314;outrun.adr.sprite_bump_data2=0x232C;outrun.adr.sprite_crash_man1=0x2344;
 outrun.adr.sprite_crash_girl1=0x23B4;outrun.adr.sprite_crash_flip=0x2424;outrun.adr.sprite_crash_flip_m1=0x2464;
 outrun.adr.sprite_crash_flip_g1=0x255C;outrun.adr.sprite_crash_flip_m2=0x24DC;outrun.adr.sprite_crash_flip_g2=0x25D4;
 outrun.adr.sprite_crash_man2=0x2604;outrun.adr.sprite_crash_girl2=0x2660;
 config.engine.bumper=false;config.engine.fix_bugs=false;config.engine.offroad=false;config.engine.turbo=false;config.engine.grippy_tyres=false;
 config.engine.randomgen=true;
 config.tick_fps=30;config.controls.gear=runtime_ipc.gear_mode?1:0;
 oinitengine.road_type=OInitEngine::ROAD_STRAIGHT;oinitengine.car_x_pos=450;oinitengine.car_x_old=450;
 oroad.road_width=450L<<16;oroad.road_width_bak=450;oroad.car_x_bak=450;oroad.road_p0=16;oroad.road_ctrl=ORoad::ROAD_BOTH_P0;
 input.held=0;oinputs.steering_inc=3;oinputs.acc_inc=16;oinputs.brake_inc=16;
 oinputs.input_steering=OInputs::STEERING_CENTRE;oinputs.input_acc=0;oinputs.input_brake=0;oinputs.gear=0;
 ostats.cur_stage=(int8_t)runtime_ipc.stage;ostats.time_counter=(int16_t)runtime_ipc.time_left;
 outils::reset_random_seed();
 ocrash.spr_ferrari=&crash_actors[0];ocrash.spr_shadow=&crash_actors[1];ocrash.spr_pass1=&crash_actors[2];
 ocrash.spr_pass1s=&crash_actors[3];ocrash.spr_pass2=&crash_actors[4];ocrash.spr_pass2s=&crash_actors[5];
 oferrari.spr_ferrari=&normal_actors[0];oferrari.spr_shadow=&normal_actors[1];oferrari.spr_pass1=&normal_actors[2];oferrari.spr_pass2=&normal_actors[3];
 normal_actors[0].control=normal_actors[1].control=normal_actors[2].control=normal_actors[3].control=OSprites::ENABLE;
 ocrash.init(ocrash.spr_ferrari,ocrash.spr_shadow,ocrash.spr_pass1,ocrash.spr_pass1s,ocrash.spr_pass2,ocrash.spr_pass2s);
 oferrari.car_state=OFerrari::CAR_NORMAL;oferrari.torque=0x1000;oferrari.reset_car();
 oferrari.car_ctrl_active=true;oferrari.spr_ferrari=&normal_actors[0];oferrari.torque=0x1000;oferrari.road_width_old=450;
 oferrari.torque_index=0x1f;oferrari.wheel_state=OFerrari::WHEELS_ON;oferrari.wheel_traction=OFerrari::TRACTION_ON;
 oinitengine.car_increment=0;oinitengine.ingame_engine=true;oroad.road_pos=0;oroad.road_pos_change=0;
 native_geo::reset_curve(runtime_curve);
}
static void runtime_rebase(uint16_t course,uint16_t stage){
 runtime_desc_key_phys=0;
 uint32_t saved_increment=oinitengine.car_increment;uint16_t saved_inc=oferrari.car_inc_old;int16_t requested_x=runtime_ipc.source_x;
 int16_t saved_torque=oferrari.torque;int32_t saved_revs=oferrari.revs;
 runtime_ipc.course=course;runtime_ipc.stage=stage;runtime_ipc.split=0;ostats.cur_stage=(int8_t)stage;
 int16_t width=(course==0)?450:212;oinitengine.car_x_pos=requested_x;oinitengine.car_x_old=requested_x;
 oroad.car_x_bak=requested_x;oroad.road_width_bak=width;oroad.road_width=(int32_t)width<<16;oroad.road_ctrl=ORoad::ROAD_BOTH_P0;
 oferrari.road_width_old=width;oroad.road_pos=0;oroad.road_pos_change=0;oinitengine.rd_split_state=0;
 runtime_clear(runtime_object_bound,sizeof(runtime_object_bound));runtime_clear(runtime_object_lazy,sizeof(runtime_object_lazy));runtime_clear(runtime_object_spawned,sizeof(runtime_object_spawned));runtime_clear(runtime_scenery_done,sizeof(runtime_scenery_done));runtime_scenery_total=0;runtime_scenery_next=0;
 for(uint8_t i=0;i<8;i++)runtime_ipc.objects[i].active=0;
 oinitengine.car_increment=saved_increment;oferrari.car_inc_old=saved_inc;oferrari.torque=saved_torque;oferrari.revs=saved_revs;
 native_geo::reset_curve(runtime_curve);
}
static void runtime_geometry(){
 if(!runtime_bind_geometry())return;
 uint16_t source=(uint16_t)(oroad.road_pos>>16);
 if(runtime_ipc.course<10 && !runtime_ipc.split && source>1948){
  runtime_ipc.split=1;oroad.road_pos=0;source=0;oinitengine.rd_split_state=2;
  oroad.road_ctrl=ORoad::ROAD_BOTH_P0_INV;native_geo::reset_curve(runtime_curve);
 }
 if(runtime_ipc.split){oinitengine.rd_split_state=(source>=63)?4:2;oroad.road_ctrl=ORoad::ROAD_BOTH_P0_INV;}
 else {oinitengine.rd_split_state=0;oroad.road_ctrl=ORoad::ROAD_BOTH_P0;}
 native_geo::update_curve(runtime_curve,runtime_phys,runtime_ipc.course,source,runtime_ipc.split);
 uint32_t nd=native_geo::native_distance_fixed(oroad.road_pos,runtime_ipc.course,runtime_ipc.split);
 uint16_t distance=(uint16_t)(nd>>16);native_geo::RoadSample s=native_geo::sample(runtime_phys,runtime_geo,runtime_ipc.course,distance);
 oroad.road_width=(int32_t)s.width<<16;oroad.road_width_bak=(int16_t)s.width;
 oroad.road_x[170]=s.road_x170;oroad.road_x[511]=s.road_x511;
 oinitengine.road_curve=runtime_curve.curve;oinitengine.road_type=runtime_curve.type;oinitengine.road_type_next=runtime_curve.next_type;
 }
static void runtime_projection_capture_key(){
 runtime_projection_cached_distance=(uint16_t)(native_geo::native_distance_fixed(oroad.road_pos,runtime_ipc.course,runtime_ipc.split)>>16);
 runtime_projection_cached_width=(uint16_t)(oroad.road_width>>16);
 runtime_projection_key_geo=runtime_geo;runtime_projection_key_road_pos=oroad.road_pos;
 runtime_projection_key_road_width=oroad.road_width;runtime_projection_key_course=runtime_ipc.course;
 runtime_projection_key_road_p0=(uint16_t)oroad.road_p0;runtime_projection_key_car_x=oinitengine.car_x_pos;
 runtime_projection_key_split=(uint8_t)runtime_ipc.split;runtime_projection_key_valid=1;
}
static bool runtime_projection_key_matches(){
 return runtime_projection_key_valid && runtime_projection_key_geo==runtime_geo &&
  runtime_projection_key_road_pos==oroad.road_pos && runtime_projection_key_road_width==oroad.road_width &&
  runtime_projection_key_course==runtime_ipc.course && runtime_projection_key_car_x==oinitengine.car_x_pos &&
  runtime_projection_key_split==(uint8_t)runtime_ipc.split && runtime_projection_key_road_p0==(uint16_t)oroad.road_p0;
}
static void runtime_projection_begin_phase(){
#if !defined(__I86__) && defined(RUNTIME_HOST_FIXTURE)
 ++runtime_projection_phases_started;
#endif
 runtime_clear(runtime_projection_depth_seen,sizeof(runtime_projection_depth_seen));
 runtime_projection_capture_key();runtime_projection_cache_active=1;
}
static void runtime_project_depth(uint16_t z){
 if(z>511 || !runtime_geo)return;
#if !defined(__I86__) && defined(RUNTIME_HOST_FIXTURE)
 ++runtime_projection_requests;
#endif
 uint16_t distance,width;
 if(runtime_projection_cache_active){
#if !defined(__I86__) && defined(RUNTIME_HOST_FIXTURE)
  assert(runtime_projection_key_matches());
#endif
  distance=runtime_projection_cached_distance;width=runtime_projection_cached_width;
  uint8_t mask=(uint8_t)(1U<<(z&7));uint8_t *entry=&runtime_projection_depth_seen[z>>3];
  if(*entry&mask){
#if !defined(__I86__) && defined(RUNTIME_HOST_FIXTURE)
   ++runtime_projection_hits;
   native_geo::ProjectionSample check=native_geo::projection(runtime_geo,distance,z,oinitengine.car_x_pos,width,runtime_ipc.split);
   uint16_t yi=(uint16_t)(oroad.road_p0+z);
   assert(oroad.road_x[z]==check.road_x && oroad.road0_h[z]==check.road0_h &&
          oroad.road1_h[z]==check.road1_h && oroad.road_y[yi]==check.road_y);
#endif
   return;
  }
 }else{
  distance=(uint16_t)(native_geo::native_distance_fixed(oroad.road_pos,runtime_ipc.course,runtime_ipc.split)>>16);
  width=(uint16_t)(oroad.road_width>>16);
 }
 native_geo::ProjectionSample p=native_geo::projection(runtime_geo,distance,z,oinitengine.car_x_pos,width,runtime_ipc.split);
 oroad.road_x[z]=p.road_x;oroad.road0_h[z]=p.road0_h;oroad.road1_h[z]=p.road1_h;
 uint16_t yi=(uint16_t)(oroad.road_p0+z);oroad.road_y[yi]=p.road_y;
 if(runtime_projection_cache_active)runtime_projection_depth_seen[z>>3]|=(uint8_t)(1U<<(z&7));
#if !defined(__I86__) && defined(RUNTIME_HOST_FIXTURE)
 ++runtime_projection_computes;
#endif
}
#if !defined(__I86__) && defined(RUNTIME_HOST_FIXTURE)
static void runtime_fill_crash_road_y_reference(){
 if(!runtime_geo)return;
 uint32_t nd=native_geo::native_distance_fixed(oroad.road_pos,runtime_ipc.course,runtime_ipc.split);
 uint16_t distance=(uint16_t)(nd>>16),index=distance>>8,fraction=distance&255;
 if(index>=64){index=63;fraction=256;}
 uint16_t a=512+index*800,b=a+800;
 for(uint16_t z=0;z<512;z+=4){
  uint16_t ypos=(uint16_t)(672+(z>>2));
  int16_t ny=native_geo::interpolate(runtime_geo[a+ypos],runtime_geo[b+ypos],fraction);
  int16_t arcade_y=(int16_t)(((uint16_t)ny*224U+100U)/200U+18U);
  int16_t road_y=(int16_t)((223L-arcade_y)*16L);
  for(uint8_t j=0;j<4;j++)oroad.road_y[oroad.road_p0+z+j]=road_y;
 }
 for(uint16_t i=0;i<(uint16_t)oroad.road_p0;i++)oroad.road_y[i]=oroad.road_y[oroad.road_p0];
}
#endif
static int16_t runtime_crash_road_y(uint16_t z){
 // Source crash positions are clamped to 0..511 before reading. Each four
 // consecutive rows share the same GEO Y sample in the retained fill formula.
 uint16_t distance=(uint16_t)(native_geo::native_distance_fixed(oroad.road_pos,runtime_ipc.course,runtime_ipc.split)>>16);
 uint16_t index=distance>>8,fraction=distance&255;
 if(index>=64){index=63;fraction=256;}
 uint16_t a=(uint16_t)(512+index*800),b=(uint16_t)(a+800);
 uint16_t ypos=(uint16_t)(672+(z>>2));
 int16_t ny=native_geo::interpolate(runtime_geo[a+ypos],runtime_geo[b+ypos],fraction);
 int16_t arcade_y=(int16_t)(((uint16_t)ny*224U+100U)/200U+18U);
 return (int16_t)((223L-arcade_y)*16L);
}
static void runtime_dispatch_scenery(oentry *s,uint8_t zoom){
 switch(s->function_holder){
  case 0: if(s->yw==0)olevelobjs.sprite_normal(s,1);else olevelobjs.set_spr_zoom_priority(s,1);break;
  case 4: olevelobjs.sprite_lights(s);break;
  case 5: case 6: olevelobjs.set_spr_zoom_priority(s,1);break;
  case 7: olevelobjs.sprite_collision_z1c(s);break;
  case 8: olevelobjs.sprite_normal(s,2);break;
  case 9: olevelobjs.sprite_rocks(s);break;
  default: olevelobjs.set_spr_zoom_priority(s,zoom);break;
 }
}
static void runtime_schedule_scenery(){
 if(!runtime_bind_geometry())return;
 uint16_t progress=(uint16_t)(oroad.road_pos>>16);
 bool future=false;
 for(uint8_t i=0;i<5;i++){
  RuntimeObject &o=runtime_ipc.objects[i];
  if(runtime_object_bound[i] || o.active)continue;
  while(runtime_scenery_next<64){
   uint16_t ev=runtime_scenery_next;
   uint16_t desc=runtime_event_descriptor(ev),spawn=native_geo::spawn_word(runtime_phys,runtime_ipc.course,ev);
   if(desc==0xffff || spawn==0xffff){runtime_scenery_done[ev]=1;++runtime_scenery_next;continue;}
   if((spawn&0x07ff)>progress){future=true;break;}
   o.kind=1;o.event_slot=ev;o.event_descriptor=desc;o.active=1;o.zoom=1;++runtime_scenery_total;runtime_scenery_done[ev]=1;
   o.z=(spawn&0x8000)?0x10400L:0x10000L;runtime_object_spawned[i]=1;
   ++runtime_scenery_next;
   break;
  }
  // Descriptor and spawn lookups are pure: every later free slot would stop at this same
  // future event, so the rest of the scan cannot change any state.
  if(future)break;
 }
}
static uint16_t runtime_sprite_depth_after_move(int32_t z,uint8_t shift){
 uint32_t a=OSprites::SPRITE_ZOOM_LOOKUP+((((uint32_t)(z>>16))<<2)|osprites.sprite_scroll_speed);
 uint32_t value=roms.rom0p->read32(a)>>shift;
 if(config.tick_fps==60)value>>=1;else if(config.tick_fps==120)value>>=2;
 uint32_t after=(uint32_t)z+value;return (uint16_t)(after>>16);
}
static uint16_t runtime_traffic_depth_after_update(int32_t z,int16_t traffic_speed){
 int32_t adjust=(((int32_t)(oinitengine.car_increment>>16)-(int32_t)traffic_speed)*(int32_t)(z>>16))<<5;
 if(config.tick_fps==60)adjust>>=1;else if(config.tick_fps==120)adjust>>=2;
 return (uint16_t)(((uint32_t)(z+adjust))>>16);
}
static void runtime_objects(uint8_t kind){
 // Preserve one source oentry per native slot between ticks. This keeps the
 // exact source collision/traffic side effects (z, enable, fx, proximity,
 // skid, score) together in the same object state rather than a shadow model.
 for(uint16_t i=0;i<8;i++){
  RuntimeObject &o=runtime_ipc.objects[i];
  if(o.kind!=kind)continue;
  if(o.kind==kind)o.event_descriptor=(kind==1)?runtime_event_descriptor(o.event_slot):0xffff;
  oentry &s=runtime_object_state[i];
  if((kind!=1 && kind!=2) || (kind==1 && o.event_descriptor==0xffff)){
   if(runtime_object_bound[i])olevelobjs.hide_sprite(&s);
   runtime_object_bound[i]=0;continue;
  }
  if(kind==1 && !runtime_object_spawned[i]){
   uint16_t spawn=native_geo::spawn_word(runtime_phys,runtime_ipc.course,o.event_slot);
   if(spawn==0xffff || (uint16_t)(oroad.road_pos>>16)<(spawn&0x07ff))continue;
   o.active=1;o.z=(spawn&0x8000)?0x10400L:0x10000L;runtime_object_spawned[i]=1;
  }
  if(!o.active)continue;
  ++runtime_ipc.object_count;
  if(!runtime_object_bound[i]){
   runtime_clear(&s,sizeof(s));runtime_object_bound[i]=1;
   s.control=OSprites::ENABLE;s.addr=0;s.traffic_orig_speed=o.traffic_speed;
   if(kind==1){
    uint16_t d=o.event_descriptor;uint8_t raw=runtime_phys[d];
    s.control=(uint8_t)(OSprites::ENABLE|(raw&3));
    s.draw_props=(uint8_t)(raw&0xf0);s.function_holder=(int8_t)(raw>>4);
    int16_t wx=(int16_t)((int8_t)runtime_phys[d+1]*16);
    s.xw1=wx;s.xw2=wx;s.yw=(uint16_t)(native_geo::be16(runtime_phys,d+2)<<7);
    s.type=(uint16_t)runtime_phys[d+5]<<2;s.pal_src=runtime_phys[d+7];
   s.addr=runtime_le24(d+8);s.z=o.z;s.x=o.x;s.y=o.y;
   if(oroad.road_width_bak>0x118)s.control|=OSprites::WIDE_ROAD;
   olevelobjs.setup_sprite_routine(&s);
   }else{
    // The traffic type field is the original source traffic-property byte offset
    // (prop*8), not the native object's compact asset ID.
    s.type=o.type;s.width=o.width;s.addr=roms.rom0p->read32(outrun.adr.traffic_props+s.type);
    s.pal_src=roms.rom0p->read8(outrun.adr.traffic_props+s.type+4);s.z=o.z;
    // RuntimeObject.x is consumed once as the source world-lane coordinate.
    s.xw1=o.x;s.xw2=o.x;
   }
  }
  // Position/type may be refreshed by the host renderer each tick;
  // z and source-mutated traffic flags are fed back below as persistent state.
  if(!(s.control&OSprites::ENABLE)){
   runtime_object_bound[i]=0;o.active=0;continue;
  }
  if(kind==2){s.x=o.x;s.y=o.y;s.type=o.type;s.z=o.z;}
  s.traffic_speed=o.traffic_speed;s.traffic_fx=o.traffic_fx;
  s.traffic_proximity=o.traffic_proximity;
  if(kind==1){
   uint16_t z1=runtime_sprite_depth_after_move(s.z,0);
   if(runtime_scenery_lazy_ok(&s,z1)){osprites.move_sprite(&s,0);runtime_object_lazy[i]=1;}
   else{runtime_object_lazy[i]=0;
   runtime_project_depth(z1);
   runtime_dispatch_scenery(&s,o.zoom);}
  }else{
   // Collision may change car speed; derive source update_props depth after it.
   s.width=o.width;
   otraffic.check_collision(&s);
   uint16_t z1=runtime_traffic_depth_after_update(s.z,s.traffic_speed);
   runtime_project_depth(z1); // Exact final z consumed by reduced update_props.
   otraffic.update_props(&s);
  }
  o.x=s.x;o.y=s.y;o.type=s.type;o.width=s.width;o.z=s.z;o.yw=s.yw;
  o.traffic_speed=s.traffic_speed;o.traffic_fx=s.traffic_fx;
  o.traffic_proximity=s.traffic_proximity;
  if(!(s.control&OSprites::ENABLE)){o.active=0;runtime_object_bound[i]=0;}
 }
}

/* Refresh the display state of deferred scenery to the state after the last completed step. Never advances z, never
 * touches collision state. Idempotent. Needs the geometry bound by the caller.
 * The baseline computed this display in the middle of the step, from the projection basis captured at
 * runtime_projection_begin_phase (road_pos, car_x, road_p0, road_width, split, distance). ocrash.tick runs after the
 * phase and may move car_x/road_pos, so the basis is swapped back in for the duration of the refresh. */
static void runtime_refresh_display(){
 /* i is a full word on purpose: Watcom -ot kept i in CH, rebuilt BX from BL after the display call and trusted a stale BH. */
 uint16_t i;uint8_t any=0;
 for(i=0;i<5;i++){RuntimeObject &o=runtime_ipc.objects[i];
  if(runtime_object_lazy[i]&&runtime_object_bound[i]&&o.kind==1&&o.active&&(runtime_object_state[i].control&OSprites::ENABLE))any=1;}
 if(!any)return;
 uint32_t save_pos=oroad.road_pos;int32_t save_width=oroad.road_width;int16_t save_p0=oroad.road_p0;int16_t save_x=oinitengine.car_x_pos;
#if !defined(RUNTIME_LAZY_NOBASIS)
 oroad.road_pos=runtime_projection_key_road_pos;oroad.road_width=runtime_projection_key_road_width;
 oroad.road_p0=(int16_t)runtime_projection_key_road_p0;oinitengine.car_x_pos=runtime_projection_key_car_x;
#endif
 runtime_projection_cache_active=
#if defined(RUNTIME_LAZY_RECOMP)
 0;
#else
 1;
#endif
 for(i=0;i<5;i++){
  RuntimeObject &o=runtime_ipc.objects[i];oentry &s=runtime_object_state[i];
  if(!(runtime_object_lazy[i]&&runtime_object_bound[i]&&o.kind==1&&o.active&&(s.control&OSprites::ENABLE)))continue;
#if defined(RUNTIME_LAZY_DIAG4) && defined(__I86__)
  o.yw=(uint16_t)s.xw1;
#endif
#if !defined(RUNTIME_LAZY_NOPROJ)
  runtime_project_depth((uint16_t)(s.z>>16));
#endif
#if defined(RUNTIME_LAZY_ADV)
  osprites.move_sprite(&s,0);
#endif
  {
   uint8_t zoom=o.zoom;
   if(s.function_holder==0||s.function_holder==5||s.function_holder==6)zoom=1;
   else if(s.function_holder==8)zoom=
#if defined(RUNTIME_LAZY_ZOOM)
    1;
#else
    2;
#endif
#if defined(RUNTIME_LAZY_TAILCOPY)
   {oentry tmp=s;olevelobjs.set_spr_zoom_priority_display(&tmp,zoom);}
#elif !defined(RUNTIME_LAZY_NOTAIL)
   olevelobjs.set_spr_zoom_priority_display(&s,zoom);
#endif
  }
#if !defined(RUNTIME_LAZY_NOXY)
  o.x=s.x;o.y=s.y;
#endif
  runtime_object_lazy[i]=0;
#if defined(RUNTIME_LAZY_DIAG2) && defined(__I86__)
  o.yw=(uint16_t)s.xw1;o.width=(uint16_t)(((uint16_t)(uint8_t)s.function_holder)|((uint16_t)s.control<<8));o.type=(uint16_t)runtime_phys[o.event_descriptor+1];
#endif
#if defined(RUNTIME_LAZY_DIAG) && defined(__I86__)
  o.event_descriptor=(uint16_t)runtime_projection_key_car_x;o.width=(uint16_t)save_x;o.type=(uint16_t)runtime_projection_key_road_p0;o.traffic_speed=(int16_t)save_p0;
#endif
 }
 runtime_projection_cache_active=0;
 oroad.road_pos=save_pos;oroad.road_width=save_width;oroad.road_p0=save_p0;oinitengine.car_x_pos=save_x;
}
static void runtime_step(){
 runtime_geometry();
 // Pinned main_switch: update_road then update_engine; adjust_inputs follows,
 // so physical keyboard state has the original one-source-tick latency.
 config.controls.gear=runtime_ipc.gear_mode?config.controls.GEAR_AUTO:config.controls.GEAR_BUTTON;
 oinputs.gear=runtime_ipc.gear_value?true:false;
 oferrari.move();if(oferrari.car_ctrl_active){oferrari.set_curve_adjust();oferrari.set_ferrari_x();oferrari.do_skid();oferrari.check_wheels();oferrari.set_ferrari_bounds();}
 oinitengine.set_granular_position();oinitengine.set_fine_position();
 oferrari.car_x_diff=(int16_t)(oinitengine.car_x_pos-oinitengine.car_x_old);oinitengine.car_x_old=oinitengine.car_x_pos;
 if(oferrari.wheel_state==OFerrari::WHEELS_ON)ostats.convert_speed_score((uint16_t)(oinitengine.car_increment>>16));
 input.held=runtime_ipc.held_keys;
 oinputs.digital_steering();oinputs.digital_pedals();oinputs.adjust_inputs();
 runtime_ipc.object_count=0;
 // OFerrari movement and fine-position updates are complete; these inputs
 // remain stable through scenery scheduling and all object projections.
 runtime_projection_begin_phase();
 runtime_schedule_scenery();runtime_objects(1);runtime_objects(2);
 runtime_projection_cache_active=0;
 oinitengine.init_crash_bonus();source_latch_scenery_collision();
 ocrash.tick();
 runtime_ipc.rng_value=outils::rnd_seed;
}
static void runtime_snapshot(){
#if !defined(__I86__) && defined(RUNTIME_HOST_FIXTURE)
 assert(ocrash.spr_ferrari==&crash_actors[0] && ocrash.spr_shadow==&crash_actors[1]);
 assert(ocrash.spr_pass1==&crash_actors[2] && ocrash.spr_pass1s==&crash_actors[3]);
 assert(ocrash.spr_pass2==&crash_actors[4] && ocrash.spr_pass2s==&crash_actors[5]);
 assert(oferrari.ferrari_pal==2);
 for(uint8_t i=0;i<6;i++)if((crash_actors[i].control&OSprites::ENABLE)&&crash_actors[i].zoom){if(!crash_actors[i].addr)fprintf(stderr,"visible actor=%u state=%d control=%u frame=%u zoom=%u palette=%u shadow=%u\n",i,(int)ocrash.crash_state,crash_actors[i].control,crash_actors[i].jump_index,crash_actors[i].zoom,crash_actors[i].pal_src,crash_actors[i].shadow);assert(crash_actors[i].addr!=0);if(crash_actors[i].shadow!=7)assert(crash_actors[i].pal_src!=0);}
#endif
 runtime_ipc.source_increment=oinitengine.car_increment;runtime_ipc.source_speed=oinitengine.car_increment>>16;
 runtime_ipc.source_x=oinitengine.car_x_pos;runtime_ipc.native_x=(int16_t)(-((int32_t)oinitengine.car_x_pos*256)/212);
 runtime_ipc.source_road_pos=oroad.road_pos;
 runtime_ipc.native_distance=native_geo::native_distance_fixed(oroad.road_pos,runtime_ipc.course,runtime_ipc.split);
 runtime_ipc.road_width=(uint16_t)(oroad.road_width>>16);runtime_ipc.wheel_state=oferrari.wheel_state;
 runtime_ipc.crash_state=(uint8_t)ocrash.crash_state;runtime_ipc.crash_counter=(uint8_t)ocrash.crash_counter;
 runtime_ipc.crash_type=(uint8_t)ocrash.crash_type;runtime_ipc.collision_count=ocrash.coll_count1;
 runtime_ipc.skid_counter=ocrash.skid_counter;runtime_ipc.traffic_collision=otraffic.collision_traffic;
 runtime_ipc.traffic_mask=otraffic.collision_mask;runtime_ipc.traffic_speed_total=otraffic.traffic_speed_total;
 runtime_ipc.car_ctrl_active=oferrari.car_ctrl_active?1:0;runtime_ipc.car_state=(uint8_t)oferrari.car_state;
 runtime_ipc.spin_control1=(uint8_t)ocrash.spin_control1;runtime_ipc.spin_control2=(uint8_t)ocrash.spin_control2;
 runtime_ipc.score_bcd=ostats.score;runtime_ipc.time_left=(uint16_t)ostats.time_counter;runtime_ipc.gear_value=oinputs.gear?1:0;
 oentry *a[6]={ocrash.spr_ferrari,ocrash.spr_shadow,ocrash.spr_pass1,ocrash.spr_pass1s,ocrash.spr_pass2,ocrash.spr_pass2s};
 for(uint8_t i=0;i<6;i++){RuntimeActor &d=runtime_ipc.actors[i];oentry *s=a[i];if(!s)continue;d.addr=s->addr;d.x=s->x;d.y=s->y;d.zoom=s->zoom;d.control=s->control;d.palette=s->pal_src;d.anchor=s->draw_props;d.priority=s->priority;d.road_priority=s->road_priority;}
}
static void runtime_reset(){
 runtime_desc_key_phys=0;
 uint16_t c=runtime_ipc.course,st=runtime_ipc.stage,t=runtime_ipc.time_left;runtime_init_globals();
#if !defined(__I86__) && defined(RUNTIME_HOST_FIXTURE)
 for(uint8_t i=0;i<6;i++){assert(crash_actors[i].jump_index==113+i);assert(crash_actors[i].function_holder==-1);} assert(crash_actors[0].shadow==3 && crash_actors[1].shadow==7 && crash_actors[2].shadow==3 && crash_actors[3].shadow==7 && crash_actors[4].shadow==3 && crash_actors[5].shadow==7);  assert(oferrari.ferrari_pal==2); assert(crash_actors[1].addr==0x103B6UL && crash_actors[1].shadow==7 && crash_actors[1].zoom==0x80 && crash_actors[1].draw_props==8); assert(crash_actors[3].addr==0x103B6UL && crash_actors[3].shadow==7); assert(crash_actors[5].addr==0x103B6UL && crash_actors[5].shadow==7 && crash_actors[5].draw_props==8);
#endif

 runtime_ipc.course=c;runtime_ipc.stage=st;runtime_ipc.time_left=t;runtime_ipc.status=0;
 if(!runtime_bind_geometry()){runtime_ipc.status=2;return;}runtime_geometry();runtime_snapshot();
}
static void runtime_command(){
 if(runtime_ipc.command==0){runtime_reset();return;}
 if(runtime_ipc.command==2){runtime_rebase(runtime_ipc.course,runtime_ipc.stage);if(!runtime_bind_geometry()){runtime_ipc.status=2;return;}runtime_geometry();runtime_snapshot();runtime_ipc.status=0;return;}
 if(runtime_ipc.command==1){
  runtime_ipc.status=0;if(!runtime_bind_geometry()){runtime_ipc.status=2;return;}runtime_step();runtime_snapshot();
#if defined(RUNTIME_HOST_FIXTURE) && !defined(RUNTIME_GATE_MANUAL) && !defined(RUNTIME_LAZY_NOREF)
  runtime_refresh_display();
#endif
  if(rom_fault_addr)runtime_ipc.status=3;return;
 }
 if(runtime_ipc.command==3){runtime_ipc.status=0;if(!runtime_bind_geometry()){runtime_ipc.status=2;return;}
#if !defined(RUNTIME_LAZY_CMD3_NOP) || !defined(__I86__)
 runtime_refresh_display();
#endif
#if defined(RUNTIME_LAZY_DELAY)
 {volatile uint16_t k;for(k=0;k<RUNTIME_LAZY_DELAY;k++);}
#endif
 return;}
 runtime_ipc.status=0xffff;
}
extern "C" void __far __cdecl runtime_entry(){runtime_command();}
#if defined(RUNTIME_HOST_FIXTURE)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t *fixture_read(const char *path,uint32_t expected){
 FILE *f=fopen(path,"rb");if(!f)return 0;uint8_t *p=(uint8_t*)malloc(expected);
 if(!p){fclose(f);return 0;}uint32_t n=(uint32_t)fread(p,1,expected,f);int extra=fgetc(f);fclose(f);
 if(n!=expected||extra!=EOF){free(p);return 0;}return p;
}
static bool fixture_recovery(uint8_t *geo,const char *label,uint16_t speed,FILE *out){
 runtime_ipc.course=0;runtime_ipc.stage=0;runtime_ipc.time_left=60;runtime_ipc.split=0;runtime_ipc.source_x=450;
 runtime_ipc.command=0;runtime_entry();if(runtime_ipc.status)return false;
 oinitengine.car_increment=(uint32_t)speed<<16;oferrari.car_inc_old=speed;oferrari.torque=0x1000;oferrari.revs=0;oinitengine.ingame_engine=true;
 oentry scenery;runtime_clear(&scenery,sizeof(scenery));scenery.control=OSprites::ENABLE;scenery.z=0x1B0L<<16;scenery.x=-50;scenery.type=0;scenery.width=20;
 runtime_project_depth(0x1B0);runtime_project_depth(runtime_sprite_depth_after_move(scenery.z,0));
 runtime_project_depth(8);olevelobjs.sprite_normal(&scenery,1);
 uint8_t contact=olevelobjs.collision_sprite;source_latch_scenery_collision();ocrash.enable();ocrash.crash_counter=1;
 uint16_t ticks=0,actor_frames=0;uint8_t max_state=0;
 for(;ticks<1000 && (ocrash.crash_state || ocrash.crash_counter);ticks++){
  outrun.tick_frame=1;runtime_step();
  if(ocrash.crash_state>max_state)max_state=(uint8_t)ocrash.crash_state;
  if(ocrash.spr_ferrari->addr)actor_frames++;
  if(runtime_ipc.status||rom_fault_addr)break;
 }
 fprintf(out,"{\"scenario\":\"%s\",\"speed\":%u,\"contact_latched\":%u,\"ticks\":%u,\"final_state\":%d,\"max_state\":%u,\"crash_counter\":%d,\"actor_frames\":%u,\"control_active\":%u,\"rom_fault\":%lu}\n",label,speed,contact,ticks,ocrash.crash_state,max_state,ocrash.crash_counter,actor_frames,oferrari.car_ctrl_active,(unsigned long)rom_fault_addr);
 return contact && ticks<1000 && !runtime_ipc.status && !rom_fault_addr && ocrash.crash_state==0 && ocrash.crash_counter==0 && oferrari.car_ctrl_active;
}
static int fixture_gear_smoke(){
 uint8_t *p=fixture_read("..\\PHYS.BIN",16033);
 uint8_t *g=fixture_read("..\\..\\behavior-v014-320\\GEO00.BIN",52512);
 if(!p||!g)return 41;runtime_set_host_geometry(p,g);
 runtime_ipc.course=0;runtime_ipc.stage=0;runtime_ipc.time_left=60;runtime_ipc.split=0;
 runtime_ipc.gear_mode=0;runtime_ipc.gear_value=0;runtime_ipc.command=0;runtime_entry();
 if(runtime_ipc.status||runtime_ipc.source_x!=450)return 42;
 runtime_ipc.gear_mode=0;runtime_ipc.gear_value=1;runtime_ipc.command=1;runtime_entry();
 uint8_t manual=runtime_ipc.gear_value;
 runtime_ipc.gear_mode=1;runtime_ipc.gear_value=0;oinitengine.car_increment=0xA1UL<<16;oferrari.car_inc_old=0xA1;
 runtime_ipc.command=1;runtime_entry();uint8_t auto_high=runtime_ipc.gear_value;
 runtime_ipc.gear_value=1;oinitengine.car_increment=0xA0UL<<16;oferrari.car_inc_old=0xA0;
 runtime_ipc.command=1;runtime_entry();uint8_t auto_low=runtime_ipc.gear_value;
 FILE *f=fopen("revision-r17-crash-y/runtime_gear_smoke.json","wt");if(!f){free(p);free(g);return 43;}
 fprintf(f,"{\\n \\\"manual_input_one\\\":%u,\\n \\\"auto_above_threshold\\\":%u,\\n \\\"auto_at_threshold\\\":%u,\\n \\\"rom_fault\\\":%lu\\n}\\n",manual,auto_high,auto_low,(unsigned long)rom_fault_addr);
 fclose(f);free(p);free(g);
 return (manual==1 && auto_high==1 && auto_low==0 && !runtime_ipc.status && !rom_fault_addr)?0:44;
}
static int fixture_traffic_contract(){
 uint8_t *p=fixture_read("..\\PHYS.BIN",16033);
 uint8_t *g=fixture_read("..\\..\\behavior-v014-320\\GEO00.BIN",52512);
 if(!p||!g)return 51;runtime_set_host_geometry(p,g);
 runtime_ipc.course=0;runtime_ipc.stage=0;runtime_ipc.time_left=60;runtime_ipc.split=0;runtime_ipc.command=0;runtime_entry();
 if(runtime_ipc.status)return 52;
 oinitengine.car_increment=200UL<<16;oferrari.car_inc_old=200;oferrari.torque=0x1000;oferrari.revs=0;
 RuntimeObject &hit=runtime_ipc.objects[5];hit.active=1;hit.kind=2;hit.type=12<<3;hit.width=16;hit.x=0;hit.y=200;hit.traffic_speed=100;hit.z=472L<<16;
 runtime_ipc.command=1;runtime_entry();
 uint16_t hit_z=(uint16_t)(runtime_ipc.objects[5].z>>16),hit_speed=(uint16_t)(oinitengine.car_increment>>16);
 if(runtime_ipc.status||hit_z!=453||hit_speed!=20){fprintf(stderr,"post_collision_z=%u expected=453 speed=%u expected=20 status=%u\n",hit_z,hit_speed,runtime_ipc.status);free(p);free(g);return 53;}
 runtime_ipc.course=0;runtime_ipc.stage=0;runtime_ipc.time_left=60;runtime_ipc.split=0;runtime_ipc.command=0;runtime_entry();
 oinitengine.car_increment=100UL<<16;oferrari.car_inc_old=100;oferrari.torque=0x1000;oferrari.revs=0;
 const int16_t lanes[3]={-100,0,100};
 for(uint8_t i=0;i<3;i++){RuntimeObject &o=runtime_ipc.objects[5+i];o.active=1;o.kind=2;o.type=(uint16_t)(i==0?12:(i==1?16:5))<<3;o.width=16;o.x=lanes[i];o.y=200;o.traffic_speed=100;o.z=0x100L<<16;}
 int16_t xs[2][3];
 for(uint8_t t=0;t<2;t++){runtime_ipc.command=1;runtime_ipc.held_keys=0;runtime_entry();if(runtime_ipc.status){free(p);free(g);return 54;}for(uint8_t i=0;i<3;i++)xs[t][i]=runtime_ipc.objects[5+i].x;
  if(xs[t][0]==xs[t][1]||xs[t][1]==xs[t][2]||xs[t][0]==xs[t][2]){fprintf(stderr,"lanes collapsed tick=%u x=%d,%d,%d\n",t,xs[t][0],xs[t][1],xs[t][2]);free(p);free(g);return 55;}}
 FILE *f=fopen("contract.csv","wt");if(!f){free(p);free(g);return 56;}
 fprintf(f,"collision,input_car_speed=200,input_traffic_speed=100,input_z=472,post_speed=%u,post_z=%u,expected_speed=20,expected_z=453\n",hit_speed,hit_z);
 fprintf(f,"world_lanes=-100,0,100\ntick0_screen_x=%d,%d,%d\ntick1_screen_x=%d,%d,%d\n",xs[0][0],xs[0][1],xs[0][2],xs[1][0],xs[1][1],xs[1][2]);fclose(f);free(p);free(g);return 0;
}
static int fixture_equiv_120(bool compact){
 const char *phys_path=compact?"..\\compact-assets-320\\PHYS00.BIN":"..\\PHYS.BIN";
 uint32_t phys_bytes=compact?1482:16033;
 uint8_t *p=fixture_read(phys_path,phys_bytes);
 uint8_t *g=fixture_read("..\\..\\behavior-v014-320\\GEO00.BIN",52512);
 if(!p||!g)return 31;runtime_set_host_geometry(p,g);
 runtime_ipc.course=0;runtime_ipc.stage=0;runtime_ipc.time_left=60;runtime_ipc.split=0;
 runtime_ipc.command=0;runtime_entry();if(runtime_ipc.status||runtime_ipc.source_x!=450){fprintf(stderr,"v%u reset status=%u x=%d valid=%u course=%u\\n",(unsigned)(compact?5:4),runtime_ipc.status,runtime_ipc.source_x,native_geo::validate(p),(unsigned)(p[4]==5?native_geo::le16(p,28):0xffff));return 32;}
 for(uint8_t i=0;i<3;i++){
  RuntimeObject &o=runtime_ipc.objects[5+i];o.active=1;o.kind=2;o.type=(uint16_t)(i==0?12:(i==1?16:5))<<3;
  o.width=16;o.x=0;o.y=200;o.traffic_speed=100;o.z=0x1d8L<<16;
 }
 const char *suffix=compact?"v5":"v4";
 char snapshot_name[48],summary_name[48];sprintf(snapshot_name,"revision-r17-crash-y/runtime_equiv120_%s.bin",suffix);sprintf(summary_name,"revision-r17-crash-y/runtime_equiv120_%s.json",suffix);
 FILE *snap=fopen(snapshot_name,"wb"),*csv=fopen(compact?"revision-r17-crash-y/runtime_equiv120_v5.csv":"revision-r17-crash-y/runtime_equiv120_v4.csv","wt");
 if(!snap||!csv){if(snap)fclose(snap);if(csv)fclose(csv);free(p);free(g);return 33;}
 fprintf(csv,"tick,status,source_progress,split,speed,road_pos,collision,crash,rom_fault\n");
 uint16_t rows=0;uint8_t status=0;
 for(uint16_t t=0;t<120;t++){
  runtime_ipc.held_keys=(t<90)?Input::ACCEL:Input::BRAKE;runtime_ipc.command=1;runtime_entry();++rows;
  if(fwrite(&runtime_ipc,1,sizeof(runtime_ipc),snap)!=sizeof(runtime_ipc)){fclose(snap);fclose(csv);free(p);free(g);return 34;}
  fprintf(csv,"%u,%u,%u,%u,%lu,%lu,%u,%u,%lu\n",t,runtime_ipc.status,(unsigned)(runtime_ipc.source_road_pos>>16),runtime_ipc.split,(unsigned long)runtime_ipc.source_speed,(unsigned long)runtime_ipc.source_road_pos,runtime_ipc.traffic_collision,runtime_ipc.crash_state,(unsigned long)rom_fault_addr);
  if(runtime_ipc.status){status=(uint8_t)runtime_ipc.status;break;}
 }
 fclose(snap);fclose(csv);uint8_t course_rejected=0;
 if(compact){runtime_ipc.course=1;runtime_ipc.command=1;runtime_entry();course_rejected=runtime_ipc.status==2;}
 FILE *summary=fopen(summary_name,"wt");if(!summary){free(p);free(g);return 35;}
 fprintf(summary,"{\n \"phys_version\":%u,\n \"embedded_course\":%u,\n \"rows\":%u,\n \"status\":%u,\n \"rom_fault\":%lu,\n \"wrong_course_rejected\":%u\n}\n",p[4],(unsigned)(p[4]==5?native_geo::le16(p,28):0xffff),rows,status,(unsigned long)rom_fault_addr,course_rejected);
 fclose(summary);free(p);free(g);return(status||rom_fault_addr||(compact&&!course_rejected))?36:0;
}
static int fixture_long_run(){
 uint8_t *p=fixture_read("..\\PHYS.BIN",16033);
 uint8_t *g=fixture_read("..\\..\\behavior-v014-320\\GEO00.BIN",52512);
 if(!p||!g)return 12;runtime_set_host_geometry(p,g);
 runtime_ipc.course=0;runtime_ipc.stage=0;runtime_ipc.time_left=600;runtime_ipc.split=0;
 runtime_ipc.source_x=450;runtime_ipc.command=0;runtime_entry();
 if(runtime_ipc.status||runtime_ipc.source_x!=450)return 13;
 FILE *f=fopen("revision-r17-crash-y/runtime_long_trace.csv","wt");if(!f)return 14;
 fprintf(f,"tick,status,source_progress,split,split_state,speed,increment,road_delta,assigned,active,event0,event1,event2,event3,event4,handler0,handler1,handler2,handler3,handler4,collision_count,crash_state,crash_counter,traffic_collision,rom_fault\n");
 uint16_t rows=0,max_active=0,split_tick=0xffff,split_handoff_tick=0xffff;
 uint32_t collision_counter_changes=0,crash_activations=0,crash_recoveries=0,slot_reuses=0;int16_t previous_collision=ocrash.coll_count1;uint8_t previous_crash=ocrash.crash_state;
 uint16_t handler_assignments[16]={0};uint8_t assigned_seen[64]={0},status=0;int8_t last_slot_event[5]={-1,-1,-1,-1,-1};
 for(uint16_t t=0;t<3000;t++){
  runtime_ipc.held_keys=Input::ACCEL;runtime_ipc.command=1;runtime_entry();++rows;
  uint16_t progress=(uint16_t)(oroad.road_pos>>16);uint8_t active=0;int event[5]={-1,-1,-1,-1,-1},handler[5]={-1,-1,-1,-1,-1};
  for(uint8_t i=0;i<5;i++){
   RuntimeObject &o=runtime_ipc.objects[i];
   if(runtime_object_bound[i]&&o.kind==1){++active;event[i]=o.event_slot;uint16_t d=o.event_descriptor;
    if(d!=0xffff){handler[i]=(runtime_phys[d]>>4)&15;
     if(o.event_slot<64&&!assigned_seen[o.event_slot]){assigned_seen[o.event_slot]=1;if(handler_assignments[handler[i]]<65535)++handler_assignments[handler[i]];}
     if(last_slot_event[i]>=0&&last_slot_event[i]!=(int8_t)o.event_slot)++slot_reuses;
     last_slot_event[i]=(int8_t)o.event_slot;
    }}
  }
  if(active>max_active)max_active=active;
  if(runtime_ipc.split&&split_tick==0xffff)split_tick=t;
  if(runtime_ipc.split&&progress>=256&&split_handoff_tick==0xffff)split_handoff_tick=t;
  if(ocrash.coll_count1!=previous_collision){++collision_counter_changes;previous_collision=ocrash.coll_count1;}
  if(ocrash.crash_state&&!previous_crash)++crash_activations;
  if(!ocrash.crash_state&&previous_crash)++crash_recoveries;
  previous_crash=(uint8_t)ocrash.crash_state;
  fprintf(f,"%u,%u,%u,%u,%u,%lu,%lu,%ld,%u,%u,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%u,%lu\n",t,runtime_ipc.status,progress,runtime_ipc.split,oinitengine.rd_split_state,(unsigned long)runtime_ipc.source_speed,(unsigned long)runtime_ipc.source_increment,(long)oroad.road_pos_change,(unsigned)runtime_scenery_total,active,event[0],event[1],event[2],event[3],event[4],handler[0],handler[1],handler[2],handler[3],handler[4],ocrash.coll_count1,ocrash.crash_state,ocrash.crash_counter,otraffic.collision_traffic,(unsigned long)rom_fault_addr);
  if(runtime_ipc.status){status=(uint8_t)runtime_ipc.status;break;}
  if(split_handoff_tick!=0xffff)break;
 }
 fclose(f);
 FILE *j=fopen("revision-r17-crash-y/runtime_long_summary.json","wt");if(!j){free(p);free(g);return 15;}
 fprintf(j,"{\n \"rows\":%u,\n \"status\":%u,\n \"rom_fault\":%lu,\n \"max_active_scenery_slots\":%u,\n \"distinct_scenery_events_assigned\":%u,\n \"scenery_slot_reuses\":%lu,\n \"split_transition_tick\":%u,\n \"split_handoff_tick\":%u,\n \"source_collision_counter_changes\":%lu,\n \"crash_activations\":%lu,\n \"crash_recoveries\":%lu,\n \"final_crash_state\":%u,\n \"final_crash_counter\":%d,\n \"traffic_collision_value\":%u,\n \"handler_assignment_counts\":[",rows,status,(unsigned long)rom_fault_addr,max_active,(unsigned)runtime_scenery_total,(unsigned long)slot_reuses,split_tick,split_handoff_tick,(unsigned long)collision_counter_changes,(unsigned long)crash_activations,(unsigned long)crash_recoveries,(unsigned)ocrash.crash_state,(int)ocrash.crash_counter,otraffic.collision_traffic);
 for(uint8_t h=0;h<16;h++)fprintf(j,"%s%u",h?",":"",handler_assignments[h]);
 fprintf(j,"]\n}\n");fclose(j);free(p);free(g);return(status||rom_fault_addr)?16:0;
}
static int fixture_split_run(bool compact,bool equivalence){
 const char *phys_path=compact?"..\\compact-assets-320\\PHYS00.BIN":"..\\PHYS.BIN";
 uint8_t *p=fixture_read(phys_path,compact?1482:16033);
 uint8_t *g=fixture_read("..\\..\\behavior-v014-320\\GEO00.BIN",52512);
 if(!p||!g)return 22;runtime_set_host_geometry(p,g);
 runtime_ipc.course=0;runtime_ipc.stage=0;runtime_ipc.time_left=600;runtime_ipc.split=0;
 runtime_ipc.source_x=450;runtime_ipc.command=0;runtime_entry();
 if(runtime_ipc.status||runtime_ipc.source_x!=450)return 23;
 const char *trace_name=equivalence?(compact?"revision-r17-crash-y/runtime_split_equiv_v5.csv":"revision-r17-crash-y/runtime_split_equiv_v4.csv"):"revision-r17-crash-y/runtime_split_trace.csv";
 const char *summary_name=equivalence?(compact?"revision-r17-crash-y/runtime_split_equiv_v5.json":"revision-r17-crash-y/runtime_split_equiv_v4.json"):"revision-r17-crash-y/runtime_split_summary.json";
 const char *snap_name=compact?"revision-r17-crash-y/runtime_split_equiv_v5.bin":"revision-r17-crash-y/runtime_split_equiv_v4.bin";
 FILE *f=fopen(trace_name,"wt");if(!f)return 24;
 FILE *snap=equivalence?fopen(snap_name,"wb"):0;if(equivalence&&!snap){fclose(f);return 27;}
 fprintf(f,"tick,status,source_progress,native_distance,split,split_state,speed,road_delta,assigned,active,event0,event1,event2,event3,event4,handler0,handler1,handler2,handler3,handler4,collision_count,crash_state,crash_counter,traffic_collision,rom_fault\n");
 uint16_t rows=0,split_tick=0xffff,handoff_tick=0xffff,pending_at_split=0xffff,max_active=0,handler_assignments[16]={0};uint8_t assigned_seen[64]={0};int8_t last_slot_event[5]={-1,-1,-1,-1,-1};uint32_t slot_reuses=0;
 uint32_t previous_distance=runtime_ipc.native_distance,split_before=0,split_after=0;
 int32_t min_distance_delta=0x7fffffffL,max_distance_delta=(-0x7fffffffL-1L);
 uint32_t collision_changes=0,crash_activations=0,crash_recoveries=0;int16_t previous_collision=ocrash.coll_count1;uint8_t previous_crash=ocrash.crash_state,status=0;
 for(uint16_t t=0;t<6000;t++){
  runtime_ipc.held_keys=Input::ACCEL;runtime_ipc.command=1;runtime_entry();++rows;
  if(equivalence&&fwrite(&runtime_ipc,1,sizeof(runtime_ipc),snap)!=sizeof(runtime_ipc)){fclose(snap);fclose(f);free(p);free(g);return 28;}
  uint16_t progress=(uint16_t)(oroad.road_pos>>16);uint8_t active=0;int event[5]={-1,-1,-1,-1,-1},handler[5]={-1,-1,-1,-1,-1};
  for(uint8_t i=0;i<5;i++){RuntimeObject &o=runtime_ipc.objects[i];if(runtime_object_bound[i]&&o.kind==1){++active;event[i]=o.event_slot;uint16_t d=o.event_descriptor;if(d!=0xffff){handler[i]=(runtime_phys[d]>>4)&15;if(o.event_slot<64&&!assigned_seen[o.event_slot]){assigned_seen[o.event_slot]=1;if(handler_assignments[handler[i]]<65535)++handler_assignments[handler[i]];}if(last_slot_event[i]>=0&&last_slot_event[i]!=(int8_t)o.event_slot)++slot_reuses;last_slot_event[i]=(int8_t)o.event_slot;}}}
  if(active>max_active)max_active=active;
  int32_t distance_delta=(int32_t)(runtime_ipc.native_distance-previous_distance);
  if(distance_delta<min_distance_delta)min_distance_delta=distance_delta;
  if(distance_delta>max_distance_delta)max_distance_delta=distance_delta;
  if(runtime_ipc.split&&split_tick==0xffff){
   split_tick=t;split_before=previous_distance;split_after=runtime_ipc.native_distance;
   pending_at_split=0;
   for(uint8_t ev=0;ev<64;ev++)if(!runtime_scenery_done[ev]){
    uint16_t spawn=native_geo::spawn_word(runtime_phys,runtime_ipc.course,ev);
    if(spawn!=0xffff&&(spawn&0x07ff)>1948)++pending_at_split;
   }
  }
  if(runtime_ipc.split&&progress>=256&&handoff_tick==0xffff)handoff_tick=t;
  if(ocrash.coll_count1!=previous_collision){++collision_changes;previous_collision=ocrash.coll_count1;}
  if(ocrash.crash_state&&!previous_crash)++crash_activations;
  if(!ocrash.crash_state&&previous_crash)++crash_recoveries;
  previous_crash=(uint8_t)ocrash.crash_state;
  fprintf(f,"%u,%u,%u,%lu,%u,%u,%lu,%ld,%u,%u,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%u,%lu\n",t,runtime_ipc.status,progress,(unsigned long)runtime_ipc.native_distance,runtime_ipc.split,oinitengine.rd_split_state,(unsigned long)runtime_ipc.source_speed,(long)oroad.road_pos_change,(unsigned)runtime_scenery_total,active,event[0],event[1],event[2],event[3],event[4],handler[0],handler[1],handler[2],handler[3],handler[4],ocrash.coll_count1,ocrash.crash_state,ocrash.crash_counter,otraffic.collision_traffic,(unsigned long)rom_fault_addr);
  previous_distance=runtime_ipc.native_distance;
  if(runtime_ipc.status){status=(uint8_t)runtime_ipc.status;break;}
  if(handoff_tick!=0xffff)break;
 }
 if(snap)fclose(snap);fclose(f);
 FILE *j=fopen(summary_name,"wt");if(!j){free(p);free(g);return 25;}
 fprintf(j,"{\n \"rows\":%u,\n \"status\":%u,\n \"rom_fault\":%lu,\n \"split_transition_tick\":%u,\n \"split_handoff_tick\":%u,\n \"pending_phys_events_at_split\":%u,\n \"native_distance_before_split\":%lu,\n \"native_distance_after_split\":%lu,\n \"native_distance_split_delta\":%ld,\n \"native_distance_delta_min\":%ld,\n \"native_distance_delta_max\":%ld,\n \"final_native_distance\":%lu,\n \"distinct_scenery_events_assigned\":%u,\n \"scenery_slot_reuses\":%lu,\n \"max_active_scenery_slots\":%u,\n \"source_collision_counter_changes\":%lu,\n \"crash_activations\":%lu,\n \"crash_recoveries\":%lu,\n \"final_crash_state\":%u,\n \"final_crash_counter\":%d,\n \"traffic_collision_value\":%u,\n \"handler_assignment_counts\":[",rows,status,(unsigned long)rom_fault_addr,split_tick,handoff_tick,pending_at_split,(unsigned long)split_before,(unsigned long)split_after,(long)((int32_t)(split_after-split_before)),(long)min_distance_delta,(long)max_distance_delta,(unsigned long)runtime_ipc.native_distance,(unsigned)runtime_scenery_total,(unsigned long)slot_reuses,(unsigned)max_active,(unsigned long)collision_changes,(unsigned long)crash_activations,(unsigned long)crash_recoveries,(unsigned)ocrash.crash_state,(int)ocrash.crash_counter,otraffic.collision_traffic);
 for(uint8_t h=0;h<16;h++)fprintf(j,"%s%u",h?",":"",handler_assignments[h]);fprintf(j,"]");
 fprintf(j,"\n}\n");
 fclose(j);free(p);free(g);return(status||rom_fault_addr)?26:0;
}
static void runtime_write_projection_stats(const char *name){
 FILE *f=fopen(name,"wt");if(!f)return;
 fprintf(f,"{\n \"requests\":%lu,\n \"computes\":%lu,\n \"hits\":%lu,\n \"key_invalidations\":%lu,\n \"phases_started\":%lu\n}\n",(unsigned long)runtime_projection_requests,(unsigned long)runtime_projection_computes,(unsigned long)runtime_projection_hits,(unsigned long)runtime_projection_key_invalidations,(unsigned long)runtime_projection_phases_started);
 fclose(f);
}
static int runtime_fixture_result(int result,const char *stats){runtime_write_projection_stats(stats);return result;}
static uint32_t source_rom32_reference(uint32_t a){
 return ((uint32_t)source_rom8(a)<<24)|((uint32_t)source_rom8(a+1)<<16)|((uint32_t)source_rom8(a+2)<<8)|source_rom8(a+3);
}
static int rom32_compare_one(uint32_t a,uint32_t *cases){
 rom_fault_addr=0;uint32_t expected=source_rom32_reference(a),expected_fault=rom_fault_addr;
 rom_fault_addr=0;uint32_t actual=roms.rom0.read32(a),actual_fault=rom_fault_addr;
 ++*cases;return expected==actual && expected_fault==actual_fault;
}
static int rom32_exhaustive_test(){
 uint32_t saved_fault=rom_fault_addr,cases=0;bool ok=true;
 const uint32_t ranges[][2]={{0x27FFCUL,0x30003UL},{0x0FFDUL,0x1202UL},{0x21FDUL,0x2692UL},
  {0x307FDUL,0x30902UL},{0x12127UL,0x1252DUL},{0x4CF7UL,0x4D9DUL},{0x5421UL,0x5527UL},{0xFFFFFFFCUL,0xFFFFFFFEUL}};
 for(uint8_t r=0;r<8 && ok;r++)for(uint32_t a=ranges[r][0];a<=ranges[r][1];a++)if(!rom32_compare_one(a,&cases)){ok=false;break;}
 rom_fault_addr=saved_fault;
 FILE *f=fopen("revision-r17-crash-y/rom32-exhaustive.json","wt");
 if(f){fprintf(f,"{\n \"passed\":%s,\n \"starts_checked\":%lu,\n \"first_fault_semantics_checked\":true,\n \"scope\":\"synthetic aligned/unaligned words, every mapped segment, traffic range, boundaries, holes and straddles\"\n}\n",ok?"true":"false",(unsigned long)cases);fclose(f);}
 return ok?0:41;
}
static int fixture_crash_y_equivalence(){
 uint8_t *p=fixture_read("..\\PHYS.BIN",16033),*g=fixture_read("..\\..\\behavior-v014-320\\GEO00.BIN",52512);
 if(!p||!g)return 81;runtime_set_host_geometry(p,g);runtime_ipc.course=0;oroad.road_p0=16;if(!runtime_bind_geometry())return 83;
 uint32_t comparisons=0;uint16_t positions=0;bool passed=true;
 for(uint8_t split=0;split<2;split++){
  runtime_ipc.split=split;
  for(uint16_t i=0;i<=64;i++){
   if(!split)oroad.road_pos=((uint32_t)i*1948UL/64UL)<<16;
   else oroad.road_pos=((uint32_t)i*256UL/64UL)<<16;
   runtime_fill_crash_road_y_reference();++positions;
   for(uint16_t z=0;z<512;z++){
    if(runtime_crash_road_y(z)!=oroad.road_y[oroad.road_p0+z]){passed=false;break;}
    ++comparisons;
   }
   if(!passed)break;
  }
  if(!passed)break;
 }
 FILE *f=fopen("revision-r17-crash-y/crash-y-equivalence.json","wt");
 if(f){fprintf(f,"{\n \"passed\":%s,\n \"road_positions\":%u,\n \"z_comparisons\":%lu,\n \"z_range\":\"0..511\",\n \"modes\":[\"presplit course 0\",\"split course 0\"],\n \"reference\":\"R15 full 512-depth road_y fill\"\n}\n",passed?"true":"false",positions,(unsigned long)comparisons);fclose(f);}
 free(p);free(g);return passed?0:82;
}
int main(int argc,char **argv){
 if(argc>1 && strcmp(argv[1],"--crash-y-equivalence")==0)return fixture_crash_y_equivalence();
 if(argc>1 && strcmp(argv[1],"--rom32-exhaustive")==0)return rom32_exhaustive_test();
 if(argc>1 && strcmp(argv[1],"--traffic-contract")==0)return fixture_traffic_contract();
 if(argc>1 && strcmp(argv[1],"--gear-smoke")==0)return runtime_fixture_result(fixture_gear_smoke(),"revision-r17-crash-y/r7-projection-stats-gear.json");
 if(argc>1 && strcmp(argv[1],"--split")==0)return runtime_fixture_result(fixture_split_run(false,false),"revision-r17-crash-y/r7-projection-stats-split.json");
 if(argc>1 && strcmp(argv[1],"--split-v4")==0)return runtime_fixture_result(fixture_split_run(false,true),"revision-r17-crash-y/r7-projection-stats-split-v4.json");
 if(argc>1 && strcmp(argv[1],"--split-v5")==0)return runtime_fixture_result(fixture_split_run(true,true),"revision-r17-crash-y/r7-projection-stats-split-v5.json");
 if(argc>1 && strcmp(argv[1],"--equiv120-v4")==0)return runtime_fixture_result(fixture_equiv_120(false),"revision-r17-crash-y/r7-projection-stats-120-v4.json");
 if(argc>1 && strcmp(argv[1],"--equiv120-v5")==0)return runtime_fixture_result(fixture_equiv_120(true),"revision-r17-crash-y/r7-projection-stats-120-v5.json");
 if(argc>1 && strcmp(argv[1],"--long")==0)return runtime_fixture_result(fixture_long_run(),"revision-r17-crash-y/r7-projection-stats-long.json");
 uint8_t *p=fixture_read("..\\PHYS.BIN",16033);
 uint8_t *g=fixture_read("..\\..\\behavior-v014-320\\GEO00.BIN",52512);
 if(!p||!g)return 2;runtime_set_host_geometry(p,g);
 runtime_ipc.course=0;runtime_ipc.stage=0;runtime_ipc.time_left=60;runtime_ipc.split=0;
 runtime_ipc.command=0;runtime_entry();
 if(runtime_ipc.status || runtime_ipc.source_x!=450)return 3;
 // Synthetic fixture only: representative traffic properties, width and Z
 // exercise the exact source methods; these are not calibrated world spawns.
 for(uint8_t i=0;i<3;i++){
  RuntimeObject &o=runtime_ipc.objects[5+i];o.active=1;o.kind=2;o.type=(uint16_t)(i==0?12:(i==1?16:5))<<3;
  o.width=16;o.x=0;o.y=200;o.traffic_speed=100;o.z=0x1d8L<<16;
 }
 FILE *f=fopen("revision-r17-crash-y/runtime_host_trace.csv","wt");if(!f)return 4;
 if(sizeof(runtime_ipc)!=372)return 7;
 FILE *snap=fopen("revision-r17-crash-y/runtime_host_snapshots.bin","wb");if(!snap)return 8;
 fprintf(f,"tick,status,speed,increment,source_x,road_pos,source_progress,split,scenery_assigned,scenery_active,road_delta,score,traffic_collisions,crash_state,rom_fault\n");
 for(uint16_t t=0;t<120;t++){
  runtime_ipc.held_keys=(t<90)?Input::ACCEL:Input::BRAKE;runtime_ipc.command=1;runtime_entry();
  if(fwrite(&runtime_ipc,1,sizeof(runtime_ipc),snap)!=sizeof(runtime_ipc)){fclose(snap);fclose(f);return 9;}
  uint8_t scenery_active=0;for(uint8_t i=0;i<5;i++)if(runtime_object_bound[i]&&runtime_ipc.objects[i].kind==1)scenery_active++;
  fprintf(f,"%u,%u,%lu,%lu,%d,%lu,%u,%u,%u,%u,%ld,%08lX,%u,%u,%lu\n",t,runtime_ipc.status,
   (unsigned long)runtime_ipc.source_speed,(unsigned long)runtime_ipc.source_increment,runtime_ipc.source_x,
   (unsigned long)runtime_ipc.source_road_pos,(unsigned)(runtime_ipc.source_road_pos>>16),runtime_ipc.split,(unsigned)runtime_scenery_total,(unsigned)scenery_active,(long)oroad.road_pos_change,(unsigned long)runtime_ipc.score_bcd,
   runtime_ipc.traffic_collision,runtime_ipc.crash_state,(unsigned long)rom_fault_addr);
  if(runtime_ipc.status)break;
 }
 fclose(snap);fclose(f);
 FILE *cr=fopen("revision-r17-crash-y/runtime_crash_fixture.jsonl","wt");if(!cr)return 6;
 bool low=fixture_recovery(g,"low_bump",40,cr);
 bool medium=fixture_recovery(g,"medium_spin",150,cr);
 bool fast=fixture_recovery(g,"fast_flip",260,cr);
 fclose(cr);free(p);free(g);
 return (runtime_ipc.status||!low||!medium||!fast)?5:0;
}
#else
int main(){return 0;}
#endif
