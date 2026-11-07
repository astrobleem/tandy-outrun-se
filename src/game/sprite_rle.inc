; Lossless source art runs. Opaque spans use MOVS; transparent bytes are skipped.
; DS:SI points after width/height, ES is the hidden native video page.
draw_rle_sprite:
 mov dx,cs:sprite_x
 cmp dx,160
 jge rle_return
 mov cx,dx
 add cx,cs:sprite_width
 cmp cx,0
 jle rle_return
 mov bx,cs:sprite_y
 cmp bx,16
 jl rle_return
 sub bx,cs:sprite_height
 inc bx
 cmp bx,184
 jge rle_return
 mov bp,cs:sprite_height
 shl bx,1
 cmp dx,0
 jl rle_clip_entry
 cmp cx,160
 ja rle_clip_entry
 cmp bx,32
 jl rle_clip_entry
 cmp cs:sprite_y,184
 jae rle_clip_entry
 ; Every GEO row depth is monotone (975 profiles); Q8 interpolation
 ; preserves this. If the bottom row is visible, every earlier row is too.
 ; Retain the clipped path for a hill that hides any bottom row.
 push bx
 mov bx,cs:sprite_y
 shl bx,1
 call source_row_occluded
 pop bx
 jc rle_clip_entry
rle_full_row:
 lodsb
 xor ah,ah
 mov cs:rle_segments,ax
 or ax,ax
 jz rle_full_row_done
rle_full_run:
 lodsw
 xor cx,cx
 mov cl,al
 mov di,cs:screen_rows[bx]
 add di,cs:sprite_x
 add di,cx
 mov cl,ah
 and cx,127
 test ah,80h
 jnz rle_full_masked
 test cx,1
 jz rle_full_even
 movsb
 dec cx
rle_full_even:
 shr cx,1
 rep movsw
 jmp rle_full_run_done
rle_full_masked:
 lodsw
 and al,es:[di]
 or al,ah
 stosb
 loop rle_full_masked
rle_full_run_done:
 dec cs:rle_segments
 jnz rle_full_run
rle_full_row_done:
 add bx,2
 dec bp
 jnz rle_full_row
rle_return:
 ret

rle_chk db 1
rle_clip_entry:
 mov cs:rle_chk,1
 cmp cs:sprite_y,184
 jae rle_clip_rows
 push bx
 mov bx,cs:sprite_y
 shl bx,1
 call source_row_occluded
 pop bx
 jc rle_clip_rows
 mov cs:rle_chk,0
 jmp rle_clip_rows
rle_clip_rows:
 ; Visibility is a row property: do not repeat it for each leaf/run.
 cmp bx,32
 jl rle_hidden_row
 ; All subsequent rows remain below the viewport or behind nearer road.
 cmp bx,368
 jge rle_return
 cmp cs:rle_chk,0
 je rle_clip_unchecked
 call source_row_occluded
 jc rle_return
rle_clip_unchecked:
 mov dx,cs:sprite_x
 cmp dx,0
 jl rle_clip_visible_row
 add dx,cs:sprite_width
 cmp dx,160
 ja rle_clip_visible_row
 jmp rle_partial_full_row
rle_hidden_row:
 lodsb
 xor ah,ah
 mov cs:rle_segments,ax
 or ax,ax
 jz rle_clip_row_done
rle_hidden_run:
 lodsw
 xor cx,cx
 mov cl,ah
 and cx,127
 test ah,80h
 jz rle_hidden_opaque
 shl cx,1
rle_hidden_opaque:
 add si,cx
 dec cs:rle_segments
 jnz rle_hidden_run
 jmp rle_clip_row_done
rle_partial_full_row:
 lodsb
 xor ah,ah
 mov cs:rle_segments,ax
 or ax,ax
 jz rle_partial_full_row_done
rle_partial_full_run:
 lodsw
 xor cx,cx
 mov cl,al
 mov di,cs:screen_rows[bx]
 add di,cs:sprite_x
 add di,cx
 mov cl,ah
 and cx,127
 test ah,80h
 jnz rle_partial_full_masked
 test cx,1
 jz rle_partial_full_even
 movsb
 dec cx
rle_partial_full_even:
 shr cx,1
 rep movsw
 jmp rle_partial_full_run_done
rle_partial_full_masked:
 lodsw
 and al,es:[di]
 or al,ah
 stosb
 loop rle_partial_full_masked
rle_partial_full_run_done:
 dec cs:rle_segments
 jnz rle_partial_full_run
rle_partial_full_row_done:
 jmp rle_clip_row_done
rle_clip_visible_row:
 lodsb
 xor ah,ah
 mov cs:rle_segments,ax
 or ax,ax
 jz rle_clip_row_done
rle_clip_run:
 lodsw
 xor cx,cx
 mov cl,ah
 and cx,127
 mov cs:rle_masked,0
 test ah,80h
 jz rle_clip_kind_ready
 mov cs:rle_masked,1
rle_clip_kind_ready:
 xor ah,ah
 add ax,cs:sprite_x
 cmp ax,160
 jge rle_clip_skip_data
 mov cs:rle_right_skip,0
 cmp ax,0
 jge rle_clip_left_ready
 neg ax
 cmp ax,cx
 jae rle_clip_skip_data
 sub cx,ax
 cmp cs:rle_masked,0
 je rle_clip_left_bytes
 shl ax,1
rle_clip_left_bytes:
 add si,ax
 xor ax,ax
rle_clip_left_ready:
 mov dx,160
 sub dx,ax
 cmp cx,dx
 jbe rle_clip_right_ready
 mov cs:rle_right_skip,cx
 sub cs:rle_right_skip,dx
 mov cx,dx
rle_clip_right_ready:
 mov di,cs:screen_rows[bx]
 add di,ax
 cmp cs:rle_masked,0
 jne rle_clip_visible_masked
 test cx,1
 jz rle_clip_visible_even
 movsb
 dec cx
rle_clip_visible_even:
 shr cx,1
 rep movsw
 jmp rle_clip_skip_right
rle_clip_visible_masked:
 lodsw
 and al,es:[di]
 or al,ah
 stosb
 loop rle_clip_visible_masked
rle_clip_skip_right:
 mov ax,cs:rle_right_skip
 cmp cs:rle_masked,0
 je rle_clip_right_bytes
 shl ax,1
rle_clip_right_bytes:
 add si,ax
 jmp rle_clip_run_done
rle_clip_skip_data:
 cmp cs:rle_masked,0
 je rle_clip_skip_opaque
 shl cx,1
rle_clip_skip_opaque:
 add si,cx
rle_clip_run_done:
 dec cs:rle_segments
 jnz rle_clip_run
rle_clip_row_done:
 add bx,2
 dec bp
 jnz rle_clip_rows
 ret
