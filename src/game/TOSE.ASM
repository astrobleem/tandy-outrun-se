.8086
.model tiny
.code
org 100h
entry: jmp near ptr start
db 'TOSE0003'
front_page dw 6
draw_page dw 4
reserve_seg dw 0
reserve_paras dw 0
flips dw 0
ready dw 0
started dw 0
player_fast dw 1
object_fast dw 1
player_call dd 0
frames dw 0
ticks dw 0
keys dw 0
speed dw 0
player_x dw 0
world dw 0
curve dw 8
hill dw 0
collisions dw 0
offroad dw 0
music dw 1
psg_writes dw 0
input_tick dw 0
display_input_tick dw 0
frame_tick dw 0
dos_free_para dw 0
dos_remaining_para dw 0
bios_kb dw 0
asset_reads dw 0
old8 dd 0
old9 dd 0
bios_di_before dw 0
bios_cx_before dw 0
bios_di_changes dw 0
bios_cx_changes dw 0
bios_acc dw 0
last_frame_tick dw 0
note_index dw 0
music_position dw 4
music_wait dw 0
radio_track dw 0
music_bank dw 0
noise_atten dw 15
noise_control_last dw 0ffffh
passenger_hair dw 0
hair_counter dw 0
race_state dw 0
stage_index dw 0
route_index dw 0
course_index dw 0
time_left dw 75
stage_distance dw 0
score dw 0
race_elapsed dw 0
second_tick dw 0
countdown dw 192
finish_reason dw 0
restarts dw 0
reset_requested dw 0
pause_flag dw 0
crash_ticks dw 0
checkpoint_ticks dw 0
hud_x dw 0
hud_y dw 0
hud_char dw 0
hud_digits db '00000',0
exit_flag dw 0
segments dw 24 dup(0)
load_index dw 0
load_size dw 0
load_handle dw 0
frame_player dw 0
frame_world dw 0
frame_speed dw 0
frame_curve dw 0
frame_hill dw 0
frame_course dw 0
frame_material dw 0
frame_distance dw 0
spawn_bucket dw 0ffffh
spawn_slot dw 18
stage_load_requested dw 0
stage_load_ticks dw 0
stage_load_max dw 0
race_disk_violations dw 0
stage_load_started dw 0
stage_resume_speed dw 0
stage_resume_race dw 0
runtime_load_phase dw 0
runtime_load_error dw 0
runtime_read_bytes dw 0
object_yw dw 8 dup(0)
frame_object_yw dw 8 dup(0)
ending_ticks dw 0
ending_length dw 0
ending_done dw 0
ending_ptr dw 0
ending_items dw 0
ending_bonus dw 0
frame_race_state dw 0
frame_stage_index dw 0
frame_time_left dw 0
frame_score dw 0
frame_ending_ticks dw 0
frame_branch_choice dw 0
frame_crash_ticks dw 0
frame_checkpoint_ticks dw 0
frame_pause_flag dw 0
frame_ending_done dw 0
scene_y_temp dw 0
scene_base_temp dw 0
load_filename db 'SCN00.BIN',0
road_material dw 0
frame_constant_material dw 0
edge_material dw 0
frame_keys dw 0
frame_input_tick dw 0
row_centre dw 0
row_half dw 0
row_lane dw 0
road_row_buffer db 1344 dup(0)
ground_rows dw 168 dup(0)
road_colors dw 4 dup(0)
edge_colors dw 2 dup(0)
frame_player_sprite dw 0
row_depth dw 0
row_left dw 0
row_right dw 0
row_split_pixels dw 0
row_gap_active dw 0
row_single_visible dw 0
row_local_centre dw 0
row_gap_left dw 0
row_gap_right dw 0
frame_fork dw 0
split_near_offset dw 0
branch_choice dw 0
ground dw 0
entity_index dw 0
sort_index dw 0
sort_best dw 0
sort_slot dw 0
sort_z dw 0
sprite_id dw 0
sprite_x dw 0
sprite_y dw 0
sprite_width dw 0
sprite_height dw 0
sprite_rle dw 0
rle_segments dw 0
rle_masked dw 0
rle_right_skip dw 0
sprite_top dw 0
sprite_line dw 0
sprite_skip dw 0
sprite_clip_left dw 0
sprite_clip_top dw 0
sprite_visible_width dw 0
sprite_visible_height dw 0
object_y dw 0
object_half dw 0
objects dw 300,-128,0,600,0,2,900,128,3,200,-350,1,450,350,1,750,-350,1,1100,350,1,1500,-350,1
frame_objects dw 24 dup(0)
sort_depths dw 8 dup(0)
centres dw 168 dup(0)
widths dw 168 dup(0)
include source_geometry.inc
include source_scene.inc
include sky_restore.inc
include gear.inc
include effects.inc
UI_BYTES equ 32768
UI_COUNT_X equ 75
UI_SCORE_X equ 70
include polish_radio.inc
include polish_ui.inc
include splash_audio.inc
start:
 cld
 mov ax,cs
 mov ds,ax
 mov es,ax
 mov ss,ax
 mov sp,offset stack_top
 cmp byte ptr ds:[80h],0
 je command_parsed
 cmp byte ptr ds:[82h],'g'
 jne command_parsed
 mov player_fast,0
 mov object_fast,0
command_parsed:
 mov bx,offset program_end
 add bx,15
 mov cl,4
 shr bx,cl
 mov ah,4ah
 int 21h
 jc load_error
 mov ax,9
 int 10h
 xor ax,ax
 mov es,ax
 mov ax,es:[413h]
 mov bios_kb,ax
 mov bx,0ffffh
 mov ah,48h
 int 21h
 mov dos_free_para,bx
 mov ax,cs
 mov es,ax
load_next:
 mov bx,load_index
 shl bx,1
 mov dx,asset_names[bx]
 mov ax,3d00h
 int 21h
 jc load_error
 mov load_handle,ax
 mov bx,load_index
 shl bx,1
 mov ax,asset_sizes[bx]
size_ready:
 mov load_size,ax
 mov bx,ax
 add bx,15
 mov cl,4
 shr bx,cl
 mov ah,48h
 int 21h
 jc load_error
 mov bx,load_index
 shl bx,1
 mov segments[bx],ax
 push ds
 mov ds,ax
 xor dx,dx
 mov bx,cs:load_handle
 mov cx,cs:load_size
 mov ah,3fh
 int 21h
 pop ds
 jc load_error
 cmp ax,load_size
 jne load_error
 inc asset_reads
 mov bx,load_handle
 mov ah,3eh
 int 21h
 inc load_index
 cmp load_index,ASSET_COUNT
 jb load_next
 call prepare_materials
 call cache_source_geometry
 call cache_source_scene
 mov ax,geo_meta[0]
 mov cx,256
 mul cx
 mov cx,212
 div cx
 mov split_near_offset,ax
 neg ax
 mov player_x,ax
 ; Own the contiguous upper free block before using shared-RAM video pages.
 mov bx,0ffffh
 mov ah,48h
 int 21h
 mov reserve_paras,bx
 mov ah,48h
 int 21h
 jc load_error
 mov reserve_seg,ax
 cmp ax,9000h
 ja load_error
 add ax,reserve_paras
 mov bx,bios_kb
 mov cl,6
 shl bx,cl
 cmp ax,bx
 jb load_error
 mov dx,3dfh
 mov al,0f6h
 out dx,al
 mov ax,0b800h
 mov es,ax
 push ds
 mov ds,segments[SKY_SEGMENT]
 xor si,si
 xor di,di
 mov cx,16384
 rep movsw
 pop ds
 ; Same static sky/HUD in the hidden 32KB page, while page 6 stays visible.
 mov dx,3dfh
 mov al,0e6h
 out dx,al
 push ds
 mov ds,segments[SKY_SEGMENT]
 xor si,si
 xor di,di
 mov cx,16384
 rep movsw
 pop ds
 ; The static initialization bank is no longer required during racing.
 mov es,segments[SKY_SEGMENT]
 mov ah,49h
 int 21h
 mov segments[SKY_SEGMENT],0
 mov bx,0ffffh
 mov ah,48h
 int 21h
 mov dos_remaining_para,bx
ifndef DIAG_COURSE
 ifndef DIAG_ENDING
 call splash_quiet
 endif
endif
 mov ax,3508h
 int 21h
 mov word ptr old8,bx
 mov word ptr old8+2,es
 mov ax,3509h
 int 21h
 mov word ptr old9,bx
 mov word ptr old9+2,es
 cli
 xor ax,ax
 mov es,ax
 mov word ptr es:[20h],offset timer_irq
 mov es:[22h],cs
 mov word ptr es:[24h],offset keyboard_irq
 mov es:[26h],cs
 mov al,36h
 out 43h,al
 mov ax,18643
 out 40h,al
 mov al,ah
 out 40h,al
 sti
 mov ready,1
 mov objects[18],0
 mov objects[24],0
 mov objects[30],0
 mov objects[36],0
 mov objects[42],0
ifdef DIAG_ENDING
 mov route_index,DIAG_ENDING
 mov stage_index,4
 mov course_index,10+DIAG_ENDING
 call load_source_geometry
 call prepare_materials
 mov music_bank,3
 mov music_position,4
 mov music_wait,0
 mov stage_load_requested,2
 mov race_state,7
endif
ifdef DIAG_COURSE
 mov pause_flag,1
 mov race_state,6
 mov course_index,DIAG_COURSE
 mov stage_index,DIAG_STAGE
 mov route_index,DIAG_ROUTE
 mov stage_distance,DIAG_DISTANCE
 mov player_x,0
 mov split_near_offset,0
 mov stage_load_requested,1
 mov stage_resume_race,1
 mov stage_resume_speed,0
 call load_stage_working_set
 call update_split_near
 if DIAG_SIDE eq 1
 neg player_x
 endif
 ifdef DIAG_SPEED
 mov speed,DIAG_SPEED
 else
 mov speed,240
 endif
 ifdef DIAG_LANE
 add player_x,DIAG_LANE
 endif
 mov started,1
 mov time_left,99
 ifdef DIAG_HOLD
 mov pause_flag,1
 else
 mov pause_flag,0
 endif
endif
ifndef DIAG_COURSE
 ifndef DIAG_ENDING
 mov race_state,8
 endif
endif
frame_loop:
 cmp exit_flag,0
 jne quit
 cmp race_state,4
 jne polish_not_timeout
 mov race_state,13
polish_not_timeout:
 cmp race_state,3
 jne polish_not_complete
 cmp ending_done,0
 je polish_not_complete
 mov race_state,14
polish_not_complete:
 cmp race_state,8
 jb polish_game_frame
 call ui_frame
 jmp frame_complete
polish_game_frame:
 cmp stage_load_requested,0
 je frame_no_load
 mov cs:keys_edge,0
 call load_stage_working_set
 call ui_after_stage
 cmp race_state,8
 jae frame_complete
frame_no_load:
 cmp exit_flag,0
 jne quit
 cli
 call engine_refresh_display
 call engine_copy_snapshot
 call engine_score_snapshot
 mov ax,stage_index
 mov frame_stage_index,ax
 mov ax,time_left
 mov frame_time_left,ax
 mov ax,score
 mov frame_score,ax
 mov ax,race_state
 mov frame_race_state,ax
 mov ax,ending_ticks
 mov frame_ending_ticks,ax
 mov ax,branch_choice
 mov frame_branch_choice,ax
 mov ax,crash_ticks
 mov frame_crash_ticks,ax
 mov ax,checkpoint_ticks
 mov frame_checkpoint_ticks,ax
 mov ax,pause_flag
 mov frame_pause_flag,ax
 mov ax,ending_done
 mov frame_ending_done,ax
 mov ax,player_x
 mov frame_player,ax
 mov ax,world
 mov frame_world,ax
 mov ax,speed
 mov frame_speed,ax
 mov ax,curve
 mov frame_curve,ax
 mov ax,hill
 mov frame_hill,ax
 mov ax,course_index
 mov frame_course,ax
 mov bx,40
 mul bx
 mov frame_material,ax
 mov ax,stage_distance
 cmp race_state,3
 jne source_frame_distance_ready
 mov ax,16383
source_frame_distance_ready:
 mov frame_distance,ax
 mov frame_fork,0
 cmp stage_index,4
 jae frame_fork_ready
 cmp ax,12032
 jb frame_fork_ready
 mov frame_fork,1
frame_fork_ready:
 mov ax,gear_auto
 mov frame_gear_auto,ax
 mov ax,gear_state
 mov frame_gear_state,ax
 mov ax,keys
 mov frame_keys,ax
 mov ax,input_tick
 mov frame_input_tick,ax
 mov ax,cs
 mov es,ax
 mov si,offset objects
 mov di,offset frame_objects
 mov cx,24
 rep movsw
 mov si,offset object_yw
 mov di,offset frame_object_yw
 mov cx,8
 rep movsw
 mov si,offset engine_object_x
 mov di,offset engine_frame_x
 mov cx,24
 rep movsw
 sti
 mov ax,0b800h
 mov es,ax
 call draw_road
 cmp frame_race_state,3
 jne render_driving_scene
 mov sprite_depth,0ffffh
 call draw_ending
 jmp player_drawn
render_driving_scene:
 call draw_objects
 call engine_draw_actors
 jc player_drawn
 mov sprite_depth,0ffffh
 ; Latch the lean pose late for visible response; world/road stay frame-stable.
 cli
 mov ax,keys
 mov frame_keys,ax
 mov ax,input_tick
 mov frame_input_tick,ax
 sti
 mov sprite_id,0
 test frame_keys,4
 jz player_right
 mov sprite_id,2
player_right:
 test frame_keys,8
 jz player_selected
 mov sprite_id,1
player_selected:
 mov ax,sprite_id
 or ax,ax
 jnz steer_active
 mov steer_dir,0
 jmp steer_done
steer_active:
 cmp ax,steer_dir
 je steer_same
 mov steer_dir,ax
 mov ax,ticks
 mov steer_since,ax
 jmp steer_done
steer_same:
 mov ax,ticks
 sub ax,steer_since
 cmp ax,32
 jb steer_done
 add sprite_id,70
steer_done:
 cmp passenger_hair,0
 je player_hair_ready
 cmp sprite_id,71
 jae hair_hard
 add sprite_id,68
 jmp player_hair_ready
hair_hard:
player_hair_ready:
 mov ax,sprite_id
 mov frame_player_sprite,ax
 mov sprite_x,60
 mov sprite_y,183
 cmp player_fast,0
 jne compiled_player
 call draw_sprite
 jmp player_drawn
compiled_player:
 call draw_player_fast
player_drawn:
 call draw_hud
 call ui_countdown
 call present
 mov ax,frame_input_tick
 mov display_input_tick,ax
 mov ax,ticks
 mov frame_tick,ax
frame_complete:
 inc frames
pace:
 mov ax,ticks
 sub ax,last_frame_tick
 cmp ax,2
 jae pace_done
 hlt
 jmp pace
pace_done:
 mov ax,ticks
 mov last_frame_tick,ax
 jmp frame_loop
present:
 ; EX manual pp.19/22: even processor/CRT pages provide 32KB banking.
 ; Change the displayed page only during vertical retrace; never draw into it.
 mov present_active,1
 mov dx,3dah
present_leave_blank:
 in al,dx
 test al,8
 jnz present_leave_blank
present_wait_blank:
 in al,dx
 test al,8
 jz present_wait_blank
present_flip:
 mov ax,draw_page
 mov front_page,ax
 xor ax,2
 mov draw_page,ax
 mov cl,3
 shl al,cl
 or ax,front_page
 or al,0c0h
 mov dx,3dfh
 out dx,al
 mov present_active,0
 inc flips
 ret
present_active dw 0
flips_late dw 0
load_error:
 mov ax,3
 int 10h
 mov ax,cs
 mov ds,ax
 mov dx,offset error_message
 mov ah,9
 int 21h
 mov ax,4c01h
 int 21h
quit:
 cli
 xor ax,ax
 mov es,ax
 mov ax,word ptr old8
 mov es:[20h],ax
 mov ax,word ptr old8+2
 mov es:[22h],ax
 mov ax,word ptr old9
 mov es:[24h],ax
 mov ax,word ptr old9+2
 mov es:[26h],ax
 mov al,36h
 out 43h,al
 xor ax,ax
 out 40h,al
 out 40h,al
 sti
 call mute_psg
 mov ax,3
 int 10h
 cmp finish_reason,3
 jne quit_success
 mov dx,offset runtime_error_text
 mov ah,9
 int 21h
 mov ax,4c01h
 int 21h
quit_success:
 mov ax,4c00h
 int 21h
runtime_error_text db 'OutRun: course/ending data could not be read. Check all BIN files.',13,10,'$'
fill_bytes:
 jcxz fill_done
 test di,1
 jz fill_phase_ready
 xchg al,ah
fill_phase_ready:
 test cx,1
 jz fill_even
 stosb
 xchg al,ah
 dec cx
fill_even:
 shr cx,1
 ; A bounded <=160-byte span masks IRQs for the REP instruction only.
 ; This avoids the local emulator restart defect and retains fast 8088 REP.
 pushf
 cli
 rep stosw
 popf
fill_done:
 ret

row_fill_bytes:
 jcxz row_fill_done
 test di,1
 jz row_fill_phase_ready
 xchg al,ah
row_fill_phase_ready:
 test cx,1
 jz row_fill_even
 stosb
 xchg al,ah
 dec cx
row_fill_even:
 shr cx,1
 ; A bounded <=160-byte span masks IRQs for the REP instruction only.
 ; This avoids the local emulator restart defect and retains fast 8088 REP.
 rep stosw
row_fill_done:
 ret

camera_sky_call dd 0
draw_camera_sky:
 mov ax,segments[32]
 mov word ptr camera_sky_call+2,ax
 call dword ptr camera_sky_call
 ret
draw_road:
 call build_source_rows
 call begin_source_sky_frame
 ; Common native palettes use one solid road word and one edge word.
 ; Keep the original per-row path if any future palette separates them.
 mov frame_constant_material,0
 mov ax,road_colors
 cmp ax,road_colors[2]
 jne source_material_initialized
 cmp ax,road_colors[4]
 jne source_material_initialized
 cmp ax,road_colors[6]
 jne source_material_initialized
 mov road_material,ax
 mov ax,edge_colors
 cmp ax,edge_colors[2]
 jne source_material_initialized
 mov edge_material,ax
 mov frame_constant_material,1
source_material_initialized:
 cmp frame_source_width,0
 jne common_road_fallback
 test frame_source_flags,1
 jnz common_road_fallback
 cmp frame_constant_material,1
 je draw_common_road
common_road_fallback:
 mov bp,frame_skip_sky_rows
 mov si,bp
 shl si,1
 shl si,1
 add si,offset road_row_buffer
 mov frame_fast_rows,0
 cmp frame_constant_material,1
 jne road_row
 mov frame_fast_rows,1
 jmp fast_row
road_row:
 lodsw
 mov row_centre,ax
 lodsw
 mov row_half,ax
 mov bx,ax
 lodsw
 mov row_depth,ax
 lodsw
 mov row_lane,ax
 cmp row_depth,0
 jne source_row_has_depth
 ; A sky row has no camera translation or carriageway separation.
 mov bx,bp
 mov ax,row_centre
 mov curve_rows[bx],ax
 mov centres[bx],ax
 mov widths[bx],0
 test frame_source_flags,1
 jz source_sky_right_ready
 neg ax
 add ax,160
source_sky_right_ready:
 mov centres_right[bx],ax
 jmp source_sky_row
source_row_has_depth:
 mov bx,bp
 mov ax,source_lateral_pixels[bx]
 sub row_centre,ax
 mov bx,bp
 mov ax,row_centre
 mov curve_rows[bx],ax
 mov dx,row_half
 mov widths[bx],dx
 call compute_row_split
 mov bx,bp
 mov ax,row_centre
 sub ax,row_split_pixels
 mov centres[bx],ax
 mov ax,row_centre_right
 mov centres_right[bx],ax
 cmp row_half,0
 jne source_ground_valid
source_sky_row:
 mov bx,bp
 add bx,32
 mov di,screen_rows[bx]
 ; All original solid-fill codes select sky palette entries (colorbase3).
 ; They are not roadside ground colors.
 call source_sky_may_skip
 jc row_done
 call draw_source_sky_row
 jmp row_done
source_ground_fill:
 mov ax,ground_rows[bp]
 mov cx,160
 call row_fill_bytes
 jmp row_done
source_ground_valid:
 call source_mark_road_row
 mov ax,ground_rows[bp]
 mov ground,ax
 cmp frame_constant_material,1
 je cached_material_ready
 mov ax,row_depth
 add ax,frame_world
 xor bx,bx
 test ax,40h
 jz cached_phase_ready
 mov bx,2
cached_phase_ready:
 test bp,2
 jz cached_row_phase_ready
 add bx,4
cached_row_phase_ready:
 mov ax,road_colors[bx]
 mov road_material,ax
 and bx,2
 mov ax,edge_colors[bx]
 mov edge_material,ax
cached_material_ready:
 mov bx,bp
 mov ax,row_left
 cmp ax,160
 jbe left_clamped
 mov ax,160
 jg left_clamped
 xor ax,ax
left_clamped:
 mov row_left,ax
 mov dx,row_right
 cmp dx,160
 jbe right_clamped
 mov dx,160
 jg right_clamped
 xor dx,dx
right_clamped:
 mov row_right,dx
 add bx,32
 mov di,screen_rows[bx]
 pushf
 cli
 mov cx,ax
 mov ax,ground
 call row_fill_bytes
 cmp row_gap_active,0
 je single_road_span
 mov cx,row_gap_left
 sub cx,row_left
 mov ax,road_material
 call row_fill_bytes
 mov cx,row_gap_right
 sub cx,row_gap_left
 mov ax,ground
 call row_fill_bytes
 mov cx,row_right
 sub cx,row_gap_right
 mov ax,road_material
 call row_fill_bytes
 jmp road_spans_done
single_road_span:
 mov cx,row_right
 sub cx,row_left
 mov ax,road_material
 call row_fill_bytes
road_spans_done:
 mov cx,160
 sub cx,row_right
 mov ax,ground
 call row_fill_bytes
 popf
 cmp row_half,3
 jbe row_done
 mov dx,screen_rows[bx]
 mov ax,edge_material
edge_colour:
 mov di,row_left
 cmp di,0
 jle no_left_edge
 cmp di,160
 jge no_left_edge
 add di,dx
 mov es:[di],al
no_left_edge:
 mov di,row_right
 cmp di,1
 jle no_right_edge
 cmp di,160
 jge no_right_edge
 add di,dx
 mov es:[di-1],al
no_right_edge:
 cmp row_gap_active,0
 je inner_edges_done
 call fork_inner_edges
inner_edges_done:
 mov ax,row_depth
 add ax,frame_world
 test ax,40h
 jz row_done
 mov ax,row_half
 cmp ax,10
 jbe row_done
 mov ax,row_lane
 mov dx,screen_rows[bx]
 ; Split==0 always retains its original common-centre lane marks.
 cmp row_split_pixels,0
 je normal_lane_marks
 cmp row_single_visible,0
 jne local_lane_marks
 call fork_lane_marks
 jmp row_done
normal_lane_marks:
 mov cx,row_centre
 jmp single_lane_centre_ready
local_lane_marks:
 mov cx,row_local_centre
single_lane_centre_ready:
 mov di,cx
 sub di,ax
 call lane_mark
 mov di,cx
 add di,ax
 call lane_mark
row_done:
 add bp,2
 cmp bp,336
 jae road_end
 cmp frame_fast_rows,0
 jne fast_row
 jmp road_row
road_end:
 ret
FAST_MARK macro
 local skip

 cmp di,160
 jae skip
 mov byte ptr es:[bx+di],0ffh
skip:
endm
FAST_FILL macro pat
 local ev, done
 mov ax,pat
 test di,1
 jz ev
 xchg al,ah
ev:
 shr cx,1
 rep stosw
 jnc done
 stosb
done:
endm
frame_fast_rows dw 0
fast_mode dw 0
ifdef ROWSTAT
fast_stat dw 8 dup(0)
endif
STAT macro k
 ifdef ROWSTAT
 inc word ptr fast_stat[k*2]
 endif
endm
fast_row:
 lodsw
 mov dx,ax
 lodsw
 mov bx,ax
 lodsw
 or ax,ax
 jnz fast_has_depth
 lodsw
 mov curve_rows[bp],dx
 mov centres[bp],dx
 mov widths[bp],0
 mov ax,dx
 test frame_source_flags,1
 jz fast_sky_right
 neg ax
 add ax,160
fast_sky_right:
 mov centres_right[bp],ax
 jmp fast_sky
fast_has_depth:
 lodsw
 sub dx,source_lateral_pixels[bp]
 mov curve_rows[bp],dx
 mov widths[bp],bx
 mov ax,source_split_pixels[bp]
 mov di,dx
 sub di,ax
 mov centres[bp],di
 add dx,ax
 test frame_source_flags,1
 jz fast_cr_ready
 mov cx,[si-8]
 sub cx,80
 shl cx,1
 sub dx,cx
fast_cr_ready:
 mov centres_right[bp],dx
 or bx,bx
 jz fast_sky
 cmp di,dx
 jg fast_escape
 mov fast_mode,0
 mov ax,dx
 sub ax,bx
 mov cx,di
 add cx,bx
 cmp ax,cx
 jg fast_gap
fast_plain:
 STAT 0
 mov ax,di
 sub ax,bx
 add bx,dx
fast_clamp:

 cmp ax,160
 jbe fast_l2
 mov ax,160
 jg fast_l2
 xor ax,ax
fast_l2:

 cmp bx,160
 jbe fast_r2
 mov bx,160
 jg fast_r2
 xor bx,bx
fast_r2:
 mov row_left,ax
 mov row_right,bx
 mov cx,ax
 mov di,bp
 add di,frame_sky_mask_base
 mov word ptr sky_row_tokens[di],0ffffh
 mov word ptr sky_dirty_spans[di],00a0h
 mov di,screen_rows[bp+32]
 pushf
 cli
 cmp fast_mode,1
 je fast_gap_fill
 FAST_FILL ground_rows[bp]
 mov cx,row_right
 sub cx,row_left

 FAST_FILL road_material
 mov cx,160
 sub cx,row_right
 FAST_FILL ground_rows[bp]
 jmp fast_filled
fast_gap_fill:
 FAST_FILL ground_rows[bp]
 mov cx,row_gap_left
 sub cx,row_left
 FAST_FILL road_material
 mov cx,row_gap_right
 sub cx,row_gap_left
 FAST_FILL ground_rows[bp]
 mov cx,row_right
 sub cx,row_gap_right
 FAST_FILL road_material
 mov cx,160
 sub cx,row_right
 FAST_FILL ground_rows[bp]
fast_filled:
 popf
 mov bx,di
 sub bx,160
 cmp word ptr widths[bp],3
 jbe fast_done
 mov al,byte ptr edge_material
 mov di,row_left
 cmp di,0
 jle fast_e1
 cmp di,160
 jge fast_e1
 mov es:[bx+di],al
fast_e1:
 mov di,row_right
 cmp di,1
 jle fast_e2
 cmp di,160
 jge fast_e2
 mov es:[bx+di-1],al
fast_e2:
 cmp fast_mode,1
 jne fast_no_inner
 mov dx,bx
 call fork_inner_edges
fast_no_inner:
 mov ax,depth_rows[bp]
 add ax,frame_world
 test ax,40h
 jz fast_done
 cmp word ptr widths[bp],10
 jbe fast_done
 mov ax,[si-2]
 cmp word ptr source_split_pixels[bp],0
 je fast_normal_marks
 cmp fast_mode,2
 je fast_local_marks
 mov di,centres[bp]
 sub di,ax
 FAST_MARK
 mov di,centres[bp]
 add di,ax
 FAST_MARK
 mov di,centres_right[bp]
 sub di,ax
 FAST_MARK
 mov di,centres_right[bp]
 add di,ax
 FAST_MARK
 jmp fast_done
fast_local_marks:
 mov cx,row_local_centre
 jmp fast_two_marks
fast_normal_marks:
 mov cx,curve_rows[bp]
fast_two_marks:
 mov di,cx
 sub di,ax
 FAST_MARK
 mov di,cx
 add di,ax
 FAST_MARK
fast_done:
 add bp,2
 cmp bp,336
 jb fast_row
 ret
fast_escape:
 STAT 3
 sub si,8
 jmp road_row
fast_sky:
 STAT 4
 mov ax,bp
 shr ax,1
 add ax,16
 sub ax,frame_sky_offset
 cmp ax,0
 jge fs1
 xor ax,ax
fs1:
 cmp ax,183
 jle fs2
 mov ax,183
fs2:
 mov bx,ax
 shl bx,1
 mov dx,bp
 add dx,frame_sky_mask_base
 mov ds,segments[32]
 mov ax,[bx]
 add bx,cs:sky_ptr_base
 mov cx,[bx]
 mov bx,cs
 mov ds,bx
 mov bx,dx
 mov dx,sky_row_tokens[bx]
 cmp ax,dx
 jne fs_whole
 mov dx,sky_dirty_spans[bx]
 mov word ptr sky_dirty_spans[bx],00a0h
 cmp dl,dh
 jae fast_done
 mov word ptr camera_sky_call,ax
 mov sky_raw_row,cx
 xor ax,ax
 mov al,dl
 mov sky_restore_left,ax
 mov al,dh
 sub al,dl
 mov sky_restore_count,ax
 jmp fs_draw
fs_whole:
 mov sky_row_tokens[bx],ax
 mov word ptr sky_dirty_spans[bx],00a0h
 mov word ptr camera_sky_call,ax
 mov sky_raw_row,cx
 mov sky_restore_left,0
 mov sky_restore_count,160
fs_draw:
 mov di,screen_rows[bp+32]
 call draw_source_sky_row
 jmp fast_done
; Separated carriageways (same decisions as compute_general_split): AX=cx2, CX=dx2 in pixels.
fast_gap:
 cmp cx,160
 jbe fg2
 mov cx,160
 jg fg2
 xor cx,cx
fg2:
 cmp ax,160
 jbe fg4
 mov ax,160
 jg fg4
 xor ax,ax
fg4:
 cmp ax,160
 jne fg_left_test
 cmp cx,4
 jb fg_active
 mov fast_mode,2
 STAT 2
 mov row_local_centre,di
 mov ax,di
 sub ax,bx
 mov bx,cx
 jmp fast_clamp
fg_left_test:
 cmp cx,0
 jne fg_active
 mov fast_mode,2
 STAT 2
 mov row_local_centre,dx
 add bx,dx
 jmp fast_clamp
fg_active:
 STAT 1
 mov fast_mode,1

 mov row_gap_left,cx

 mov row_gap_right,ax
 mov ax,di
 sub ax,bx
 add bx,dx
 jmp fast_clamp

; All operations retain the general renderer's integer floors and clipping.
; Preconditions: zero separation, identical curves, invariant road/edge words.
draw_common_road:
 mov bp,frame_skip_sky_rows
 mov si,bp
 shl si,1
 shl si,1
 add si,offset road_row_buffer
common_road_row:
 lodsw
 cmp word ptr [si+2],0
 je common_camera_ready
 sub ax,source_lateral_pixels[bp]
common_camera_ready:
 mov curve_rows[bp],ax
 mov centres[bp],ax
 mov centres_right[bp],ax
 mov dx,ax
 lodsw
 mov widths[bp],ax
 mov bx,ax
 add si,4
 or ax,ax
 jz common_sky_row
 ; BX=half, DX=centre. Clamp the single strip's two endpoints once.
 call source_mark_road_row
 mov ax,dx
 sub ax,bx
 cmp ax,160
 jbe common_left_valid
 mov ax,160
 jg common_left_valid
 xor ax,ax
common_left_valid:
 add dx,bx
 cmp dx,160
 jbe common_right_valid
 mov dx,160
 jg common_right_valid
 xor dx,dx
common_right_valid:
 mov bx,ax
 mov di,screen_rows[bp+32]
 pushf
 cli
 mov cx,bx
 mov ax,ground_rows[bp]
 call row_fill_bytes
 mov cx,dx
 sub cx,bx
 mov ax,road_material
 call row_fill_bytes
 mov cx,160
 sub cx,dx
 mov ax,ground_rows[bp]
 call row_fill_bytes
 popf
 cmp widths[bp],3
 jbe common_row_done
 ; DI finishes at row-end; restore the row base without another table read.
 sub di,160
 mov al,byte ptr edge_material
 cmp bx,0
 jle common_no_left_edge
 cmp bx,160
 jge common_no_left_edge
 mov es:[di+bx],al
common_no_left_edge:
 cmp dx,1
 jle common_no_right_edge
 cmp dx,160
 jge common_no_right_edge
 push bx
 mov bx,dx
 mov es:[di+bx-1],al
 pop bx
common_no_right_edge:
 mov ax,depth_rows[bp]
 add ax,frame_world
 test ax,40h
 jz common_row_done
 cmp widths[bp],10
 jbe common_row_done
 mov dx,di
 mov ax,[si-2]
 mov cx,centres[bp]
 mov di,cx
 sub di,ax
 call lane_mark
 mov di,cx
 add di,ax
 call lane_mark
 jmp common_row_done
common_sky_row:
 mov di,screen_rows[bp+32]
 call source_sky_may_skip
 jc common_row_done
 call draw_source_sky_row
common_row_done:
 add bp,2
 cmp bp,336
 jb common_road_row
 ret

lane_mark:
 cmp di,160
 jae lane_done
 add di,dx
 mov byte ptr es:[di],0ffh
lane_done:
 ret
draw_objects:
 xor si,si
 xor di,di
sort_copy:
 mov ax,frame_objects[si]
 mov sort_depths[di],ax
 add si,6
 add di,2
 cmp di,16
 jb sort_copy
 mov sort_index,8
sort_next:
 mov sort_z,0
 mov sort_best,0ffffh
 xor bx,bx
 xor si,si
select_object:
 mov ax,sort_depths[si]
 cmp ax,sort_z
 jbe object_not_best
 mov sort_z,ax
 mov sort_best,bx
 mov sort_slot,si
object_not_best:
 add bx,6
 add si,2
 cmp bx,48
 jb select_object
 mov bx,sort_best
 cmp bx,0ffffh
 je objects_done
 mov entity_index,bx
 mov ax,frame_objects[bx]
 mov si,sort_slot
 mov sort_depths[si],0
 cmp ax,48
 jb object_done
 cmp ax,2040
 jae object_done
 call project_source_object
 mov si,sort_slot
 mov ax,engine_frame_z[si]
 mov object_source_z,ax
 mov ax,engine_frame_y[si]
 push ax
 mov ax,object_source_z
 mov sprite_depth,ax
 pop ax
 mov object_y,ax
 mov object_half,dx
 cmp ax,16
 jl object_done
 cmp ax,184
 jb source_object_row_valid
 mov ax,183
source_object_row_valid:
 sub ax,16
 shl ax,1
 mov bx,ax
 mov si,sort_slot
 mov ax,engine_frame_x[si]
 mov cl,ENGINE_PIXEL_SHIFT
 sar ax,cl
 add ax,ENGINE_CENTRE_BYTES
 mov sprite_x,ax
 mov bx,entity_index
 cmp frame_objects[bx+4],256
 jae select_stage_scene
 cmp frame_objects[bx+4],1
 je select_palm
 mov ax,object_half
 shl ax,1
 xor dx,dx
 mov cx,5
 div cx
 cmp ax,8
 jae van_min
 mov ax,8
van_min:
 cmp ax,40
 jbe van_max
 mov ax,40
van_max:
 sub ax,8
 shr ax,1
 add ax,VAN_FIRST
 cmp frame_objects[bx+4],2
 jne select_porsche
 add ax,31
 jmp object_selected
select_porsche:
 cmp frame_objects[bx+4],3
 jne object_selected
 add ax,48
 jmp object_selected
select_stage_scene:
 mov ax,frame_objects[bx+4]
 sub ax,256
 cmp object_source_z,44
 jb scene_scale_selected
 inc ax
 cmp object_source_z,96
 jb scene_scale_selected
 inc ax
 cmp object_source_z,192
 jb scene_scale_selected
 inc ax
 cmp object_source_z,352
 jb scene_scale_selected
 inc ax
scene_scale_selected:
 or ax,8000h
 jmp object_selected
select_palm:
 mov ax,object_half
 xor dx,dx
 mov cx,2
 div cx
 cmp ax,6
 jae palm_min
 mov ax,6
palm_min:
 cmp ax,32
 jbe palm_max
 mov ax,32
palm_max:
 sub ax,6
 shr ax,1
 add ax,PALM_FIRST
object_selected:
 mov sprite_id,ax
 mov ax,object_y
 mov sprite_y,ax
 ; The original handler controls left/center/right and top/center/bottom anchors.
 mov scene_anchor,8
 push ds
 mov ds,segments[SPRITE_SEGMENT]
 mov bx,cs:sprite_id
 test bx,8000h
 jz sprite_bank_chosen_1
 and bx,7fffh
 xor ax,ax
 mov al,cs:scene_anchor_map[bx]
 mov cs:scene_anchor,ax
 mov ds,cs:segments[SCENE_SEGMENT]
 cmp byte ptr cs:scene_bank_map[bx],0
 je sprite_bank_chosen_1
 mov ds,cs:segments[34]
sprite_bank_chosen_1:
 shl bx,1
 mov si,[bx]
 mov ax,[si]
 and ax,3fffh
 mov dx,[si+2]
 pop ds
 mov bx,scene_anchor
 and bx,3
 cmp bx,1
 je source_anchor_x_done
 cmp bx,2
 je source_anchor_x_right
 shr ax,1
source_anchor_x_right:
 sub sprite_x,ax
source_anchor_x_done:
 mov bx,scene_anchor
 and bx,12
 cmp bx,8
 je source_anchor_y_done
 cmp bx,4
 je source_anchor_y_top
 shr dx,1
 jmp source_anchor_y_adjust
source_anchor_y_top:
 dec dx
source_anchor_y_adjust:
 add sprite_y,dx
source_anchor_y_done:
 call draw_sprite
object_done:
 dec sort_index
 jne sort_next
objects_done:
 ret
draw_sprite:
 push ds
 mov ds,segments[SPRITE_SEGMENT]
 mov bx,cs:sprite_id
 test bx,8000h
 jz sprite_bank_chosen_2
 and bx,7fffh
 mov ds,cs:segments[SCENE_SEGMENT]
 cmp byte ptr cs:scene_bank_map[bx],0
 je sprite_bank_chosen_2
 mov ds,cs:segments[34]
sprite_bank_chosen_2:
 shl bx,1
 mov si,[bx]
 mov cs:sprite_rle,0
 lodsw
 test ax,4000h
 jz sprite_width_ready
 mov cs:sprite_rle,1
 and ax,3fffh
sprite_width_ready:
 mov cs:sprite_width,ax
 lodsw
 mov cs:sprite_height,ax
 call mark_source_sprite_dirty
 cmp cs:sprite_rle,0
 je normal_sprite_data
 call draw_rle_sprite
 jmp sprite_done
normal_sprite_data:
 mov dx,cs:sprite_x
 cmp dx,0
 jl sprite_clipped
 mov cx,dx
 add cx,cs:sprite_width
 cmp cx,160
 ja sprite_clipped
 mov bx,cs:sprite_y
 sub bx,ax
 inc bx
 cmp bx,16
 jl sprite_clipped
 cmp cs:sprite_y,184
 jae sprite_clipped
 cmp cs:sprite_id,3
 jb sprite_generic_full
 cmp cs:sprite_id,34
 jae sprite_generic_full
 cmp cs:object_fast,0
 je sprite_generic_full
 ; Compiled opaque/masked code cannot omit rows: use it only when every
 ; sprite row passes the same source-depth occlusion check as the raw path.
 push bx
 push cx
 shl bx,1
 mov cx,cs:sprite_height
compiled_source_visibility:
 call source_row_occluded
 jc compiled_source_occluded
 add bx,2
 loop compiled_source_visibility
 pop cx
 pop bx
 jmp compiled_source_clear
compiled_source_occluded:
 pop cx
 pop bx
 jmp sprite_generic_full
compiled_source_clear:
 mov di,bx
 shl di,1
 mov di,cs:screen_rows[di]
 add di,dx
 mov ax,bx
 and ax,3
 shl ax,1
 shl ax,1
 mov bx,cs:sprite_id
 mov cl,4
 shl bx,cl
 add bx,ax
 mov si,cs:object_code_table[bx]
 mov ax,cs:segments[si]
 mov word ptr cs:player_call+2,ax
 mov ax,cs:object_code_table[bx+2]
 mov word ptr cs:player_call,ax
 pop ds
 call dword ptr player_call
 ret
sprite_generic_full:
 shl bx,1
 mov bp,cs:sprite_height
sprite_row:
 call source_row_occluded
 jc sprite_done
 mov di,cs:screen_rows[bx]
 add di,cs:sprite_x
 mov cx,cs:sprite_width
sprite_pixel:
 lodsw
 and al,es:[di]
 or al,ah
 stosb
 loop sprite_pixel
 jmp sprite_raw_row_done
sprite_skip_occluded_row:
 mov ax,cs:sprite_width
 shl ax,1
 add si,ax
sprite_raw_row_done:
 add bx,2
 dec bp
 jne sprite_row
sprite_done:
 pop ds
 ret
sprite_clipped:
 ; Slow fallback clips the rectangle instead of dropping the whole sprite.
 mov cs:sprite_clip_left,0
 mov ax,cs:sprite_x
 cmp ax,0
 jge clip_left_ready
 neg ax
 mov cs:sprite_clip_left,ax
clip_left_ready:
 mov ax,cs:sprite_width
 sub ax,cs:sprite_clip_left
 jle sprite_done
 mov dx,cs:sprite_x
 cmp dx,0
 jge clipped_x_ready
 xor dx,dx
clipped_x_ready:
 mov cs:sprite_top,dx
 mov cx,160
 sub cx,dx
 cmp ax,cx
 jle clipped_width_ready
 mov ax,cx
clipped_width_ready:
 cmp ax,0
 jle sprite_done
 mov cs:sprite_visible_width,ax
 mov ax,cs:sprite_width
 sub ax,cs:sprite_visible_width
 shl ax,1
 mov cs:sprite_skip,ax
 mov bx,cs:sprite_y
 sub bx,cs:sprite_height
 inc bx
 mov cs:sprite_clip_top,0
 cmp bx,16
 jge clipped_top_ready
 mov ax,16
 sub ax,bx
 mov cs:sprite_clip_top,ax
 mov bx,16
clipped_top_ready:
 mov ax,cs:sprite_height
 sub ax,cs:sprite_clip_top
 jle sprite_done
 mov dx,184
 sub dx,bx
 cmp ax,dx
 jle clipped_height_ready
 mov ax,dx
clipped_height_ready:
 cmp ax,0
 jle sprite_done
 mov bp,ax
 mov ax,cs:sprite_clip_top
 mul cs:sprite_width
 add ax,cs:sprite_clip_left
 shl ax,1
 add si,ax
 shl bx,1
clipped_row:
 call source_row_occluded
 jc sprite_done
 mov di,cs:screen_rows[bx]
 add di,cs:sprite_top
 mov cx,cs:sprite_visible_width
clipped_pixel:
 lodsw
 and al,es:[di]
 or al,ah
 stosb
 loop clipped_pixel
 add si,cs:sprite_skip
 jmp clipped_row_advance
clipped_occluded_row:
 mov ax,cs:sprite_width
 shl ax,1
 add si,ax
clipped_row_advance:
 add bx,2
 dec bp
 jne clipped_row
 pop ds
 ret
include hud.inc
draw_player_fast:
 mov sprite_x,60
 mov sprite_width,40
 mov sprite_height,32
 mov sprite_y,183
 call mark_source_sprite_dirty
 mov di,screen_rows[304]
 add di,60
 mov bx,sprite_id
 cmp bx,68
 jb player_code_pose
 sub bx,65
player_code_pose:
 shl bx,1
 push ds
 mov ds,segments[PLAYER_SEGMENT]
 mov ax,[bx]
 pop ds
 mov word ptr player_call,ax
 mov ax,segments[PLAYER_SEGMENT]
 mov word ptr player_call+2,ax
 call dword ptr player_call
 ret
keyboard_irq:
 push ax
 push bx
 push ds
 push cs
 pop ds
 in al,60h
 mov bl,al
 in al,61h
 mov ah,al
 or al,80h
 out 61h,al
 mov al,ah
 out 61h,al
 mov al,bl
 and al,7fh
 xor bh,bh
 cmp al,48h
 jne key_down
 mov bh,1
key_down:
 cmp al,50h
 jne key_left
 mov bh,2
key_left:
 cmp al,4bh
 jne key_right
 mov bh,4
key_right:
 cmp al,4dh
 jne key_apply
 mov bh,8
key_apply:
 test bl,80h
 jnz key_release
 or byte ptr keys,bh
 call key_edge_set
 cmp race_state,8
 jb polish_keyboard_game
 cmp al,32h
 je polish_keyboard_game
 xor ah,ah
 mov ui_key,ax
 jmp key_record_done
polish_keyboard_game:
 cmp al,1
 jne key_music
 mov exit_flag,1
key_music:
 cmp al,32h
 jne key_record
 xor music,1
 jne key_record
 call mute_psg
 cmp race_state,8
 jne key_record
 call splash_quiet
key_record:
 cmp al,13h
 jne key_enter
 mov reset_requested,1
key_enter:
 cmp al,1ch
 jne key_pause
 cmp race_state,0
 jne key_pause
 mov race_state,1
key_pause:
 cmp al,19h
 jne key_radio
 xor pause_flag,1
 mov cs:keys_edge,0
key_radio:
 cmp al,22h
 jne key_auto_gear
 mov gear_auto,0
 xor gear_state,1
key_auto_gear:
 cmp al,1eh
 jne key_low_gear
 xor gear_auto,1
key_low_gear:
 cmp al,26h
 jne key_high_gear
 mov gear_auto,0
 mov gear_state,0
key_high_gear:
 cmp al,23h
 jne key_radio_select
 mov gear_auto,0
 mov gear_state,1
key_radio_select:
 ; Catalog selection uses a mainline disk read in the graphical radio.
key_record_done:
 mov ax,ticks
 mov input_tick,ax
 jmp key_ack
key_release:
 not bh
 and byte ptr keys,bh
key_ack:
 mov al,20h
 out 20h,al
 pop ds
 pop bx
 pop ax
 iret
timer_irq:
 push ax
 push bx
 push cx
 push dx
 push si
 push di
 push bp
 push ds
 push es
 push cs
 pop ds
 inc ticks
timer_boundary:
 cmp music,0
 jne sound_enabled
 ; Muted feedback expires instead of replaying a stale collision/checkpoint.
 mov effect_ticks,0
 mov checkpoint_sound,0
 mov effect_noise_cache,0ffffh
 jmp sound_done
sound_enabled:
sound_music_call:
 call music_tick
sound_effects_call:
 call effects_tick
sound_done:
 call physics
 add bios_acc,18643
 jc chain_bios
 mov al,20h
 out 20h,al
 pop es
 pop ds
 pop bp
 pop di
 pop si
 pop dx
 pop cx
 pop bx
 pop ax
 iret
chain_bios:
 mov bios_di_before,di
 mov bios_cx_before,cx
 pushf
 call dword ptr cs:[old8]
 cmp di,bios_di_before
 je bios_di_ok
 inc bios_di_changes
bios_di_ok:
 cmp cx,bios_cx_before
 je bios_cx_ok
 inc bios_cx_changes
bios_cx_ok:
 pop es
 pop ds
 pop bp
 pop di
 pop si
 pop dx
 pop cx
 pop bx
 pop ax
 iret
physics:
 cmp speed,20
 jae passenger_moving
 mov passenger_hair,0
 jmp passenger_ready
passenger_moving:
 mov ax,speed
 mov cl,5
 shr ax,cl
 mov bx,9
 sub bx,ax
 shl bx,1
 inc hair_counter
 cmp hair_counter,bx
 jb passenger_ready
 mov hair_counter,0
 xor passenger_hair,1
passenger_ready:
 cmp stage_load_requested,0
 je physics_no_stage_load
 ret
physics_no_stage_load:
 cmp reset_requested,0
 je game_no_reset
 call reset_race
game_no_reset:
 cmp race_state,3
 jne game_no_ending
 cmp ending_done,0
 jne game_return
 inc ending_ticks
 mov ax,ending_ticks
 cmp ax,ending_length
 jb ending_clock_active
 mov ending_done,1
ending_clock_active:
 test ending_ticks,7
 jnz game_return
 cmp time_left,0
 je game_return
 dec time_left
 mov ax,100h
 xor dx,dx
 call engine_add_bonus
 add ending_bonus,100
 ret
game_no_ending:
 cmp pause_flag,0
 je game_unpaused
 ret
game_unpaused:
 cmp race_state,0
 jne game_countdown
 test keys,1
 jz game_return
 mov race_state,1
game_countdown:
 cmp race_state,1
 jne game_racing
 dec countdown
 jnz game_return
 mov race_state,2
 mov started,1
 mov engine_frame_counter,30
game_racing:
 cmp race_state,2
 jne game_return
 inc second_tick
 cmp second_tick,64
 jb game_second_done
 mov second_tick,0
 inc race_elapsed
game_second_done:
 cmp checkpoint_ticks,0
 je game_checkpoint_done
 dec checkpoint_ticks
game_checkpoint_done:
 jmp physics_started
game_return:
 ret
 cmp started,0
 jne physics_started
 test keys,1
 jnz begin_drive
 ret
begin_drive:
 mov started,1
physics_started:
 call engine_tick
 call update_split_near
 cmp stage_distance,16384
 jb engine_physics_done
 sub stage_distance,16384
 cmp stage_index,4
 jb next_stage
 mov race_state,7
 mov stage_load_requested,2
 mov finish_reason,1
 mov speed,0
 mov music_bank,3
 mov music_position,4
 mov music_wait,0
 ret
next_stage:
 cmp branch_choice,1
 jne fork_left
 inc route_index
fork_left:
 inc stage_index
 mov bx,stage_index
 mov al,stage_bases[bx]
 xor ah,ah
 add ax,route_index
 mov course_index,ax
 mov bx,ax
 dec bx
 xor ax,ax
 mov al,checkpoint_bonus[bx]
 add time_left,ax
 cmp time_left,99
 jbe checkpoint_capped
 mov time_left,99
checkpoint_capped:
 mov ax,1000h
 xor dx,dx
 call engine_add_bonus
 mov checkpoint_ticks,128
 mov checkpoint_sound,8
 ; Preserve the position within the chosen road across the bank handoff.
 mov ax,split_near_offset
 cmp branch_choice,1
 je checkpoint_local_x
 neg ax
checkpoint_local_x:
 sub player_x,ax
 mov ax,speed
 mov stage_resume_speed,ax
 mov stage_resume_race,1
 mov stage_load_requested,1
 mov race_state,6
 mov speed,0
 ret
engine_physics_done:
 ret
psg_out:
 out 0c0h,al
 ; Track the actual write-only PSG noise register without changing AX.
 pushf
 push ax
 mov ah,al
 and ah,0f8h
 cmp ah,0e0h
 jne psg_noise_control_ready
 xor ah,ah
 mov cs:noise_control_last,ax
psg_noise_control_ready:
 pop ax
 popf
 inc cs:psg_writes
 ifdef TRACE_PSG
 push bx
 push dx
 mov bx,cs:trace_count
 cmp bx,5000
 jae trace_full
 inc cs:trace_count
 shl bx,1
 shl bx,1
 mov dx,cs:ticks
 mov word ptr cs:trace_buffer[bx],dx
 mov cs:trace_buffer[bx+2],al
 mov dx,cs:music_bank
 mov cs:trace_buffer[bx+3],dl
trace_full:
 pop dx
 pop bx
 endif
 ret
tone:
 ; AX period (1..1023), BL channel latch. Standard SN76489 writes only.
 mov dx,ax
 and al,0fh
 or al,bl
 call psg_out
 mov ax,dx
 mov cl,4
 shr ax,cl
 call psg_out
 ret
music_tick:
 cmp music_bank,3
 je original_music_tick
 cmp rbg_enabled,0
 je original_music_tick
 jmp rbg_tick
original_music_tick:
 cmp noise_atten,15
 jae music_decay_done
 inc noise_atten
 mov ax,noise_atten
 or al,0f0h
 call psg_out
music_decay_done:
 test ticks,1
 jnz music_return
 cmp music_wait,0
 je music_record
 dec music_wait
 jnz music_return
music_record:
 push ds
 mov bx,music_bank
 shl bx,1
 mov ds,segments[bx+MUSIC_SEGMENT_BASE]
 mov si,cs:music_position
 cmp si,word ptr ds:[0]
 jb music_read
 mov si,word ptr ds:[2]
music_read:
 lodsb
 xor ah,ah
 mov cs:music_wait,ax
 mov bl,80h
 call music_voice
 mov bl,0a0h
 call music_voice
 mov bl,0c0h
 call music_voice
 lodsw
 or ax,ax
 jz music_no_drum
 push ax
 mov cs:effect_noise_cache,0ffffh
 call psg_out
 pop ax
 mov al,ah
 xor ah,ah
 and ax,15
 mov cs:noise_atten,ax
 or al,0f0h
 call psg_out
music_no_drum:
 mov cs:music_position,si
 pop ds
music_return:
 ret
music_voice:
 lodsw
 cmp bl,0c0h
 jne music_voice_not_second
 mov cs:music_second_word,ax
music_voice_not_second:
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
reset_race:
 mov engine_initialized,0
 mov word ptr engine_bonus_bcd,0
 mov word ptr engine_bonus_bcd+2,0
 mov engine_phase,0
 mov engine_pending,0
 mov engine_last_collision,0
 mov reset_requested,0
 mov stage_resume_race,0
 mov stage_load_requested,1
 mov race_state,6
 mov countdown,192
 mov started,0
 mov stage_index,0
 mov route_index,0
 mov course_index,0
 mov stage_distance,0
 mov speed,0
 mov player_x,0
 mov world,0
 mov curve,8
 mov hill,0
 mov score,0
 mov time_left,75
 mov second_tick,0
 mov race_elapsed,0
 mov finish_reason,0
 mov crash_ticks,0
 mov effect_ticks,0
 mov checkpoint_sound,0
 mov gear_state,0
 mov checkpoint_ticks,0
 mov collisions,0
 mov branch_choice,0
 mov split_near_offset,0
 mov ending_ticks,0
 mov ending_done,0
 mov ending_bonus,0
 mov offroad,0
 mov pause_flag,0
 mov music_position,4
 mov music_wait,0
 mov music_bank,0
 cmp rbg_enabled,0
 je polish_reset_song_done
 call rbg_reset
polish_reset_song_done:
 mov cs:keys_edge,0
 inc restarts
 mov objects[0],300
 mov objects[6],600
 mov objects[12],900
 mov objects[18],0
 mov objects[24],0
 mov objects[30],0
 mov objects[36],0
 mov objects[42],0
 ret
include stage_assets.inc
include fork.inc
include sprite_rle.inc
stage_bases db 0,1,3,6,10
mute_psg:
 mov al,9fh
 ifdef TRACE_PSG
 call psg_out
 else
 out 0c0h,al
 endif
 mov al,0bfh
 ifdef TRACE_PSG
 call psg_out
 else
 out 0c0h,al
 endif
 mov al,0dfh
 ifdef TRACE_PSG
 call psg_out
 else
 out 0c0h,al
 endif
 mov al,0ffh
 ifdef TRACE_PSG
 call psg_out
 else
 out 0c0h,al
 endif
 ret
error_message db 'Tandy OutRun SE: insufficient memory or missing asset bank.',13,10,'$'
include SOURCE_WIDTH.INC
include ASSETS.INC
include COURSE_META.INC
include PHYSLOC.INC
include engine_bridge.inc
include engine_game.inc
include engine_actors.inc
include crash_scale.inc
ifdef TRACE_PSG
trace_count dw 0
trace_buffer label byte
db 20000 dup(0)
endif
stack_space db 2048 dup(0)
stack_top label byte
program_end label byte
end entry
