ENGINE_PIXEL_SHIFT equ 1
ENGINE_CENTRE_BYTES equ 80
ENGINE_ACTOR_X_DIV equ 22
ENGINE_BODY_WIDTH_DIV equ 14
ENGINE_VIEW_BYTES equ 160
ENGINE_SHADOW_DIRECTORY equ 39813
ENGINE_SHADOW_ZOOM equ 41269
ENGINE_STACK_CANARY equ 36422
; Native lifecycle/presentation boundary. Source bank owns movement and contacts.
engine_initialized dw 0
engine_busy dw 0
engine_pending dw 0
engine_pending_max dw 0
engine_logic_ticks dw 0
engine_last_collision dw 0
engine_old_distance dw 0
engine_traffic_width dw 16,16,16
engine_object_x dw 8 dup(0)
engine_object_y dw 8 dup(0)
engine_object_z dw 8 dup(0)
engine_frame_x dw 8 dup(0)
engine_frame_y dw 8 dup(0)
engine_frame_z dw 8 dup(0)
engine_bonus_bcd dd 0
engine_frame_score dd 0
engine_score_text db '00000000',0
engine_bonus_input dd 0
engine_load_name db 'ENGINE.BIN',0
engine_actor_drawn db 6 dup(0)
engine_actor_index dw 0
engine_actor_offset dw 0
engine_actor_best dw 0
engine_actor_priority dw 0
engine_actor_row dw 0
engine_shadow_profile dw 0
engine_actor_missing dw 0
engine_actor_errors dw 0

engine_sync_inputs:
 mov ax,segments[ENGINE_SEGMENT]
 mov es,ax
 pushf
 cli
 mov ax,keys
 or ax,cs:keys_edge
 mov cs:keys_edge,0
 popf
 mov es:[ENGINE_IPC+IPC_HELD_KEYS],ax
 mov ax,gear_auto
 mov es:[ENGINE_IPC+IPC_GEAR_MODE],al
 mov ax,gear_state
 mov es:[ENGINE_IPC+IPC_GEAR_VALUE],al
 mov ax,time_left
 mov es:[ENGINE_IPC+IPC_TIME_LEFT],ax
 mov ax,course_index
 mov es:[ENGINE_IPC+IPC_COURSE],ax
 mov ax,stage_index
 mov es:[ENGINE_IPC+IPC_STAGE],ax
 mov ax,segments[0]
 mov es:[ENGINE_IPC+IPC_GEO_SEG],ax
 mov bx,course_index
 shl bx,1
 mov ax,segments[34]
 add ax,phys_tail_paras[bx]
 mov segments[40],ax
 mov es:[ENGINE_IPC+IPC_PHYS_SEG],ax
 ret

; Only called by the stopped-stage loader, never from an IRQ or racing.
engine_after_course:
 mov ax,segments[24]
 mov segments[ENGINE_SEGMENT],ax
 cmp engine_initialized,0
 jne engine_rebase
 mov dx,offset engine_load_name
 mov ax,3d00h
 int 21h
 jc engine_load_failed
 mov load_handle,ax
 push ds
 mov ds,cs:segments[ENGINE_SEGMENT]
 xor dx,dx
 mov cx,ENGINE_BYTES
 mov bx,cs:load_handle
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
 jc engine_load_failed
 cmp ax,ENGINE_BYTES
 jne engine_load_failed
 inc asset_reads
 mov ax,segments[ENGINE_SEGMENT]
 mov es,ax
 mov di,ENGINE_STACK_CANARY
 mov cx,32
 mov ax,0a5a5h
 pushf
 cli
 rep stosw
 popf
 call engine_sync_inputs
 xor al,al
 call engine_command
 call engine_call
 mov engine_initialized,1
 jmp engine_after_ready
engine_rebase:
 call engine_sync_inputs
 mov ax,player_x
 mov cx,212
 imul cx
 mov cx,256
 idiv cx
 neg ax
 mov es:[ENGINE_IPC+IPC_SOURCE_X],ax
 mov al,2
 call engine_command
 call engine_call
engine_after_ready:
 mov engine_phase,0
 mov engine_pending,0
 mov engine_old_distance,0
 cmp engine_fault,0
 jne engine_load_failed
 call engine_export
 ret
engine_load_failed:
 mov exit_flag,1
 mov finish_reason,3
 ret

engine_tick:
 call engine_cadence
 jnc engine_no_new_tick
 inc engine_pending
 mov ax,engine_pending
 cmp ax,engine_pending_max
 jbe engine_no_new_tick
 mov engine_pending_max,ax
engine_no_new_tick:
 cmp engine_busy,0
 jne engine_tick_done
 cmp present_active,0
 je engine_run_now
 cmp engine_pending,1
 jbe engine_tick_done
engine_run_now:
 cmp engine_pending,0
 je engine_tick_done
 dec engine_pending
 call engine_sync_inputs
 ; Three retained traffic slots; source methods own movement/hit/pass effects.
 mov si,ENGINE_IPC+IPC_OBJECTS+5*IPC_OBJECT_BYTES
 xor bx,bx
engine_traffic_inputs:
 mov ax,engine_traffic_width[bx]
 mov es:[si+8],ax
 cmp byte ptr es:[si],0
 jne engine_traffic_next
 mov byte ptr es:[si],1
 mov byte ptr es:[si+1],2
 mov ax,engine_traffic_types[bx]
 mov es:[si+6],ax
 mov ax,engine_traffic_lanes[bx]
 mov es:[si+10],ax
 mov word ptr es:[si+12],200
 mov word ptr es:[si+14],100
 mov word ptr es:[si+16],0
 mov word ptr es:[si+20],0
 mov ax,engine_traffic_depths[bx]
 mov es:[si+22],ax
engine_traffic_next:
 add si,IPC_OBJECT_BYTES
 add bx,2
 cmp bx,6
 jb engine_traffic_inputs
 mov al,1
 call engine_command
 call engine_call
 cmp engine_fault,0
 jne engine_load_failed
 inc engine_logic_ticks
 mov engine_display_dirty,1
 cmp race_state,2
 jne engine_tick_done
 call engine_timer_step
 call engine_export
engine_tick_done:
 ret
engine_traffic_types dw 96,128,40
engine_traffic_lanes dw -100,0,100
engine_traffic_depths dw 128,224,320

engine_export:
 push ds
 mov ds,cs:segments[ENGINE_SEGMENT]
 mov ax,ds:[ENGINE_IPC+IPC_SOURCE_SPEED]
 mov cs:speed,ax
 mov ax,ds:[ENGINE_IPC+IPC_NATIVE_X]
 mov cs:player_x,ax
 mov ax,ds:[ENGINE_IPC+IPC_NATIVE_DISTANCE+2]
 mov dx,cs:engine_old_distance
 mov cs:stage_distance,ax
 mov cs:engine_old_distance,ax
 sub ax,dx
 ; Defensive boundary: a source adapter failure cannot unsigned-wrap world.
 js engine_distance_fault
 add cs:world,ax
 jmp engine_distance_ready
engine_distance_fault:
 mov cs:engine_fault,4
engine_distance_ready:
 xor ax,ax
 cmp byte ptr ds:[ENGINE_IPC+IPC_WHEEL_STATE],0
 je engine_wheels_ready
 inc ax
engine_wheels_ready:
 mov cs:offroad,ax
 xor ax,ax
 mov al,byte ptr ds:[ENGINE_IPC+IPC_CRASH_STATE]
 or al,byte ptr ds:[ENGINE_IPC+IPC_CRASH_COUNTER]
 or al,byte ptr ds:[ENGINE_IPC+IPC_SPIN_CONTROL1]
 or al,byte ptr ds:[ENGINE_IPC+IPC_SPIN_CONTROL2]
 mov cs:crash_ticks,ax
 mov ax,ds:[ENGINE_IPC+IPC_COLLISION_COUNT]
 cmp ax,cs:engine_last_collision
 jle engine_collision_ready
 inc word ptr cs:collisions
 mov word ptr cs:effect_ticks,12
engine_collision_ready:
 mov cs:engine_last_collision,ax
 xor ax,ax
 mov al,ds:[ENGINE_IPC+IPC_GEAR_VALUE]
 mov cs:gear_state,ax
 mov si,ENGINE_IPC+IPC_OBJECTS
 xor di,di
 xor bp,bp
engine_export_object:
 mov word ptr cs:objects[bp],0
 cmp byte ptr [si],0
 je engine_export_next
 mov cx,[si+22]
 cmp cx,4
 jb engine_export_next
 cmp cx,512
 jae engine_export_next
 mov cs:engine_object_z[di],cx
 mov ax,24528
 xor dx,dx
 div cx
 mov cs:objects[bp],ax
 mov ax,[si+10]
 mov cs:engine_object_x[di],ax
 mov ax,[si+12]
 sub ax,18
 mov cx,200
 imul cx
 mov cx,224
 idiv cx
 mov cs:engine_object_y[di],ax
 cmp byte ptr [si+1],1
 jne engine_export_traffic
 push ds
 mov ds,cs:segments[40]
 mov bx,cs:course_index
 ; Descriptor offset is relative to the current compact PHYS segment.
 pop ds
 mov bx,[si+4]
 push ds
 mov ds,cs:segments[40]
 xor ax,ax
 mov al,[bx+18]
 pop ds
 add ax,256
 jmp engine_export_type
engine_export_traffic:
 mov ax,0
 cmp di,10
 je engine_export_type
 mov ax,2
 cmp di,12
 je engine_export_type
 mov ax,3
engine_export_type:
 mov cs:objects[bp+4],ax
 cmp byte ptr [si+1],1
 je engine_export_next
 call engine_logic_width
engine_export_next:
 add si,IPC_OBJECT_BYTES
 add di,2
 add bp,6
 cmp di,16
 jb engine_export_object
 pop ds
 ret

; AX:DX input packed BCD (AX low), retained checkpoint/ending display bonus.
engine_add_bonus:
 mov word ptr engine_bonus_input,ax
 mov word ptr engine_bonus_input+2,dx
 xor bx,bx
 mov cx,4
 clc
engine_bonus_byte:
 mov al,byte ptr engine_bonus_bcd[bx]
 adc al,byte ptr engine_bonus_input[bx]
 daa
 mov byte ptr engine_bonus_bcd[bx],al
 inc bx
 loop engine_bonus_byte
 ret

engine_score_snapshot:
 xor bx,bx
 mov cx,4
 clc
engine_score_byte:
 mov al,byte ptr engine_snapshot[IPC_SCORE_BCD+bx]
 adc al,byte ptr engine_bonus_bcd[bx]
 daa
 mov byte ptr engine_frame_score[bx],al
 inc bx
 loop engine_score_byte
 ret

engine_hud_score:
 mov bx,3
 mov di,offset engine_score_text
engine_score_digits:
 mov al,byte ptr engine_frame_score[bx]
 mov ah,al
 mov cl,4
 shr al,cl
 add al,'0'
 mov [di],al
 inc di
 mov al,ah
 and al,15
 add al,'0'
 mov [di],al
 inc di
 dec bx
 jns engine_score_digits
 mov si,offset engine_score_text
 call hud_text
 ret

; W: retained-art traffic width at LOGIC cadence. Mirrors the renderer's selection
; exactly (TOSE.ASM object loop + draw_sprite): word guard [48,2040), depth = 24528/word
; clamped to 511, half = source_half_width[depth], id = (clamp(half*2/5,8,40)-8)/2 +
; VAN_FIRST (+31 type 2, +48 type 3), width = sprite bank width word & 3fffh.
; In: AX = objects z-word, DX = objects type word. Out: AX = width << ENGINE_PIXEL_SHIFT;
; CF set (AX undefined) when the renderer would not draw the object. Other regs preserved.
engine_width_from_object:
 push bx
 push cx
 push dx
 push ds
 cmp ax,48
 jb engine_wfo_none
 cmp ax,2040
 jae engine_wfo_none
 mov cx,ax
 mov ax,24528
 push dx
 xor dx,dx
 div cx
 pop dx
 cmp ax,511
 jbe engine_wfo_depth
 mov ax,511
engine_wfo_depth:
 mov bx,ax
 shl bx,1
 mov ax,cs:source_half_width[bx]
 shl ax,1
 push dx
 xor dx,dx
 mov cx,5
 div cx
 pop dx
 cmp ax,8
 jae engine_wfo_min
 mov ax,8
engine_wfo_min:
 cmp ax,40
 jbe engine_wfo_max
 mov ax,40
engine_wfo_max:
 sub ax,8
 shr ax,1
 add ax,VAN_FIRST
 cmp dx,2
 jne engine_wfo_not2
 add ax,31
 jmp short engine_wfo_id
engine_wfo_not2:
 cmp dx,3
 jne engine_wfo_id
 add ax,48
engine_wfo_id:
 mov bx,ax
 shl bx,1
 mov ds,cs:segments[SPRITE_SEGMENT]
 mov bx,[bx]
 mov ax,[bx]
 and ax,3fffh
 mov cl,ENGINE_PIXEL_SHIFT
 shl ax,cl
 pop ds
 pop dx
 pop cx
 pop bx
 clc
 ret
engine_wfo_none:
 pop ds
 pop dx
 pop cx
 pop bx
 stc
 ret

; Logic-cadence record for traffic slot DI (word offsets 10/12/14) from objects[BP].
engine_logic_width:
 push ax
 push bx
 push dx
 mov ax,cs:objects[bp]
 mov dx,cs:objects[bp+4]
 call engine_width_from_object
 jc engine_lw_done
 mov bx,di
 sub bx,10
 mov cs:engine_traffic_width[bx],ax
engine_lw_done:
 pop dx
 pop bx
 pop ax
 ret
IFDEF WIDTH_ORACLE
engine_width_checks dw 0
engine_width_mismatch dw 0
; Oracle: compare the helper, fed the FRAME SNAPSHOT the renderer just drew from, with
; the width the renderer actually used. Counts every drawn traffic sprite.
engine_oracle_width:
 push ax
 push bx
 push cx
 push dx
 mov ax,entity_index
 xor dx,dx
 mov cx,6
 div cx
 sub ax,5
 jb engine_ow_done
 cmp ax,3
 jae engine_ow_done
 mov bx,entity_index
 mov ax,frame_objects[bx]
 mov dx,frame_objects[bx+4]
 call engine_width_from_object
 inc engine_width_checks
 jc engine_ow_bad
 mov bx,sprite_width
 mov cl,ENGINE_PIXEL_SHIFT
 shl bx,cl
 cmp ax,bx
 je engine_ow_done
engine_ow_bad:
 inc engine_width_mismatch
engine_ow_done:
 pop dx
 pop cx
 pop bx
 pop ax
 ret
ENDIF
engine_record_width:
 mov ax,entity_index
 xor dx,dx
 mov cx,6
 div cx
 sub ax,5
 jb engine_record_done
 cmp ax,3
 jae engine_record_done
 shl ax,1
 mov bx,ax
 mov ax,sprite_width
 mov cl,ENGINE_PIXEL_SHIFT
 shl ax,cl
 mov engine_traffic_width[bx],ax
engine_record_done:
 ret

engine_frame_counter dw 30
; Cannonball outrun.cpp decrement_timers(), frame_reset = 30.
engine_timer_step:
 dec engine_frame_counter
 js engine_timer_fire
 ret
engine_timer_fire:
 mov engine_frame_counter,30
 cmp time_left,0
 je engine_timer_done
 dec time_left
 jnz engine_timer_done
 mov race_state,4
 mov finish_reason,2
 mov speed,0
engine_timer_done:
 ret

keys_edge dw 0
keys_edge2 dw 0
key_edge_set:
 cmp cs:race_state,2
 jne key_edge_ret
 cmp cs:pause_flag,0
 jne key_edge_ret
 or byte ptr cs:keys_edge,bh
key_edge_ret:
 ret
engine_display_dirty dw 0
; Refresh the deferred scenery display (kernel command 3) and re-export the object table. Called with interrupts
; disabled right before the frame snapshot. A step cannot be running (steps run inside the timer handler, and the
; mainline only runs between handlers); a timer tick that nests during the kernel call only queues its step.
engine_refresh_display:
 cmp engine_display_dirty,0
 je engine_refresh_done
 cmp engine_busy,0
 jne engine_refresh_done
 mov engine_display_dirty,0
 mov al,3
 call engine_command
 call engine_call
 cmp engine_fault,0
 jne engine_refresh_done
 call engine_export
engine_refresh_done:
 ret
