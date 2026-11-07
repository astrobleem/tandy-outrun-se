; Source-engine states: 65 profiles, each 168 road rows + 128 object-Y samples.
frame_geo_index dw 0
source_previous_active dw 0
geo_meta dw 130 dup(0)
geo_flags db 65 dup(0)
frame_geo_a dw 512
frame_geo_b dw 1312
frame_geo_fraction dw 0
frame_source_width dw 450
frame_source_flags dw 0
frame_source_horizon dw 152
frame_sky_offset dw 0
geo_player_scale dw 0
geo_temp_a dw 0
geo_temp_flag dw 0
source_split_pixels dw 168 dup(0)
source_lateral_pixels dw 168 dup(0)
centres_right dw 168 dup(0)
curve_rows dw 168 dup(0)
depth_rows dw 168 dup(0)
object_source_z dw 0
sprite_depth dw 0ffffh
current_source_flags dw 0
source_width_old dw 0
geo_filename db 'GEO00.BIN',0

cache_source_geometry:
 push ds
 push es
 mov ds,segments[0]
 cmp word ptr ds:[0],4547h
 jne source_cache_bad
 cmp word ptr ds:[2],354fh
 jne source_cache_bad
 cmp word ptr ds:[4],52512
 jne source_cache_bad
 push cs
 pop es
 mov si,16
 mov di,offset geo_meta
 mov cx,130
 rep movsw
 mov si,276
 mov di,offset geo_flags
 mov cx,65
 rep movsb
 pop es
 pop ds
 ret
source_cache_bad:
 pop es
 pop ds
 jmp source_geometry_failed

; Input AX: native stage distance. Return source width AX, flags BX, horizon DX.
get_source_meta:
 mov dx,ax
 and dx,255
 mov frame_geo_fraction,dx
 mov cl,8
 shr ax,cl
 cmp ax,63
 jbe source_index_valid
 mov ax,63
source_index_valid:
 mov bx,ax
 mov frame_geo_index,bx
 xor ah,ah
 mov al,geo_flags[bx]
 mov current_source_flags,ax
 mov ax,bx
 mov cx,800
 mul cx
 add ax,512
 mov frame_geo_a,ax
 add ax,800
 mov frame_geo_b,ax
 shl bx,1
 shl bx,1
 mov ax,geo_meta[bx+4]
 sub ax,geo_meta[bx]
 imul frame_geo_fraction
 mov al,ah
 mov ah,dl
 add ax,geo_meta[bx]
 mov frame_source_width,ax
 push ax
 mov ax,geo_meta[bx+6]
 sub ax,geo_meta[bx+2]
 imul frame_geo_fraction
 mov al,ah
 mov ah,dl
 add ax,geo_meta[bx+2]
 mov frame_source_horizon,ax
 sub ax,152
 mov cx,200
 imul cx
 mov cx,224
 idiv cx
 mov frame_sky_offset,ax
 mov dx,frame_source_horizon
 mov bx,current_source_flags
 pop ax
 ret

build_source_rows:
 mov ax,frame_distance
 call get_source_meta
 mov frame_source_flags,bx
 and bx,1
 mov frame_fork,bx
 ; Only the road's visible Z values need affine products. No 512-entry
 ; camera/width tables are rebuilt for depths unused by this frame.
 mov ax,frame_player
 mov cx,212
 imul cx
 mov al,ah
 mov ah,dl
 mov geo_player_scale,ax
 mov word ptr source_lateral_pixels,0
 mov word ptr source_split_pixels,0
 call prepare_source_prefix
 push ds
 mov ds,segments[0]
 mov si,cs:frame_geo_a
 mov di,cs:frame_geo_b
 mov ax,bp
 shl ax,1
 add si,ax
 add di,ax
 cmp bp,336
 jae source_rows_complete
source_row_build:
 mov ax,[si]
 cmp ax,[di]
 je source_curve_interpolated
 mov ax,[di]
 sub ax,[si]
 imul cs:frame_geo_fraction
 mov al,ah
 mov ah,dl
 add ax,[si]
source_curve_interpolated:
 sar ax,1
 sar ax,1
 sar ax,1
 sar ax,1
 sar ax,1
 sar ax,1
 sar ax,1
 mov curve_rows[bp],ax
 mov ax,[si+2]
 and ax,511
 mov bx,[di+2]
 and bx,511
 cmp bx,ax
 je source_depth_interpolated
 mov cx,ax
 sub bx,ax
 mov ax,bx
 imul cs:frame_geo_fraction
 mov al,ah
 mov ah,dl
 add ax,cx
source_depth_interpolated:
 mov depth_rows[bp],ax
 or ax,ax
 jz source_row_background
 mov cx,ax
 imul cs:geo_player_scale
 mov al,ah
 mov ah,dl
 sar ax,1
 sar ax,1
 mov source_lateral_pixels[bp],ax
 cmp cs:frame_source_width,0
 jne source_width_affine_nonzero
 mov source_split_pixels[bp],0
 jmp source_width_affine_done
source_width_affine_nonzero:
 mov ax,cx
 mul cs:frame_source_width
 mov al,ah
 mov ah,dl
 shr ax,1
 shr ax,1
 mov source_split_pixels[bp],ax
source_width_affine_done:
 mov bx,cx
 shl bx,1
 mov ax,cs:source_half_width[bx]
 mov dx,cs:source_lane_width[bx]
 mov bx,bp
 shl bx,1
 shl bx,1
 mov word ptr cs:road_row_buffer[bx+2],ax
 mov word ptr cs:road_row_buffer[bx+6],dx
 mov word ptr cs:road_row_buffer[bx+4],cx
 mov ax,curve_rows[bp]
 add ax,80
 mov word ptr cs:road_row_buffer[bx],ax
 jmp source_row_built
source_row_background:
 mov bx,bp
 shl bx,1
 shl bx,1
 mov ax,curve_rows[bp]
 add ax,80
 mov word ptr cs:road_row_buffer[bx],ax
 mov word ptr cs:road_row_buffer[bx+2],0
 mov word ptr cs:road_row_buffer[bx+4],0
 cmp cs:frame_geo_fraction,128
 mov ax,[si+2]
 jb source_flag_low
 mov ax,[di+2]
source_flag_low:
 mov al,ah
 xor ah,ah
 shr ax,1
 and ax,63
 mov word ptr cs:road_row_buffer[bx+6],ax
source_row_built:
 add si,4
 add di,4
 add bp,2
 cmp bp,336
 jb source_row_build
source_rows_complete:
 pop ds
 ret

; Input AX original inverse-distance object coordinate, output Y AX/half DX.
project_source_object:
 mov cx,ax
 mov ax,24528
 xor dx,dx
 div cx
 cmp ax,511
 jbe object_source_depth_valid
 mov ax,511
object_source_depth_valid:
 mov object_source_z,ax
 mov bx,ax
 shl bx,1
 mov dx,source_half_width[bx]
 push dx
 mov cl,2
 shr ax,cl
 mov bx,ax
 add bx,672
 push ds
 mov ds,segments[0]
 mov si,cs:frame_geo_a
 add si,bx
 mov di,cs:frame_geo_b
 add di,bx
 xor ax,ax
 mov al,[si]
 mov cs:geo_temp_a,ax
 xor ax,ax
 mov al,[di]
 sub ax,cs:geo_temp_a
 imul cs:frame_geo_fraction
 mov al,ah
 mov ah,dl
 add ax,cs:geo_temp_a
 pop ds
 pop dx
 ret

draw_source_sky_row:
 cmp sky_restore_count,160
 je source_sky_full_restore
 push ds
 push si
 mov ds,cs:segments[32]
 mov si,cs:sky_raw_row
 add si,cs:sky_restore_left
 add di,cs:sky_restore_left
 mov cx,cs:sky_restore_count
 test cx,1
 jz source_sky_partial_even
 movsb
 dec cx
source_sky_partial_even:
 shr cx,1
 rep movsw
 pop si
 pop ds
 ret
source_sky_full_restore:
 call dword ptr camera_sky_call
 ret

load_source_geometry:
 mov ax,course_index
 xor dx,dx
 mov bx,10
 div bx
 add al,'0'
 mov geo_filename[3],al
 add dl,'0'
 mov geo_filename[4],dl
 mov dx,offset geo_filename
 mov ax,3d00h
 int 21h
 jc source_geometry_failed
 mov load_handle,ax
 push ds
 mov ds,segments[0]
 mov bx,cs:load_handle
 xor dx,dx
 mov cx,52512
 mov ah,3fh
 int 21h
 pop ds
 jc source_geometry_read_failed
 cmp ax,52512
 jne source_geometry_read_failed
 inc asset_reads
 mov bx,load_handle
 mov ah,3eh
 int 21h
 call cache_source_geometry
 ret
source_geometry_read_failed:
 mov bx,load_handle
 mov ah,3eh
 int 21h
source_geometry_failed:
 mov runtime_load_error,0fffeh
 mov exit_flag,1
 mov finish_reason,3
 ret

; BX is screen-Y word index. Larger source Z is nearer: foreground road
; hides pixels from a sprite farther away, while sky rows remain visible.
source_row_occluded:
 push ax
 push dx
 cmp bx,32
 jb source_row_visible
 cmp bx,368
 jae source_row_visible
 mov dx,cs:sprite_depth
 cmp dx,511
 ja source_row_visible
 add dx,8
 mov ax,cs:depth_rows[bx-32]
 cmp ax,dx
 ja source_row_hidden
source_row_visible:
 pop dx
 pop ax
 clc
 ret
source_row_hidden:
 pop dx
 pop ax
 stc
 ret

; A prefix that is zero in both endpoints stays zero under interpolation.
; Clear rows only when the all-sky prefix grows; active rows overwrite the rest.
prepare_source_prefix:
 mov ax,frame_course
 mov bx,65
 mul bx
 mov bx,ax
 add bx,frame_geo_index
 xor ax,ax
 mov al,source_first_active[bx]
 mov cl,source_first_active[bx+1]
 cmp al,cl
 jbe source_prefix_chosen
 mov al,cl
source_prefix_chosen:
 shl ax,1
 mov cx,ax
 mov bp,source_previous_active
 cmp bp,cx
 jae source_prefix_ready
source_prefix_clear:
 mov curve_rows[bp],0
 mov centres[bp],80
 mov centres_right[bp],80
 mov widths[bp],0
 mov depth_rows[bp],0
 mov bx,bp
 shl bx,1
 shl bx,1
 mov word ptr road_row_buffer[bx],80
 mov word ptr road_row_buffer[bx+2],0
 mov word ptr road_row_buffer[bx+4],0
 mov word ptr road_row_buffer[bx+6],0
 add bp,2
 cmp bp,cx
 jb source_prefix_clear
source_prefix_ready:
 mov bp,cx
 mov source_previous_active,bp
 ret
include SOURCE_ACTIVE.INC
