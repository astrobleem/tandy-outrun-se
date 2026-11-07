; RBG5 radio: original compiled periods, all lead gates and drum attacks.
; Clock is PIT input cycles, paused exactly when music_tick is not called.
rbg_enabled dw 0
rbg_clock_lo dw 0
rbg_clock_hi dw 0
rbg_state_ptr dw 44
rbg_state_left dw 0
rbg_lead_ptr dw 0
rbg_lead_left dw 0
rbg_drum_ptr dw 0
rbg_drum_left dw 0
rbg_lead_off dw 0ffffh
rbg_drum_tick dw 0
rbg_drum_length dw 0
rbg_drum_base dw 15
rbg_drum_control dw 0e4h
rbg_drum_level dw 0
rbg_drums_emitted dw 0
rbg_leads_emitted dw 0
rbg_loops dw 0
rbg_end_pending dw 0
rbg_controls db 2,5,4,4,4,1,0
rbg_bases db 0,1,5,4,2,1,1
rbg_lengths dw 560,840,230,1400,4200,1160,930
rbg_reset:
 mov rbg_clock_lo,0
 mov rbg_clock_hi,0
 mov rbg_end_pending,0
 mov rbg_lead_off,0ffffh
 mov rbg_drum_length,0
 mov noise_atten,15
 push ds
 mov ds,segments[MUSIC_SEGMENT_BASE]
 mov ax,word ptr ds:[8]
 mov cs:rbg_state_left,ax
 mov bx,12
 mul bx
 add ax,44
 mov cs:rbg_lead_ptr,ax
 mov ax,word ptr ds:[10]
 mov cs:rbg_lead_left,ax
 mov bx,10
 mul bx
 add ax,cs:rbg_lead_ptr
 push ax
 mov ax,word ptr ds:[12]
 mov bx,12
 mul bx
 pop bx
 add ax,bx
 mov cs:rbg_drum_ptr,ax
 mov ax,word ptr ds:[40]
 mov cs:rbg_drum_left,ax
 mov cs:rbg_state_ptr,44
 pop ds
 call mute_psg
 ret
rbg_tick:
 push ds
 mov ds,segments[MUSIC_SEGMENT_BASE]
 cmp cs:rbg_end_pending,0
 je rbg_time_ready
 pop ds
 inc rbg_loops
 call rbg_reset
 push ds
 mov ds,segments[MUSIC_SEGMENT_BASE]
rbg_time_ready:
 mov ax,cs:rbg_clock_hi
 cmp ax,word ptr ds:[6]
 jb rbg_not_at_end
 ; Emit the authored terminal mute state/gates before the loop reset.
 mov cs:rbg_end_pending,1
rbg_not_at_end:
 cmp cs:rbg_lead_off,0ffffh
 je rbg_states
 mov ax,cs:rbg_clock_hi
 cmp ax,cs:rbg_lead_off
 jb rbg_states
 mov al,9fh
 call psg_out
 mov cs:rbg_lead_off,0ffffh
rbg_states:
 cmp cs:rbg_state_left,0
 je rbg_leads
 mov si,cs:rbg_state_ptr
 mov ax,[si]
 cmp ax,cs:rbg_clock_hi
 ja rbg_leads
 mov ax,[si+4]
 mov bl,[si+9]
 call rbg_pack
 mov bl,0a0h
 call rbg_voice
 mov ax,[si+6]
 mov bl,[si+10]
 call rbg_pack
 mov cs:music_second_word,ax
 mov bl,0c0h
 call rbg_voice
 add cs:rbg_state_ptr,12
 dec cs:rbg_state_left
 jmp rbg_states
rbg_leads:
 cmp cs:rbg_lead_left,0
 je rbg_drums
 mov si,cs:rbg_lead_ptr
 mov ax,[si]
 cmp ax,cs:rbg_clock_hi
 ja rbg_drums
 ; Every onset rearticulates, including repeated equal pitches.
 mov al,9fh
 call psg_out
 mov ax,[si+2]
 mov cs:rbg_lead_off,ax
 mov ax,[si+4]
 mov bl,[si+7]
 call rbg_pack
 mov bl,80h
 call rbg_voice
 ; A very short interval can begin and end within one native IRQ.
 mov ax,cs:rbg_clock_hi
 cmp ax,cs:rbg_lead_off
 jb rbg_lead_active
 mov al,9fh
 call psg_out
 mov cs:rbg_lead_off,0ffffh
rbg_lead_active:
 inc cs:rbg_leads_emitted
 add cs:rbg_lead_ptr,10
 dec cs:rbg_lead_left
 jmp rbg_leads
rbg_drums:
 cmp cs:rbg_drum_left,0
 je rbg_envelope
 mov si,cs:rbg_drum_ptr
 mov ax,[si]
 cmp ax,cs:rbg_clock_hi
 ja rbg_envelope
 mov cs:rbg_drum_tick,ax
 xor bx,bx
 mov bl,[si+2]
 mov al,cs:rbg_controls[bx]
 or al,0e0h
 xor ah,ah
 mov cs:rbg_drum_control,ax
 call psg_out
 xor ax,ax
 mov al,cs:rbg_bases[bx]
 mov cs:rbg_drum_base,ax
 xor ax,ax
 mov al,[si+3]
 mov cs:rbg_drum_level,ax
 shl bx,1
 mov ax,cs:rbg_lengths[bx]
 mov cs:rbg_drum_length,ax
 ; Preserve each attack even when multiple authored hits quantize together.
 mov ax,cs:rbg_drum_base
 add ax,cs:rbg_drum_level
 cmp ax,15
 jbe rbg_attack_bounded
 mov ax,15
rbg_attack_bounded:
 or al,0f0h
 call psg_out
 mov cs:effect_noise_cache,0ffffh
 inc cs:rbg_drums_emitted
 add cs:rbg_drum_ptr,4
 dec cs:rbg_drum_left
 jmp rbg_drums
rbg_envelope:
 mov bx,cs:rbg_drum_length
 or bx,bx
 jz rbg_envelope_silent
 mov ax,cs:rbg_clock_hi
 sub ax,cs:rbg_drum_tick
 cmp ax,17
 jae rbg_envelope_silent
 mov ah,al
 mov al,byte ptr cs:rbg_clock_lo+1
 cmp ax,bx
 jae rbg_envelope_silent
 mov cx,15
 sub cx,cs:rbg_drum_base
 mul cx
 div bx
 add ax,cs:rbg_drum_base
 add ax,cs:rbg_drum_level
 cmp ax,15
 jbe rbg_envelope_emit
rbg_envelope_silent:
 mov ax,15
rbg_envelope_emit:
 mov cs:noise_atten,ax
 ; effects_tick decides the final noise owner after evaluating crash/skid.
 or al,0f0h
 call psg_out
 add cs:rbg_clock_lo,18643
 adc cs:rbg_clock_hi,0
 pop ds
 ret
rbg_pack:
 ; AX period, BL attenuation -> the existing packed tone representation.
 cmp ax,0
 jne rbg_pack_on
 mov bl,15
rbg_pack_on:
 xor bh,bh
 mov cl,12
 shl bx,cl
 or ax,bx
 ret
rbg_voice:
 push ax
 and ax,3ffh
 call tone
 pop ax
 mov cl,12
 shr ax,cl
 or al,bl
 or al,10h
 call psg_out
 ret
