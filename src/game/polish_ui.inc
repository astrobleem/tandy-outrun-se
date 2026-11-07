; Full-screen presentation runs only while race_state >= 8.
ui_key dw 0
ui_last dw 0ffffh
ui_started dw 0
ui_return dw 2
ui_selected dw 0
ui_reads dw 0
ui_read_violations dw 0
ui_filename dw offset ui_sega
ui_size dw 0
ui_copy_words dw 0
ui_sega db 'UISGA.BIN',0
ui_title db 'UITITLE.BIN',0
ui_radio db 'UIRAD0.BIN',0
ui_check db 'UICHECK.BIN',0
ui_finish db 'UIFIN.BIN',0
ui_goal db 'UIGOAL.BIN',0
ui_route db 'GOAL A',0
ui_over db 'UIOVER.BIN',0
ui_sky db 'SKYHUD.BIN',0
song_names dw offset song0,offset song1,offset song2
song0 db 'MUSIC0.BIN',0
song1 db 'MUSIC1.BIN',0
song2 db 'MUSIC2.BIN',0
; Expected exact lengths and Fletcher16 values are generated with the assets.
include RADIOCAT.INC
ui_frame:
 mov ax,race_state
 cmp ax,ui_last
 je ui_input
 mov ui_last,ax
 mov ax,ticks
 mov ui_started,ax
 mov ax,offset ui_sega
 cmp race_state,8
 je ui_enter
 mov ax,offset ui_title
 cmp race_state,9
 je ui_enter
 mov ax,offset ui_radio
 cmp race_state,10
 je ui_enter
 mov ax,offset ui_check
 cmp race_state,11
 je ui_enter
 mov ax,offset ui_goal
 cmp race_state,12
 je ui_enter
 mov ax,offset ui_finish
 cmp race_state,14
 je ui_enter
 mov ax,offset ui_over
ui_enter:
 mov ui_filename,ax
 call ui_show
 cmp race_state,8
 jne ui_input
 call splash_start
ui_input:
 cli
 mov bx,ui_key
 mov ui_key,0
 sti
 cmp race_state,8
 jne ui_not_splash
 cmp bx,1
 jne ui_splash_key
 call splash_quiet
 mov exit_flag,1
 ret
ui_splash_key:
 or bx,bx
 jnz ui_to_title
 cmp splash_done,0
 je ui_done
ui_to_title:
 call splash_quiet
 mov race_state,9
 ret
ui_not_splash:
 cmp race_state,9
 jne ui_not_title
 cmp bx,1
 jne ui_title_start
 mov exit_flag,1
 ret
ui_title_start:
 cmp bx,1ch
 je ui_to_radio
 cmp bx,39h
 jne ui_done
ui_to_radio:
 call ui_load_song
 mov race_state,10
 ret
ui_not_title:
 cmp race_state,10
 jne ui_not_radio
 cmp bx,1
 je ui_to_title
 cmp bx,1ch
 je ui_start_race
 mov ax,ui_selected
 cmp bx,48h
 je ui_prev
 cmp bx,50h
 je ui_next
 cmp bx,2
 jb ui_done
 cmp bx,4
 ja ui_done
 mov ax,bx
 sub ax,2
 jmp ui_select
ui_prev:
 dec ax
 jge ui_select
 mov ax,2
 jmp ui_select
ui_next:
 inc ax
 cmp ax,3
 jb ui_select
 xor ax,ax
ui_select:
 cmp ax,ui_selected
 je ui_done
 mov ui_selected,ax
 add al,'0'
 mov ui_radio[5],al
 call ui_load_song
 mov ui_last,0ffffh
 ret
ui_not_radio:
 cmp race_state,11
 je ui_transition
 cmp race_state,12
 je ui_transition
 cmp bx,1
 jne ui_end_restart
 mov exit_flag,1
 ret
ui_end_restart:
 cmp bx,13h
 je ui_to_radio
 cmp bx,1ch
 je ui_to_radio
 ret
ui_transition:
 or bx,bx
 jnz ui_resume
 mov ax,ticks
 sub ax,ui_started
 cmp ax,64
 jb ui_done
ui_resume:
 call ui_restore
 mov ax,ui_return
 mov race_state,ax
ui_done:
 ret
ui_start_race:
 mov ax,ui_selected
 mov radio_track,ax
 call reset_race
 call ui_restore
 ret
ui_load_song:
 mov engine_initialized,0
 ; Load and checksum in staging before replacing the active song.
 mov bx,ui_selected
 shl bx,1
 mov dx,song_names[bx]
 mov cx,song_sizes[bx]
 mov ui_size,cx
 call ui_read
 jc ui_song_failed
 push ds
 mov ds,segments[MUSIC_SEGMENT_BASE+2]
 xor si,si
 xor bx,bx
 xor dx,dx
 mov cx,cs:ui_size
ui_song_sum:
 lodsb
 xor ah,ah
 add bx,ax
 cmp bx,255
 jb ui_song_sum_bounded
 sub bx,255
ui_song_sum_bounded:
 add dx,bx
 cmp dx,255
 jb ui_song_sum_second
 sub dx,255
ui_song_sum_second:
 loop ui_song_sum
 mov ah,dl
 mov al,bl
 pop ds
 mov bx,ui_selected
 shl bx,1
 cmp ax,song_checksums[bx]
 jne ui_song_failed
 ; A short atomic flag update stops the IRQ from seeing a half-copied bank.
 mov ax,music
 push ax
 mov music,0
 call mute_psg
 push ds
 mov ax,segments[MUSIC_SEGMENT_BASE]
 mov es,ax
 mov ds,segments[MUSIC_SEGMENT_BASE+2]
 xor si,si
 xor di,di
 mov cx,cs:ui_size
 inc cx
 shr cx,1
 call ui_copy
 pop ds
 mov music_bank,0
 mov music_position,4
 mov music_wait,0
 mov noise_atten,15
 mov effect_noise_cache,0ffffh
 mov effect_volume_cache,0ffffh
 mov rbg_enabled,0
 cmp ui_selected,3
 jb ui_song_ready
 mov rbg_enabled,1
 mov rbg_loops,0
 call rbg_reset
ui_song_ready:
 call splash_release
 pop ax
 mov music,ax
 ret
ui_song_failed:
 ; Keep the previous complete active bank. Show a clear recoverable error.
 mov ui_last,0ffffh
 mov exit_flag,1
 mov finish_reason,3
 ret
ui_read:
 cmp race_state,2
 jne ui_read_safe
 inc ui_read_violations
ui_read_safe:
 mov ui_size,cx
 mov ax,3d00h
 int 21h
 jc ui_read_failed
 mov load_handle,ax
 push ds
 mov ds,segments[MUSIC_SEGMENT_BASE+2]
 xor dx,dx
 mov bx,cs:load_handle
 mov cx,cs:ui_size
 mov ah,3fh
 int 21h
 pop ds
 pushf
 push ax
 mov bx,load_handle
 mov ah,3eh
 int 21h
 pop ax
 popf
 jc ui_read_failed
 cmp ax,ui_size
 jne ui_read_failed
 inc ui_reads
 clc
 ret
ui_read_failed:
 stc
 ret
ui_read_video:
 cmp race_state,2
 jne ui_read_video_safe
 inc ui_read_violations
ui_read_video_safe:
 mov ui_size,cx
 mov ax,3d00h
 int 21h
 jc ui_read_video_failed
 mov load_handle,ax
 push ds
 mov ax,0b800h
 mov ds,ax
 xor dx,dx
 mov bx,cs:load_handle
 mov cx,cs:ui_size
 mov ah,3fh
 int 21h
 pop ds
 pushf
 push ax
 mov bx,load_handle
 mov ah,3eh
 int 21h
 pop ax
 popf
 jc ui_read_video_failed
 cmp ax,ui_size
 jne ui_read_video_failed
 inc ui_reads
 clc
 ret
ui_read_video_failed:
 stc
 ret
ui_copy:
 ; Bounded REP chunks avoid the known local 8088 emulator IRQ restart defect.
 jcxz ui_copy_done
 mov bx,cx
 cmp cx,64
 jbe ui_copy_chunk
 mov cx,64
ui_copy_chunk:
 sub bx,cx
 pushf
 cli
 rep movsw
 popf
 mov cx,bx
 jmp ui_copy
ui_copy_done:
 ret
ui_show:
 mov ax,0b800h
 mov es,ax
 xor di,di
 xor ax,ax
 mov cx,UI_BYTES/2
ui_black_loop:
 stosw
 loop ui_black_loop
 call present
 mov dx,ui_filename
 mov cx,UI_BYTES
 call ui_read_video
 jc ui_screen_failed
 mov ax,0b800h
 mov es,ax
 ; Final cards include the actual achieved score and route.
 cmp race_state,14
 je ui_results
 cmp race_state,13
 jne ui_show_present
ui_results:
 cmp race_state,14
 jne ui_results_score
 mov ax,route_index
 add al,'A'
 mov ui_route[5],al
 mov si,offset ui_route
 mov hud_y,136
 mov hud_x,UI_SCORE_X
 call hud_text
ui_results_score:
 mov hud_y,161
 if UI_BYTES eq 32768
 mov hud_x,44
 else
 mov hud_x,21
 endif
 call engine_hud_score
 if UI_BYTES eq 32768
 mov hud_x,108
 else
 mov hud_x,53
 endif
 mov hud_digit_count,3
 mov ax,race_elapsed
 call hud_number
ui_show_present:
 call present
 ret
ui_screen_failed:
 mov exit_flag,1
 mov finish_reason,3
 ret
ui_restore:
 mov bp,2
ui_restore_page:
 mov dx,offset ui_sky
 mov cx,UI_BYTES
 call ui_read_video
 jc ui_screen_failed
 call present
 dec bp
 jnz ui_restore_page
 mov sky_page_prefix[0],0
 mov sky_page_prefix[2],0
 mov sky_page_uniform[0],0
 mov sky_page_uniform[2],0
 mov sky_page_offset[0],7fffh
 mov sky_page_offset[2],7fffh
 mov sky_dirty_y[0],16
 mov sky_dirty_y[2],16
 mov hud_initialized[0],0
 mov hud_initialized[2],0
 mov hud_cache[0],0ffffh
 mov hud_cache[2],0ffffh
 mov hud_time_cache[0],0ffffh
 mov hud_time_cache[2],0ffffh
 mov hud_stage_cache[0],0ffffh
 mov hud_stage_cache[2],0ffffh
 mov hud_speed_cache[0],0ffffh
 mov hud_speed_cache[2],0ffffh
 push cs
 pop es
 mov di,offset sky_row_tokens
 mov ax,0ffffh
 mov cx,336
 pushf
 cli
 rep stosw
 popf
 mov ui_last,0ffffh
 ret
ui_after_stage:
 cmp race_state,3
 jne ui_stage_resume
 ; Last Wave has no authored noise events; stop the previous race effects.
 mov effect_ticks,0
 mov checkpoint_sound,0
 mov noise_atten,15
 mov effect_noise_cache,0ffffh
 mov effect_volume_cache,0ffffh
 mov al,0ffh
 call psg_out
 mov ui_return,3
 mov race_state,12
 ret
ui_stage_resume:
 cmp race_state,2
 jne ui_after_done
 mov ui_return,2
 mov race_state,11
ui_after_done:
 ret
ui_countdown:
 cmp frame_race_state,1
 jne ui_countdown_done
 mov si,offset ui_digit3
 cmp countdown,128
 ja ui_countdown_draw
 mov si,offset ui_digit2
 cmp countdown,64
 ja ui_countdown_draw
 mov si,offset ui_digit1
 cmp countdown,16
 ja ui_countdown_draw
 mov si,offset ui_go
 mov hud_y,72
 mov hud_x,UI_COUNT_X
 call hud_text
 jmp ui_countdown_dirty
ui_countdown_draw:
 mov bp,72
 mov ui_digit_rows,5
ui_digit_row:
 lodsb
 mov ui_digit_mask,al
 mov ui_digit_repeat,4
ui_digit_scanline:
 mov bx,bp
 shl bx,1
 mov di,screen_rows[bx]
 add di,UI_COUNT_X
 mov dl,4
 mov cx,3
ui_digit_cell:
 xor ax,ax
 test ui_digit_mask,dl
 jz ui_digit_cell_draw
 mov ax,0ffffh
ui_digit_cell_draw:
 if UI_BYTES eq 32768
 stosw
 else
 stosb
 endif
 shr dl,1
 loop ui_digit_cell
 inc bp
 dec ui_digit_repeat
 jnz ui_digit_scanline
 dec ui_digit_rows
 jnz ui_digit_row
ui_countdown_dirty:
 ; Ensure the road/sky replaces the countdown when this page next draws.
 mov bx,draw_page
 sub bx,4
 cmp sky_dirty_y[bx],72
 jbe ui_countdown_mark
 mov sky_dirty_y[bx],72
ui_countdown_mark:
 mov bx,frame_sky_mask_base
 add bx,112
 mov cx,20
ui_countdown_rows:
 mov sky_row_tokens[bx],0ffffh
 add bx,2
 loop ui_countdown_rows
ui_countdown_done:
 ret
ui_digit_rows dw 0
ui_digit_repeat dw 0
ui_digit_mask db 0
ui_digit3 db 7,1,7,1,7
ui_digit2 db 7,1,7,4,7
ui_digit1 db 2,6,2,2,7
ui_go db 'GO!',0
