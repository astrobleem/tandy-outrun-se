; Bounded 8086 nearest-neighbour scaler for canonical crash RLE frames.
;
; ABI (caller initializes CS variables below):
;   DS:SI              frame header (u16 width|4000h, u16 source height)
;   crash_frame_bytes  byte extent from DS:SI, including header
;   ES                 active video page; CS:screen_rows[y*2] is active row offset
;   crash_xleft_bytes  signed left edge in packed video bytes
;   crash_ybottom      signed inclusive bottom scanline
;   crash_width_bytes  target packed video byte width (1..256)
;   crash_height_rows  target scanline height (1..255)
;   crash_hflip        0/1; reverses byte-pair order and swaps packed nibbles
;   crash_viewport_bytes 80 (160 mode) or 160 (320 mode)
;   crash_use_source_occlusion 0 disables hook; nonzero calls source_row_occluded
;   source_row_occluded input BX=screen_y*2, reads caller's sprite_depth, CF=hidden
;
; Preserves DS, ES, and SI. Clobbers AX,BX,CX,DX,DI,BP. Returns CF=1 for an
; invalid/truncated frame or unsupported dimensions; CF=0 when drawn/clipped.
; Call mark_source_sprite_dirty in the actor adapter before this routine.
; Fast-path unscaled non-HFLIP frames through draw_rle_sprite in the adapter.
;
; RLE row bytes: segment count; each segment [byte-start, length|masked];
; opaque payload has length value bytes; masked payload has length (mask,value)
; byte pairs. The row buffer stores mask then value for every source byte.

draw_crash_rle_scaled:
 push si
 mov cs:crash_frame_base,si
 mov ax,cs:crash_frame_bytes
 cmp ax,4
 jb crash_scale_bad
 mov bx,ax
 dec bx
 mov ax,si
 add ax,bx
 jc crash_scale_bad
 mov ax,[si]
 test ax,8000h
 jnz crash_scale_bad
 test ax,4000h
 jz crash_scale_bad
 and ax,3fffh
 or ax,ax
 jz crash_scale_bad
 cmp ax,128
 ja crash_scale_bad
 mov cs:crash_src_width,ax
 mov ax,[si+2]
 or ax,ax
 jz crash_scale_bad
 cmp ax,256
 jae crash_scale_bad
 mov cs:crash_src_height,ax
 mov ax,cs:crash_width_bytes
 or ax,ax
 jz crash_scale_bad
 cmp ax,256
 ja crash_scale_bad
 mov ax,cs:crash_height_rows
 or ax,ax
 jz crash_scale_bad
 cmp ax,256
 jae crash_scale_bad
 cmp cs:crash_viewport_bytes,80
 je crash_scale_viewport_valid
 cmp cs:crash_viewport_bytes,160
 jne crash_scale_bad
crash_scale_viewport_valid:
 cmp cs:crash_hflip,1
 ja crash_scale_bad

 ; Index every compressed source row and validate all run extents first.
 mov si,cs:crash_frame_base
 add si,4
 xor cx,cx
crash_scale_scan_row:
 cmp cx,cs:crash_src_height
 jae crash_scale_scan_done
 mov ax,si
 sub ax,cs:crash_frame_base
 cmp ax,cs:crash_frame_bytes
 jae crash_scale_bad
 mov bx,cx
 shl bx,1
 mov cs:crash_row_offsets[bx],ax
 xor ax,ax
 mov al,[si]
 inc si
 mov cs:crash_scan_runs,ax
crash_scale_scan_run:
 cmp cs:crash_scan_runs,0
 je crash_scale_scan_row_done
 mov ax,si
 sub ax,cs:crash_frame_base
 add ax,2
 jc crash_scale_bad
 cmp ax,cs:crash_frame_bytes
 ja crash_scale_bad
 mov al,[si]
 mov ah,[si+1]
 xor dx,dx
 mov dl,ah
 and dx,7fh
 or dx,dx
 jz crash_scale_bad
 xor bx,bx
 mov bl,al
 add bx,dx
 cmp bx,cs:crash_src_width
 ja crash_scale_bad
 test ah,80h
 jz crash_scale_scan_opaque
 shl dx,1
crash_scale_scan_opaque:
 add dx,2
 mov ax,si
 sub ax,cs:crash_frame_base
 add ax,dx
 jc crash_scale_bad
 cmp ax,cs:crash_frame_bytes
 ja crash_scale_bad
 add si,dx
 dec cs:crash_scan_runs
 jmp crash_scale_scan_run
crash_scale_scan_row_done:
 inc cx
 jmp crash_scale_scan_row
crash_scale_scan_done:

 ; Build destination-column -> source-byte-offset map with a rational DDA.
 ; Center-sampled nearest is floor((2*dst_column+1)*src/(2*dst)); no DIV.
 mov cs:crash_map_src_index,0
 mov ax,cs:crash_width_bytes
 shl ax,1
 mov cs:crash_x_phase_threshold,ax
 mov ax,cs:crash_src_width
 mov cs:crash_map_phase,ax
 mov cs:crash_map_dest_index,0
crash_scale_map_x_initial:
 mov ax,cs:crash_map_phase
 cmp ax,cs:crash_x_phase_threshold
 jb crash_scale_map_x
 sub ax,cs:crash_x_phase_threshold
 mov cs:crash_map_phase,ax
 inc cs:crash_map_src_index
 jmp crash_scale_map_x_initial
crash_scale_map_x:
 mov ax,cs:crash_map_dest_index
 cmp ax,cs:crash_width_bytes
 jae crash_scale_map_x_done
 mov ax,cs:crash_map_src_index
 cmp cs:crash_hflip,0
 je crash_scale_map_x_forward
 mov ax,cs:crash_src_width
 dec ax
 sub ax,cs:crash_map_src_index
crash_scale_map_x_forward:
 shl ax,1
 mov bx,cs:crash_map_dest_index
 shl bx,1
 mov cs:crash_xmap[bx],ax
 mov ax,cs:crash_map_phase
 add ax,cs:crash_src_width
 add ax,cs:crash_src_width
 mov cs:crash_map_phase,ax
crash_scale_map_advance:
 mov ax,cs:crash_map_phase
 cmp ax,cs:crash_x_phase_threshold
 jb crash_scale_map_x_advance_done
 sub ax,cs:crash_x_phase_threshold
 mov cs:crash_map_phase,ax
 inc cs:crash_map_src_index
 jmp crash_scale_map_advance
crash_scale_map_x_advance_done:
 inc cs:crash_map_dest_index
 jmp crash_scale_map_x
crash_scale_map_x_done:

 ; Source and target y are both top-to-bottom. Repeated source rows are decoded
 ; only once, even when the destination is larger than the canonical frame.
 mov ax,cs:crash_ybottom
 sub ax,cs:crash_height_rows
 inc ax
 mov cs:crash_target_top,ax
 mov cs:crash_dest_y_index,0
 mov cs:crash_y_src_index,0
 mov ax,cs:crash_height_rows
 shl ax,1
 mov cs:crash_y_phase_threshold,ax
 mov ax,cs:crash_src_height
 mov cs:crash_y_phase,ax
 mov cs:crash_last_decoded_y,0ffffh
crash_scale_y_initial:
 mov ax,cs:crash_y_phase
 cmp ax,cs:crash_y_phase_threshold
 jb crash_scale_draw_row
 sub ax,cs:crash_y_phase_threshold
 mov cs:crash_y_phase,ax
 inc cs:crash_y_src_index
 jmp crash_scale_y_initial
crash_scale_draw_row:
 mov ax,cs:crash_dest_y_index
 cmp ax,cs:crash_height_rows
 jae crash_scale_ok
 mov ax,cs:crash_target_top
 add ax,cs:crash_dest_y_index
 mov cs:crash_screen_y,ax
 cmp ax,16
 jl crash_scale_next_row
 cmp ax,184
 jge crash_scale_next_row
 cmp cs:crash_use_source_occlusion,0
 je crash_scale_row_visible
 mov bx,cs:crash_screen_y
 shl bx,1
 call source_row_occluded
 jc crash_scale_next_row
crash_scale_row_visible:
 mov ax,cs:crash_y_src_index
 cmp ax,cs:crash_last_decoded_y
 je crash_scale_row_ready
 mov cs:crash_last_decoded_y,ax
 call crash_scale_decode_row
 jc crash_scale_bad
crash_scale_row_ready:
 mov bx,cs:crash_screen_y
 shl bx,1
 mov di,cs:screen_rows[bx]
 mov cs:crash_screen_row_base,di
 mov cs:crash_draw_x_index,0
crash_scale_draw_x:
 mov ax,cs:crash_draw_x_index
 cmp ax,cs:crash_width_bytes
 jae crash_scale_next_row
 add ax,cs:crash_xleft_bytes
 cmp ax,0
 jl crash_scale_draw_x_skip
 cmp ax,cs:crash_viewport_bytes
 jae crash_scale_draw_x_skip
 mov cs:crash_screen_x,ax
 mov bx,cs:crash_draw_x_index
 shl bx,1
 mov bx,cs:crash_xmap[bx]
 mov al,cs:crash_row_buffer[bx]
 mov ah,cs:crash_row_buffer[bx+1]
 cmp cs:crash_hflip,0
 je crash_scale_pixel_ready
 ; Reverse the two 4-bit pixels in both mask and value bytes.
 mov dl,al
 and al,0f0h
 mov cl,4
 shr al,cl
 and dl,0fh
 shl dl,cl
 or al,dl
 mov dl,ah
 and ah,0f0h
 mov cl,4
 shr ah,cl
 and dl,0fh
 shl dl,cl
 or ah,dl
crash_scale_pixel_ready:
 mov bx,cs:crash_screen_x
 add di,bx
 mov dl,es:[di]
 and dl,al
 or dl,ah
 mov es:[di],dl
 sub di,bx
crash_scale_draw_x_skip:
 inc cs:crash_draw_x_index
 jmp crash_scale_draw_x

crash_scale_next_row:
 mov ax,cs:crash_y_phase
 add ax,cs:crash_src_height
 add ax,cs:crash_src_height
 mov cs:crash_y_phase,ax
crash_scale_advance_y:
 mov ax,cs:crash_y_phase
 cmp ax,cs:crash_y_phase_threshold
 jb crash_scale_y_advanced
 sub ax,cs:crash_y_phase_threshold
 mov cs:crash_y_phase,ax
 inc cs:crash_y_src_index
 jmp crash_scale_advance_y
crash_scale_y_advanced:
 inc cs:crash_dest_y_index
 jmp crash_scale_draw_row

crash_scale_decode_row:
 ; Initialize active source bytes to fully transparent (mask FF, value 00).
 xor di,di
 mov cx,cs:crash_src_width
crash_scale_clear_row:
 mov byte ptr cs:crash_row_buffer[di],0ffh
 inc di
 mov byte ptr cs:crash_row_buffer[di],0
 inc di
 loop crash_scale_clear_row
 mov bx,cs:crash_y_src_index
 shl bx,1
 mov si,cs:crash_frame_base
 add si,cs:crash_row_offsets[bx]
 xor ax,ax
 mov al,[si]
 inc si
 mov cs:crash_decode_runs,ax
crash_scale_decode_run:
 cmp cs:crash_decode_runs,0
 je crash_scale_decode_done
 mov al,[si]
 mov ah,[si+1]
 add si,2
 xor bx,bx
 mov bl,al
 mov cs:crash_run_index,bx
 xor cx,cx
 mov cl,ah
 and cx,7fh
 mov cs:crash_run_length,cx
 test ah,80h
 jnz crash_scale_decode_masked
crash_scale_decode_opaque_byte:
 mov al,[si]
 inc si
 mov bx,cs:crash_run_index
 shl bx,1
 mov byte ptr cs:crash_row_buffer[bx],0
 inc bx
 mov cs:crash_row_buffer[bx],al
 inc cs:crash_run_index
 dec cs:crash_run_length
 jnz crash_scale_decode_opaque_byte
 jmp crash_scale_decode_run_done
crash_scale_decode_masked:
 mov al,[si]
 inc si
 mov bx,cs:crash_run_index
 shl bx,1
 mov cs:crash_row_buffer[bx],al
 inc si
 inc bx
 mov al,[si-1]
 mov cs:crash_row_buffer[bx],al
 inc cs:crash_run_index
 dec cs:crash_run_length
 jnz crash_scale_decode_masked
crash_scale_decode_run_done:
 dec cs:crash_decode_runs
 jmp crash_scale_decode_run
crash_scale_decode_done:
 clc
 ret

crash_scale_bad:
 stc
 pop si
 ret
crash_scale_ok:
 clc
 pop si
 ret

; Private variables/buffers. The row-offset table and x map reserve 512 B each;
; the maximum canonical width/height currently fits within 128 source units.
crash_frame_base dw 0
crash_frame_bytes dw 0
crash_src_width dw 0
crash_src_height dw 0
crash_xleft_bytes dw 0
crash_ybottom dw 0
crash_width_bytes dw 0
crash_height_rows dw 0
crash_hflip dw 0
crash_viewport_bytes dw 160
crash_use_source_occlusion dw 0
crash_scan_runs dw 0
crash_decode_runs dw 0
crash_row_offsets dw 256 dup(0)
crash_xmap dw 256 dup(0)
crash_row_buffer db 256 dup(0)
crash_map_src_index dw 0
crash_map_phase dw 0
crash_x_phase_threshold dw 0
crash_map_dest_index dw 0
crash_target_top dw 0
crash_dest_y_index dw 0
crash_y_src_index dw 0
crash_y_phase dw 0
crash_y_phase_threshold dw 0
crash_last_decoded_y dw 0
crash_screen_y dw 0
crash_screen_x dw 0
crash_screen_row_base dw 0
crash_draw_x_index dw 0
crash_run_index dw 0
crash_run_length dw 0
