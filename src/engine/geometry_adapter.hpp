// Cached-road boundary for the native presentation. Authored adapter code.
// It consumes verified source samples; it is not a replacement road CPU.
#ifndef OUTRUN_GEOMETRY_ADAPTER_HPP
#define OUTRUN_GEOMETRY_ADAPTER_HPP
#include <stdint.h>
#if defined(__I86__)
extern "C" int32_t native_mul_s16_s16(int16_t a,int16_t b);
#pragma aux native_mul_s16_s16 parm [ax] [bx] value [dx ax] modify exact [ax dx]
#else
inline int32_t native_mul_s16_s16(int16_t a,int16_t b) { return (int32_t)a*(int32_t)b; }
#endif
#if defined(__I86__)
typedef const uint8_t __far *GeometryBytes;
#else
typedef const uint8_t *GeometryBytes;
#endif
namespace native_geo {
// Exact signed 16x16->32 multiply. 8086 IMUL returns DX:AX; the Watcom
// declaration binds parameters to AX/BX and the long result to DX:AX.
#if defined(__I86__)
inline int32_t mul_s16_s16(int16_t a,int16_t b) { return native_mul_s16_s16(a,b); }
#else
inline int32_t mul_s16_s16(int16_t a,int16_t b) { return native_mul_s16_s16(a,b); }
#endif
#if defined(__I86__)
inline uint16_t le16(GeometryBytes p,uint16_t a) { return *(const uint16_t __far *)(p+a); }
#else
inline uint16_t le16(GeometryBytes p,uint16_t a) { return p[a]|((uint16_t)p[a+1]<<8); }
#endif
inline int16_t be16(GeometryBytes p,uint16_t a) { return (int16_t)(((uint16_t)p[a]<<8)|p[a+1]); }
struct RoadSample { uint16_t width,flags; int16_t road_x170,road_x511; };
struct ProjectionSample {int16_t road_x,road_y,road0_h,road1_h,native_y;};
struct CurveState { uint16_t cursor,course,split; int16_t curve,type,next_curve,next_type; };
// Low 16 bits of (p>>8) and (p>>9) from the two words, without a 32-bit shift loop.
inline uint16_t shr8_low16(int32_t p) { return (uint16_t)(((uint16_t)((uint32_t)p>>16)<<8)|((uint16_t)p>>8)); }
inline uint16_t shr9_low16(int32_t p) { return (uint16_t)(((uint16_t)((uint32_t)p>>16)<<7)|((uint16_t)p>>9)); }
inline int16_t interpolate(int16_t a,int16_t b,uint16_t fraction) {
    int16_t delta=(int16_t)(b-a);
    // Callers pass a low byte (0..255) or the terminal sample 256.
    // delta*fraction stays in signed 32-bit range, including -32768*256.
    int32_t product=fraction<=256 ? mul_s16_s16(delta,(int16_t)fraction)
                                  : (int32_t)delta*(int32_t)fraction;
    return (int16_t)((uint16_t)a+shr8_low16(product));
}
inline bool validate(GeometryBytes p) {
    return p[0]=='P' && p[1]=='H' && p[2]=='Y' && p[3]=='1' && ((p[4]==4 && p[5]==15)||(p[4]==5 && p[5]==1 && le16(p,28)<15)) && p[6]==19 && p[7]==18 && p[8]==64 && p[9]==6 && le16(p,22)<16384;
}
inline uint16_t directory_offset(GeometryBytes p,uint16_t course){return le16(p,14)+(p[4]==5?0:course*18);}
inline uint16_t split_curve_offset(GeometryBytes p) {return p[4]==5?le16(p,24):le16(p,20)+le16(p,12)*6+15*65*4;}
inline uint16_t split_curve_count(GeometryBytes p) {
    uint16_t a=split_curve_offset(p),count=0;
    while(count<64 && be16(p,a+count*6)!=-1)++count;
    return count;
}
inline uint16_t spawn_word(GeometryBytes p,uint16_t course,uint16_t slot) {
    if(p[4]==5)return le16(p,le16(p,26)+slot*2);
    uint16_t start=split_curve_offset(p)+split_curve_count(p)*6+2;
    return le16(p,start+(course*64+slot)*2);
}
inline uint16_t descriptor_offset(GeometryBytes p,uint16_t course,uint16_t slot) {
    uint16_t dir=directory_offset(p,course);
    uint16_t id=p[le16(p,18)+le16(p,dir+12)+slot];
    return id==255?0xffff:le16(p,16)+(le16(p,dir+10)+id)*p[6];
}
inline RoadSample sample(GeometryBytes p,GeometryBytes geo,uint16_t course,uint16_t distance) {
    RoadSample out;
    uint16_t index=distance>>8, fraction=distance&255;
    if(index>=64) {index=63;fraction=256;}
    uint16_t a=16+index*4;
    out.width=(uint16_t)interpolate((int16_t)le16(geo,a),(int16_t)le16(geo,a+4),fraction);
    out.flags=geo[276+index];
    uint16_t dir=directory_offset(p,course);
    uint16_t taps=le16(p,20)+le16(p,12)*6+le16(p,dir+16)*4+index*4;
    out.road_x170=interpolate((int16_t)le16(p,taps),(int16_t)le16(p,taps+4),fraction);
    out.road_x511=interpolate((int16_t)le16(p,taps+2),(int16_t)le16(p,taps+6),fraction);
    return out;
}
// This is the same sampled projection domain used by the retained renderer,
// not a live road CPU. Sample individual depths needed by source object code
// rather than clearing/rebuilding 512 unused road_x values every logic tick.
inline ProjectionSample projection(GeometryBytes geo,uint16_t distance,uint16_t z,int16_t car_x,uint16_t width,uint16_t split) {
    uint16_t index=distance>>8,fraction=distance&255;
    if(index>=64){index=63;fraction=256;}
    if(z>511)z=511;
    uint16_t a=512+index*800,b=a+800;
    uint16_t ypos=672+(z>>2);
    int16_t ny=interpolate(geo[a+ypos],geo[b+ypos],fraction);
    uint16_t row=ny<16?0:ny>=184?167:(uint16_t)(ny-16);
    int16_t x=interpolate((int16_t)le16(geo,a+row*4),(int16_t)le16(geo,b+row*4),fraction);
    int16_t arcade_y=(int16_t)(((uint16_t)ny*224U+100U)/200U+18U);
    ProjectionSample out;
    out.native_y=ny;out.road_x=x;out.road_y=(int16_t)((223L-arcade_y)*16L);
    int32_t left=(int32_t)car_x-(int32_t)width;
    int32_t right=(int32_t)car_x+(int32_t)width;
    int32_t left_product=(left>=-32768L && left<=32767L && z<=32767U)
        ? mul_s16_s16((int16_t)left,(int16_t)z) : left*(int32_t)z;
    int32_t right_product=(right>=-32768L && right<=32767L && z<=32767U)
        ? mul_s16_s16((int16_t)right,(int16_t)z) : right*(int32_t)z;
    out.road0_h=(int16_t)((int16_t)shr9_low16(left_product)+(x>>6));
    out.road1_h=(int16_t)((int16_t)shr9_low16(right_product)+(split?-(x>>6):(x>>6)));
    return out;
}
inline void reset_curve(CurveState &s) {
    s.cursor=0;s.course=0xffff;s.split=0xffff;s.curve=0;s.type=0;s.next_curve=0;s.next_type=0;
}
inline void update_curve(CurveState &s,GeometryBytes p,uint16_t course,uint16_t source_position,uint16_t split) {
    if(s.course!=course || s.split!=split) {s.cursor=0;s.course=course;s.split=split;}
    uint16_t start,count;
    if(split) {
        start=split_curve_offset(p);count=split_curve_count(p);
    } else {
        uint16_t dir=directory_offset(p,course);
        start=le16(p,20)+le16(p,dir+14)*6;count=p[dir+8];
    }
    if(s.cursor>=count) return;
    uint16_t record=start+s.cursor*6;
    int16_t position=be16(p,record);
    // Exact source update_road curve selection order: anticipate at -60,
    // then consume at the boundary and clear anticipation on that tick.
    if((int16_t)(position-0x3c)<=(int16_t)source_position) {
        s.next_curve=be16(p,record+2);s.next_type=be16(p,record+4);
    }
    if(position<=(int16_t)source_position) {
        s.curve=be16(p,record+2);s.type=be16(p,record+4);
        ++s.cursor;s.next_curve=0;s.next_type=0;
    }
}
inline uint32_t native_distance_fixed_uncached(uint32_t source_position,uint16_t course,uint16_t split) {
    if(split) return (14336UL<<16)+(source_position<<3);
    // Source road reset discards overshoot on the following logic tick. Keep
    // that canonical state intact while bounding its native course interval;
    // otherwise the last presplit sample can exceed the first split sample.
    if(source_position>(1948UL<<16))source_position=1948UL<<16;
    uint16_t numerator=course<10?14336:16384;
    return (source_position/1948UL)*numerator+((source_position%1948UL)*numerator)/1948UL;
}
inline uint32_t native_distance_fixed(uint32_t source_position,uint16_t course,uint16_t split) {
    // Pure result: full input key, including pre/post-movement position changes.
    // No geometry pointers or mutable game state participate in this conversion.
    static uint32_t cached_position=0,cached_result=0;
    static uint16_t cached_course=0,cached_split=0;
    static uint8_t valid=0;
    if(valid && cached_position==source_position && cached_course==course && cached_split==split)
        return cached_result;
    uint32_t result=native_distance_fixed_uncached(source_position,course,split);
    cached_position=source_position;cached_course=course;cached_split=split;
    cached_result=result;valid=1;return result;
}

}
#endif
