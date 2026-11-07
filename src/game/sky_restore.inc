; Matched-pixel optimization: the compiled sky header shares offsets for
; identical row patterns. Reuse a row only when the same page already has that
; pattern and no previous sprite touched it. Source hills may change the offset.
frame_skip_sky_rows dw 0
steer_dir dw 0
steer_since dw 0
sky_ptr_base dw 1724
sky_uniform_base dw 34
sky_page_prefix dw 0,0
sky_page_uniform dw 0,0
sky_page_offset dw 7fffh,7fffh
sky_row_tokens dw 336 dup(0ffffh)
sky_dirty_y dw 0,0
sky_dirty_spans dw 336 dup(00a0h)
sky_restore_left dw 0
sky_restore_count dw 160
sky_raw_row dw 0
frame_old_dirty_y dw 0
frame_sky_mask_base dw 0
begin_source_sky_frame:
 mov bx,draw_page
 sub bx,4
 mov ax,sky_dirty_y[bx]
 mov frame_old_dirty_y,ax
 mov sky_dirty_y[bx],200
 mov dx,source_previous_active
 mov cx,sky_page_prefix[bx]
 mov sky_page_prefix[bx],dx
 cmp cx,dx
 jbe source_sky_prefix_bounded
 mov cx,dx
source_sky_prefix_bounded:
 ; Rows0..49 are the exact same solid sky pattern. They stay invariant
 ; across a camera-offset change until either page's first different pattern.
 mov ax,frame_sky_offset
 add ax,sky_uniform_base
 jge source_uniform_nonnegative
 xor ax,ax
source_uniform_nonnegative:
 shl ax,1
 cmp ax,source_previous_active
 jbe source_uniform_bounded
 mov ax,source_previous_active
source_uniform_bounded:
 mov dx,sky_page_uniform[bx]
 mov sky_page_uniform[bx],ax
 cmp dx,ax
 jbe source_uniform_common
 mov dx,ax
source_uniform_common:
 mov ax,frame_sky_offset
 cmp ax,sky_page_offset[bx]
 je source_sky_prefix_same
 mov cx,dx
source_sky_prefix_same:
 mov ax,frame_old_dirty_y
 sub ax,16
 jge source_sky_damage_nonnegative
 xor ax,ax
source_sky_damage_nonnegative:
 shl ax,1
 cmp cx,ax
 jbe source_sky_prefix_ready
 mov cx,ax
source_sky_prefix_ready:
 mov ax,frame_sky_offset
 mov sky_page_offset[bx],ax
 mov frame_skip_sky_rows,cx
 mov ax,168
 mul bx
 mov frame_sky_mask_base,ax
 mov ax,segments[32]
 mov word ptr camera_sky_call+2,ax
 ret
source_sky_may_skip:
 push ax
 push bx
 push dx
 push ds
 mov ax,bp
 shr ax,1
 add ax,16
 sub ax,cs:frame_sky_offset
 cmp ax,0
 jge source_sky_token_nonnegative
 xor ax,ax
source_sky_token_nonnegative:
 cmp ax,183
 jle source_sky_token_valid
 mov ax,183
source_sky_token_valid:
 mov bx,ax
 shl bx,1
 mov ds,cs:segments[32]
 mov ax,[bx]
 add bx,cs:sky_ptr_base
 mov dx,[bx]
 mov cs:sky_raw_row,dx
 mov word ptr cs:camera_sky_call,ax
 mov bx,bp
 add bx,cs:frame_sky_mask_base
 mov dx,cs:sky_row_tokens[bx]
 mov cs:sky_row_tokens[bx],ax
 cmp ax,dx
 jne source_sky_whole_row
 mov ax,cs:sky_dirty_spans[bx]
 mov word ptr cs:sky_dirty_spans[bx],00a0h
 cmp al,ah
 jae source_sky_clean_row
 xor dx,dx
 mov dl,al
 mov cs:sky_restore_left,dx
 sub ah,al
 mov dl,ah
 mov cs:sky_restore_count,dx
 jmp source_sky_need_restore
source_sky_whole_row:
 mov word ptr cs:sky_dirty_spans[bx],00a0h
 mov cs:sky_restore_left,0
 mov cs:sky_restore_count,160
 jmp source_sky_need_restore
source_sky_clean_row:
 pop ds
 pop dx
 pop bx
 pop ax
 stc
 ret
source_sky_need_restore:
 pop ds
 pop dx
 pop bx
 pop ax
 clc
 ret
source_mark_road_row:
 push bx
 mov bx,bp
 add bx,cs:frame_sky_mask_base
 mov word ptr cs:sky_row_tokens[bx],0ffffh
 mov word ptr cs:sky_dirty_spans[bx],00a0h
 pop bx
 ret
mark_source_sprite_dirty:
 push ax
 push bx
 push cx
 push dx
 mov dx,cs:sprite_x
 cmp dx,160
 jge source_dirty_done
 mov cx,dx
 add cx,cs:sprite_width
 cmp cx,0
 jle source_dirty_done
 cmp dx,0
 jge source_dirty_left
 xor dx,dx
source_dirty_left:
 cmp cx,160
 jle source_dirty_right
 mov cx,160
source_dirty_right:
 mov dh,cl
 mov ax,cs:sprite_y
 cmp ax,16
 jl source_dirty_done
 cmp ax,183
 jle source_dirty_bottom
 mov ax,183
source_dirty_bottom:
 sub ax,16
 shl ax,1
 mov cx,ax
 add cx,cs:frame_sky_mask_base
 mov ax,cs:sprite_y
 sub ax,cs:sprite_height
 inc ax
 cmp ax,184
 jge source_dirty_done
 cmp ax,16
 jge source_dirty_top
 mov ax,16
source_dirty_top:
 sub ax,16
 shl ax,1
 mov bx,ax
 ; All later road depths are >= this depth; halfwidth is nonzero for Z>=3.
 cmp word ptr cs:depth_rows[bx],3
 jae source_dirty_done
 push bx
 mov ax,bx
 shr ax,1
 add ax,16
 mov bx,cs:draw_page
 sub bx,4
 cmp ax,cs:sky_dirty_y[bx]
 jge source_dirty_min_y_ready
 mov cs:sky_dirty_y[bx],ax
source_dirty_min_y_ready:
 pop bx
 add bx,cs:frame_sky_mask_base
source_dirty_span:
 cmp dl,byte ptr cs:sky_dirty_spans[bx]
 jae source_dirty_min_valid
 mov byte ptr cs:sky_dirty_spans[bx],dl
source_dirty_min_valid:
 cmp dh,byte ptr cs:sky_dirty_spans[bx+1]
 jbe source_dirty_max_valid
 mov byte ptr cs:sky_dirty_spans[bx+1],dh
source_dirty_max_valid:
 add bx,2
 cmp bx,cx
 jbe source_dirty_span
source_dirty_done:
 pop dx
 pop cx
 pop bx
 pop ax
 ret
