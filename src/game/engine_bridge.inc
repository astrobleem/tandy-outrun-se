; opt8-lazy6-s1k bank ABI; checked by tools/build_engine.py.
; Caller must serialize calls. Source kernel permits nested timer/keyboard IRQs; busy blocks kernel re-entry.
ENGINE_SEGMENT equ 38
ENGINE_BYTES equ 37456
ENGINE_ENTRY equ 22462
ENGINE_IPC equ 26048
ENGINE_IPC_BYTES equ 372
ENGINE_STACK_TOP equ 37440
IPC_MAGIC equ 0
IPC_VERSION equ 4
IPC_COMMAND equ 6
IPC_STATUS equ 8
IPC_BYTES equ 10
IPC_HELD_KEYS equ 12
IPC_GEAR_MODE equ 14
IPC_GEAR_VALUE equ 15
IPC_TIME_LEFT equ 16
IPC_COURSE equ 18
IPC_STAGE equ 20
IPC_SPLIT equ 22
IPC_PHYS_SEG equ 24
IPC_GEO_SEG equ 26
IPC_RNG_VALUE equ 28
IPC_SOURCE_INCREMENT equ 32
IPC_SOURCE_SPEED equ 36
IPC_SOURCE_X equ 40
IPC_NATIVE_X equ 42
IPC_SOURCE_ROAD_POS equ 44
IPC_NATIVE_DISTANCE equ 48
IPC_ROAD_WIDTH equ 52
IPC_WHEEL_STATE equ 54
IPC_CRASH_STATE equ 55
IPC_CRASH_COUNTER equ 56
IPC_CRASH_TYPE equ 57
IPC_CAR_CTRL_ACTIVE equ 58
IPC_CAR_STATE equ 59
IPC_SPIN_CONTROL1 equ 60
IPC_SPIN_CONTROL2 equ 61
IPC_COLLISION_COUNT equ 62
IPC_SKID_COUNTER equ 64
IPC_TRAFFIC_COLLISION equ 66
IPC_TRAFFIC_MASK equ 68
IPC_TRAFFIC_SPEED_TOTAL equ 70
IPC_SCORE_BCD equ 72
IPC_OBJECT_COUNT equ 76
IPC_OBJECTS equ 78
IPC_ACTORS equ 270
IPC_OBJECT_BYTES equ 24
IPC_ACTOR_BYTES equ 17
engine_call_ptr dw ENGINE_ENTRY,0
engine_saved_ss dw 0
engine_saved_sp dw 0
engine_phase dw 0
engine_fault dw 0
engine_snapshot db ENGINE_IPC_BYTES dup(0)

; Preserve every caller register, segment and flags, including its IF state.
; PUSHes remain on the caller stack until the source bank has returned.
engine_call proc near
 pushf
 cli
 push ax
 push bx
 push cx
 push dx
 push si
 push di
 push bp
 push ds
 push es
 mov ax,ss
 mov cs:engine_saved_ss,ax
 mov cs:engine_saved_sp,sp
 mov ax,cs:segments[ENGINE_SEGMENT]
 mov cs:engine_call_ptr+2,ax
 mov ds,ax
 mov es,ax
 mov ss,ax
 mov sp,ENGINE_STACK_TOP
 mov word ptr cs:engine_busy,1
 ; Audio was serviced before physics. Permit nested timer/keyboard service
 ; while the non-reentrant kernel owns its separate stack and data segment.
 mov al,20h
 out 20h,al
 sti
 call dword ptr cs:engine_call_ptr
 cli
 mov word ptr cs:engine_busy,0
 ; Kernel DS/ES may be scratch after a future compiler rebuild: reload DS.
 mov ax,cs:segments[ENGINE_SEGMENT]
 mov ds,ax
 mov ax,word ptr ds:[ENGINE_IPC+IPC_STATUS]
 mov cs:engine_fault,ax
 mov ax,cs:engine_saved_ss
 mov ss,ax
 mov sp,cs:engine_saved_sp
 pop es
 pop ds
 pop bp
 pop di
 pop si
 pop dx
 pop cx
 pop bx
 pop ax
 popf
 ret
engine_call endp

; AL=command. This helper does not execute the bank.
engine_command proc near
 push es
 push bx
 mov bx,cs:segments[ENGINE_SEGMENT]
 mov es,bx
 xor ah,ah
 mov word ptr es:[ENGINE_IPC+IPC_COMMAND],ax
 pop bx
 pop es
 ret
engine_command endp

; Caller invokes only for a running, unpaused 64 Hz game tick. Carry means
; exactly one source logic tick is due. No catch-up accumulation while paused.
engine_cadence proc near
 add word ptr cs:engine_phase,30
 cmp word ptr cs:engine_phase,64
 jb engine_not_due
 sub word ptr cs:engine_phase,64
 stc
 ret
engine_not_due:
 clc
 ret
engine_cadence endp

; Frame snapshot is atomic with respect to IRQ source-bank updates.
engine_copy_snapshot proc near
 pushf
 cli
 push ax
 push cx
 push si
 push di
 push ds
 push es
 mov ax,cs:segments[ENGINE_SEGMENT]
 mov ds,ax
 mov ax,cs
 mov es,ax
 mov si,ENGINE_IPC
 mov di,offset engine_snapshot
 mov cx,ENGINE_IPC_BYTES/2
 cld
 rep movsw
 pop es
 pop ds
 pop di
 pop si
 pop cx
 pop ax
 popf
 ret
engine_copy_snapshot endp
