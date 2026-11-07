; PSG adaptations: music owns three tones; engine yields to percussion.
; Brief crash/skid feedback overrides the shared noise voice.
; Engine borrows periodic noise only during quiet percussion. Crash/skid
; borrow white noise. Checkpoint cue briefly borrows tone 2 (speech substitute).
music_second_word dw 0f000h
effect_ticks dw 0
checkpoint_sound dw 0
effect_noise_cache dw 0ffffh
effect_volume_cache dw 0ffffh
effect_engine_ticks dw 0
effect_crash_ticks dw 0
effect_skid_ticks dw 0
effect_checkpoint_ticks dw 0
effects_tick:
 cmp checkpoint_sound,0
 je effects_noise
 dec checkpoint_sound
 inc effect_checkpoint_ticks
 mov ax,144
 cmp checkpoint_sound,4
 jae effects_checkpoint_pitch
 mov ax,108
effects_checkpoint_pitch:
 mov bl,0c0h
 call tone
 mov al,0d6h
 call psg_out
 cmp checkpoint_sound,0
 jne effects_noise
 mov ax,music_second_word
 push ax
 and ax,3ffh
 mov bl,0c0h
 call tone
 pop ax
 mov cl,12
 shr ax,cl
 or al,0d0h
 call psg_out
effects_noise:
 mov bx,15
 mov dx,0e5h
 cmp effect_ticks,0
 je effects_test_skid
 dec effect_ticks
 inc effect_crash_ticks
 mov bx,11
 sub bx,effect_ticks
 shl bx,1
 mov bx,crash_envelope[bx]
 jmp effects_emit_priority
effects_test_skid:
 cmp race_state,2
 jne effects_music_yield
 cmp pause_flag,0
 jne effects_music_yield
 cmp speed,80
 jb effects_test_engine
 cmp offroad,0
 jne effects_skid
 test keys,2
 jz effects_test_engine
 test keys,12
 jz effects_test_engine
effects_skid:
 inc effect_skid_ticks
 mov dx,0e4h
 mov bx,10
 jmp effects_emit_priority
effects_test_engine:
 cmp noise_atten,13
 jb effects_percussion_priority
 inc effect_engine_ticks
 mov dx,0e2h
 mov bx,13
 cmp speed,80
 jb effects_emit
 mov dx,0e1h
 mov bx,12
 cmp speed,170
 jb effects_emit
 mov dx,0e0h
 mov bx,11
 jmp effects_emit
effects_music_yield:
 cmp rbg_enabled,0
 je effects_return
 cmp music_bank,3
 je effects_return
 cmp noise_atten,15
 jae effects_return
effects_percussion_priority:
 cmp rbg_enabled,0
 je effects_music_control_ready
 cmp music_bank,3
 je effects_music_control_ready
 mov ax,rbg_drum_control
 cmp noise_control_last,ax
 je effects_music_control_ready
 call psg_out
effects_music_control_ready:
 mov effect_noise_cache,0ffffh
 mov effect_volume_cache,0ffffh
 ret
effects_emit:
 cmp noise_atten,13
 jb effects_percussion_priority
effects_emit_priority:
 cmp effect_noise_cache,dx
 je effects_volume
 mov effect_noise_cache,dx
 mov ax,dx
 call psg_out
effects_volume:
 mov effect_volume_cache,bx
 mov ax,bx
 or al,0f0h
 call psg_out
effects_return:
 ret

crash_envelope dw 3,3,3,4,4,4,4,5,5,6,6,6
