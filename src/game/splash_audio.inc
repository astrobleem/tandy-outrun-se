; Cold presentation-only audio. IRQ call operands restore before radio/racing.
splash_clock dw 0
splash_pointer dw offset splash_data
splash_active dw 0
splash_done dw 0
splash_writes dw 0
splash_start_tick dw 0
splash_end_tick dw 0
splash_quiet_tick:
 ret
splash_quiet:
 pushf
 cli
 mov word ptr sound_music_call+1,offset splash_quiet_tick-offset sound_music_call-3
 mov word ptr sound_effects_call+1,offset splash_quiet_tick-offset sound_effects_call-3
 mov splash_active,0
 mov splash_done,1
 call mute_psg
 popf
 ret
splash_release:
 pushf
 cli
 mov word ptr sound_music_call+1,offset music_tick-offset sound_music_call-3
 mov word ptr sound_effects_call+1,offset effects_tick-offset sound_effects_call-3
 mov splash_active,0
 popf
 ret
splash_start:
 cmp music,0
 jne splash_begin
 jmp splash_quiet
splash_begin:
 pushf
 cli
 call mute_psg
 mov splash_clock,0
 mov splash_pointer,offset splash_data
 mov splash_active,1
 mov splash_done,0
 mov splash_writes,0
 mov ax,ticks
 mov splash_start_tick,ax
 mov word ptr sound_music_call+1,offset splash_tick-offset sound_music_call-3
 mov word ptr sound_effects_call+1,offset splash_quiet_tick-offset sound_effects_call-3
 call splash_emit
 popf
 ret
splash_tick:
 inc splash_clock
 call splash_emit
 cmp splash_clock,SPLASH_END_TICKS
 jb splash_return
 mov ax,ticks
 mov splash_end_tick,ax
 call splash_quiet
splash_return:
 ret
splash_emit:
 mov si,splash_pointer
splash_next:
 mov ax,[si]
 cmp ax,splash_clock
 ja splash_store
 add si,2
 xor cx,cx
 lodsb
 mov cl,al
splash_bytes:
 lodsb
 call psg_out
 inc splash_writes
 loop splash_bytes
 jmp splash_next
splash_store:
 mov splash_pointer,si
 ret
include SPLTUNE.INC
