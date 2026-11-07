// Scalar layout transcribed from pinned OInputs header; no host SDL dependency.
#ifndef KERNEL_INPUTS_HPP
#define KERNEL_INPUTS_HPP
#include <stdint.h>
struct Input {
 enum { ACCEL=1,BRAKE=2,LEFT=4,RIGHT=8 };
 uint16_t held;
 bool is_pressed(uint16_t key) const { return (held & key)!=0; }
};
extern Input input;
struct OInputs {
 enum { BRAKE_THRESHOLD1=0x80,BRAKE_THRESHOLD2=0xa0,BRAKE_THRESHOLD3=0xc0,BRAKE_THRESHOLD4=0xe0,
 STEERING_MIN=0x48,STEERING_MAX=0xb8,STEERING_CENTRE=0x80,PEDAL_MIN=0x30,PEDAL_MAX=0x90 };
 int8_t crash_input;
 int16_t input_acc,input_steering,steering_adjust,acc_adjust,brake_adjust;
 bool gear;
 uint8_t steering_inc,acc_inc,brake_inc;
 int16_t steering_old,steering_change,input_brake;
 void digital_steering(); void digital_pedals(); void adjust_inputs();
};
#endif
