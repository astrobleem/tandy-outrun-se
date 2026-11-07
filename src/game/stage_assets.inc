; Stopped-stage/ending working set loader. Never invoked from an IRQ.
load_stage_working_set:
 mov runtime_load_phase,1
 mov runtime_load_error,0
 mov runtime_read_bytes,0
 mov ax,ticks
 mov stage_load_started,ax
 cmp race_state,2
 jne stage_load_safe
 inc race_disk_violations
stage_load_safe:
 cmp stage_load_requested,2
 je ending_load_name
 mov word ptr load_filename,4353h
 mov byte ptr load_filename[2],'N'
 mov ax,course_index
 xor dx,dx
 mov bx,10
 div bx
 add al,'0'
 mov load_filename[3],al
 mov al,dl
 add al,'0'
 mov load_filename[4],al
 jmp working_set_name_ready
ending_load_name:
 mov word ptr load_filename,4e45h
 mov byte ptr load_filename[2],'D'
 mov byte ptr load_filename[3],'0'
 mov ax,route_index
 add al,'0'
 mov load_filename[4],al
working_set_name_ready:
 mov dx,offset load_filename
 mov ax,3d00h
 int 21h
 jc stage_open_failed
 mov runtime_load_phase,2
 mov load_handle,ax
 push ds
 mov ds,segments[SCENE_SEGMENT]
 mov bx,cs:load_handle
 xor dx,dx
 mov cx,60000
 mov ah,3fh
 int 21h
 pop ds
 mov runtime_read_bytes,ax
 jc stage_read_failed
 cmp ax,60000
 jne stage_read_failed
 inc asset_reads
 mov bx,load_handle
 mov ah,3eh
 int 21h
 mov runtime_load_phase,3
 push ds
 mov ds,segments[SCENE_SEGMENT]
 cmp cs:stage_load_requested,2
 je ending_header_check
 cmp word ptr ds:[512],4353h
 jne stage_header_failed
 cmp word ptr ds:[514],354eh
 jne stage_header_failed
 pop ds
 jmp working_set_header_valid
ending_header_check:
 cmp word ptr ds:[512],4e45h
 jne stage_header_failed
 cmp word ptr ds:[514],3344h
 jne stage_header_failed
 mov ax,word ptr ds:[526]
 mov cs:ending_length,ax
 pop ds
working_set_header_valid:
 cmp stage_load_requested,1
 jne source_load_not_needed
 call load_source_geometry
 cmp exit_flag,0
 jne source_load_abort
 call load_source_scene_secondary
 cmp exit_flag,0
 jne source_load_abort
 call load_course_sky
 cmp exit_flag,0
 jne source_load_abort
 jmp source_scene_ready
source_load_not_needed:
 call clear_source_scene_map
source_scene_ready:
 mov runtime_load_phase,4
 mov ax,ticks
 sub ax,stage_load_started
 mov stage_load_ticks,ax
 cmp ax,stage_load_max
 jbe stage_load_time_ready
 mov stage_load_max,ax
stage_load_time_ready:
 cmp stage_load_requested,2
 jne scenery_load_complete
 mov stage_load_requested,0
 mov race_state,3
 mov ending_ticks,0
 mov ending_done,0
 ret
scenery_load_complete:
 call prepare_materials
 mov branch_choice,0
 mov ax,geo_meta[0]
 mov cx,256
 mul cx
 mov cx,212
 div cx
 mov split_near_offset,ax
 sub player_x,ax
 mov spawn_bucket,0ffffh
 mov spawn_slot,18
 mov objects[0],300
 mov objects[6],600
 mov objects[12],900
 mov objects[18],0
 mov objects[24],0
 mov objects[30],0
 mov objects[36],0
 mov objects[42],0
 call engine_after_course
 mov stage_load_requested,0
 cmp stage_resume_race,0
 je scenery_start_countdown
 mov stage_resume_race,0
 mov ax,stage_resume_speed
 mov speed,ax
 mov race_state,2
 ret
scenery_start_countdown:
 mov race_state,1
 mov countdown,192
 ret
stage_header_failed:
 pop ds
 mov runtime_load_error,0ffffh
 jmp stage_load_failed
stage_read_failed:
 mov runtime_load_error,ax
 mov bx,load_handle
 mov ah,3eh
 int 21h
 jmp stage_load_failed
stage_open_failed:
 mov runtime_load_error,ax
stage_load_failed:
 mov stage_load_requested,0
 mov exit_flag,1
 mov finish_reason,3
 ret

spawn_stage_scenery:
 mov ax,stage_distance
 mov cl,8
 shr ax,cl
 cmp ax,spawn_bucket
 je scene_spawn_done
 mov spawn_bucket,ax
 mov bx,ax
 shl bx,1
 shl bx,1
 shl bx,1
 add bx,528
 push ds
 mov ds,segments[SCENE_SEGMENT]
 cmp word ptr [bx+6],0
 je scene_spawn_empty
 mov dx,[bx]
 mov ax,[bx+2]
 mov cs:scene_y_temp,ax
 mov ax,[bx+4]
 add ax,256
 mov cs:scene_base_temp,ax
 pop ds
 mov si,spawn_slot
 mov objects[si],1900
 mov objects[si+2],dx
 mov ax,scene_base_temp
 mov objects[si+4],ax
 mov ax,si
 xor dx,dx
 mov bx,6
 div bx
 shl ax,1
 mov bx,ax
 mov ax,scene_y_temp
 mov object_yw[bx],ax
 add spawn_slot,6
 cmp spawn_slot,48
 jb scene_spawn_done
 mov spawn_slot,18
 ret
scene_spawn_empty:
 pop ds
scene_spawn_done:
 ret

draw_ending:
 push ds
 mov ds,segments[SCENE_SEGMENT]
 mov si,word ptr ds:[510]
 or si,si
 jz ending_no_back
 lodsw
 mov cs:ending_items,ax
 mov cs:ending_ptr,si
 pop ds
 call ending_actor
 push ds
 mov ds,segments[SCENE_SEGMENT]
ending_no_back:
 mov bx,cs:frame_ending_ticks
 mov cl,3
 shr bx,cl
 cmp bx,word ptr ds:[522]
 jb ending_frame_valid
 mov bx,word ptr ds:[522]
 dec bx
ending_frame_valid:
 shl bx,1
 add bx,word ptr ds:[520]
 mov si,[bx]
 lodsw
 mov cs:ending_items,ax
 mov cs:ending_ptr,si
 pop ds
ending_actor:
 cmp ending_items,0
 je ending_draw_done
 mov si,ending_ptr
 push ds
 mov ds,segments[SCENE_SEGMENT]
 lodsw
 sar ax,1
 mov cs:sprite_x,ax
 lodsw
 mov cs:sprite_y,ax
 lodsw
 mov bx,ax
 or ax,8000h
 mov cs:sprite_id,ax
 mov cs:ending_ptr,si
 shl bx,1
 mov si,[bx]
 mov ax,[si]
 and ax,3fffh
 shr ax,1
 sub cs:sprite_x,ax
 pop ds
 call draw_sprite
 dec ending_items
 jmp ending_actor
ending_draw_done:
 ret

prepare_materials:
 mov ax,course_index
 mov bx,40
 mul bx
 mov bx,ax
 xor si,si
material_row:
 mov ax,334
 sub ax,si
 mov cl,3
 shr ax,cl
 and ax,0fffeh
 cmp ax,30
 jbe source_ground_palette_valid
 mov ax,30
source_ground_palette_valid:
 mov di,bx
 add di,ax
 mov ax,course_materials[di]
 test si,2
 jz material_row_even
 ror ax,1
 ror ax,1
 ror ax,1
 ror ax,1
material_row_even:
 mov ground_rows[si],ax
 add si,2
 cmp si,336
 jb material_row
 mov ax,course_materials[bx+32]
 mov road_colors[0],ax
 ror ax,1
 ror ax,1
 ror ax,1
 ror ax,1
 mov road_colors[4],ax
 mov ax,course_materials[bx+34]
 mov road_colors[2],ax
 ror ax,1
 ror ax,1
 ror ax,1
 ror ax,1
 mov road_colors[6],ax
 mov ax,course_materials[bx+36]
 mov edge_colors[0],ax
 mov ax,course_materials[bx+38]
 mov edge_colors[2],ax
 ret

source_load_abort:
 ret
