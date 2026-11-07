; Own compact interface. Every write goes to the currently hidden video page.
draw_hud:
 mov bx,draw_page
 sub bx,4
 cmp hud_initialized[bx],1
 je hud_header_done
 mov hud_initialized[bx],1
 ; Both bands are outside the existing road/sprite clip rectangle.
 ; Clear once per hidden-page initialization, before writing glyphs.
 call initialize_hud_bands
 mov hud_y,1
 mov hud_x,2
 mov si,offset hud_header
 call hud_text
hud_header_done:
 mov hud_y,8
 mov hud_x,29
 mov hud_digit_count,3
 mov ax,frame_time_left
 mov bx,draw_page
 sub bx,4
 cmp hud_time_cache[bx],ax
 je hud_time_done
 mov hud_time_cache[bx],ax
 call hud_number
hud_time_done:
 mov hud_x,62
 mov hud_digit_count,1
 mov ax,frame_stage_index
 inc ax
 mov bx,draw_page
 sub bx,4
 cmp hud_stage_cache[bx],ax
 je hud_stage_done
 mov hud_stage_cache[bx],ax
 call hud_number
hud_stage_done:
 mov hud_x,98
 call engine_hud_score
 mov hud_x,134
 mov hud_digit_count,3
 mov ax,frame_speed
 mov bx,draw_page
 sub bx,4
 cmp hud_speed_cache[bx],ax
 je hud_speed_done
 mov hud_speed_cache[bx],ax
 call hud_number
hud_speed_done:
 mov hud_x,149
 mov hud_y,8
 mov byte ptr hud_gear,'M'
 cmp frame_gear_auto,0
 je hud_gear_mode_ready
 mov byte ptr hud_gear,'A'
hud_gear_mode_ready:
 mov byte ptr hud_gear[1],'L'
 cmp frame_gear_state,0
 je hud_gear_ready
 mov byte ptr hud_gear[1],'H'
hud_gear_ready:
 mov si,offset hud_gear
 call hud_text
 ; Course/status text is updated separately on each page when it changes.
 mov ax,frame_race_state
 cmp frame_pause_flag,0
 je hud_not_paused
 mov ax,5
 jmp hud_no_fork
hud_not_paused:
 cmp frame_checkpoint_ticks,0
 je hud_no_checkpoint
 mov ax,6
hud_no_checkpoint:
 cmp frame_race_state,3
 jne hud_no_ending
 mov ax,12
 cmp frame_ending_done,0
 je hud_no_fork
 mov ax,13
 jmp hud_no_fork
hud_no_ending:
 cmp frame_crash_ticks,0
 je hud_no_collision
 mov ax,11
hud_no_collision:
 cmp frame_distance,14336
 jb hud_no_fork
 cmp frame_race_state,2
 jne hud_no_fork
 mov ax,7
 cmp frame_stage_index,4
 jne hud_fork_choice
 mov ax,8
 jmp hud_no_fork
hud_fork_choice:
 cmp frame_branch_choice,0
 je hud_no_fork
 mov ax,9
 cmp frame_branch_choice,1
 jne hud_no_fork
 mov ax,10
hud_no_fork:
 mov dx,frame_course
 mov dh,al
 push ax
 mov ax,radio_track
 mov cl,3
 shl al,cl
 add dh,al
 pop ax
 mov bx,draw_page
 sub bx,4
 cmp hud_cache[bx],dx
 je hud_done
 mov hud_cache[bx],dx
 push ax
 mov hud_y,184
 mov hud_x,2
 mov si,offset hud_blank
 call hud_text
 mov bx,frame_course
 shl bx,1
 mov si,course_names[bx]
 cmp frame_race_state,3
 jne hud_not_goal
 mov ax,route_index
 add al,'A'
 mov hud_goal[5],al
 mov si,offset hud_goal
 jmp hud_course_selected
hud_not_goal:
 cmp frame_race_state,0
 jne hud_course_selected
 mov bx,radio_track
 shl bx,1
 mov si,hud_tracks[bx]
hud_course_selected:
 mov hud_x,2
 call hud_text
 mov hud_y,192
 mov hud_x,2
 mov si,offset hud_blank
 call hud_text
 pop bx
 shl bx,1
 mov si,hud_states[bx]
 mov hud_x,2
 call hud_text
hud_done:
 ret
hud_number:
 ; Five decimal characters, zero padded, no DOS/BIOS calls.
 mov bx,10
 mov di,offset hud_digits+4
 mov cx,5
hud_digit:
 xor dx,dx
 div bx
 add dl,'0'
 mov [di],dl
 dec di
 loop hud_digit
 mov si,offset hud_digits
 mov ax,5
 sub ax,hud_digit_count
 add si,ax
 call hud_text
 ret
hud_text:
 lodsb
 or al,al
 jz hud_text_done
 push si
 sub al,32
 xor ah,ah
 mov bx,24
 mul bx
 mov si,ax
 add si,offset hud_font
 mov bx,hud_y
 shl bx,1
 mov bp,8
hud_glyph_row:
 mov di,screen_rows[bx]
 add di,hud_x
 movsw
 movsb
 add bx,2
 dec bp
 jnz hud_glyph_row
 add hud_x,3
 pop si
 jmp hud_text
hud_text_done:
 ret
hud_digit_count dw 5
hud_goal db 'GOAL A - OUTRUN',0
hud_cache dw 0ffffh,0ffffh
hud_initialized dw 0,0
hud_time_cache dw 0ffffh,0ffffh
hud_stage_cache dw 0ffffh,0ffffh
hud_speed_cache dw 0ffffh,0ffffh
hud_header db 'OUTRUN   TIME       STAGE       SCORE       KM/H',0
hud_blank db '                                                    ',0
hud_states dw offset hud_title,offset hud_countdown,offset hud_race,offset hud_finish,offset hud_timeout,offset hud_pause,offset hud_checkpoint,offset hud_fork,offset hud_approach,offset hud_left,offset hud_right,offset hud_collision,offset hud_outro,offset hud_complete
hud_title db 'CHOOSE MUSIC 1/2/3   ENTER OR UP TO START',0
hud_countdown db 'GET READY - HOLD UP TO ACCELERATE',0
hud_race db 'ARROWS DRIVE  M SOUND  P PAUSE  R RESTART  ESC EXIT',0
hud_finish db 'GOAL REACHED!  R RACE AGAIN   ESC EXIT',0
hud_timeout db 'TIME UP!  R TRY AGAIN   ESC EXIT',0
hud_pause db 'PAUSED - P TO RESUME   ESC EXIT',0
hud_checkpoint db 'CHECKPOINT! TIME EXTENDED',0
hud_fork db 'FORK: STEER LEFT OR RIGHT TO CHOOSE NEXT COURSE',0
hud_approach db 'FINISH AHEAD - BRING IT HOME!',0
hud_tracks dw offset hud_mss,offset hud_pb,offset hud_sw
hud_mss db '1 MAGICAL SOUND SHOWER',0
hud_pb db '2 PASSING BREEZE',0
hud_sw db '3 SPLASH WAVE',0

hud_left db 'LEFT ROUTE LOCKED - FOLLOW THE LEFT ROAD',0
hud_right db 'RIGHT ROUTE LOCKED - FOLLOW THE RIGHT ROAD',0
hud_collision db 'COLLISION - RECOVERING',0
hud_outro db 'CONGRATULATIONS!   R NEW RACE   ESC EXIT',0
hud_complete db 'GOAL COMPLETE!   R NEW RACE   ESC EXIT',0

initialize_hud_bands:
 push ax
 push bx
 push cx
 push di
 xor ax,ax
 xor bx,bx
initialize_hud_top_row:
 mov di,screen_rows[bx]
 mov cx,80
 pushf
 cli
 rep stosw
 popf
 add bx,2
 cmp bx,32
 jb initialize_hud_top_row
 mov bx,368
initialize_hud_bottom_row:
 mov di,screen_rows[bx]
 mov cx,80
 pushf
 cli
 rep stosw
 popf
 add bx,2
 cmp bx,400
 jb initialize_hud_bottom_row
 pop di
 pop cx
 pop bx
 pop ax
 ret
