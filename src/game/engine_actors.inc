; Source-enabled actors, ordered by source priority. CF=1 replaces normal car.
engine_draw_actors:
 cmp frame_crash_ticks,0
 jne engine_actors_active
 clc
 ret
engine_actors_active:
 mov word ptr engine_actor_drawn,0
 mov word ptr engine_actor_drawn+2,0
 mov word ptr engine_actor_drawn+4,0
 mov engine_actor_index,6
engine_actor_select:
 mov engine_actor_best,0ffffh
 mov engine_actor_priority,0ffffh
 xor bx,bx
 mov si,offset engine_snapshot+IPC_ACTORS
engine_actor_scan:
 cmp byte ptr engine_actor_drawn[bx],0
 jne engine_actor_scan_next
 test byte ptr [si+9],80h
 jz engine_actor_scan_next
 mov ax,[si+13]
 cmp ax,engine_actor_priority
 ja engine_actor_scan_next
 ; Later actor wins ties: shadows (odd) precede their corresponding body.
 mov engine_actor_priority,ax
 mov engine_actor_best,bx
 mov engine_actor_offset,si
engine_actor_scan_next:
 inc bx
 add si,IPC_ACTOR_BYTES
 cmp bx,6
 jb engine_actor_scan
 mov bx,engine_actor_best
 cmp bx,0ffffh
 je engine_actors_done
 mov byte ptr engine_actor_drawn[bx],1
 call engine_draw_actor
 dec engine_actor_index
 jnz engine_actor_select
engine_actors_done:
 stc
 ret

engine_draw_actor:
 mov bx,engine_actor_offset
 cmp byte ptr [bx+8],0
 je engine_actor_return
 mov ax,[bx+4]
 mov cx,10
 imul cx
 mov cx,ENGINE_ACTOR_X_DIV
 idiv cx
 add ax,ENGINE_CENTRE_BYTES
 mov sprite_x,ax
 mov ax,[bx+6]
 sub ax,221
 mov cx,4
 imul cx
 mov cx,5
 idiv cx
 add ax,183
 mov sprite_y,ax
 ; Source sort priority and road occlusion priority are distinct, especially
 ; for shadows whose sort priority remains0 while road_priority tracks Z.
 mov ax,[bx+15]
 mov sprite_depth,ax
 mov ax,[bx]
 mov dx,[bx+2]
 cmp dx,1
 jne engine_body_actor
 cmp ax,03b6h
 jne engine_body_actor
 ; Source shadow mask is encoded with mask7/transparentF nibbles, value0.
 xor ax,ax
 mov al,[bx+8]
 cmp ax,191
 ja engine_actor_return
 mov cx,3
 mul cx
 mov si,ax
 push ds
 mov ds,cs:segments[36]
 xor ax,ax
 mov al,[si+ENGINE_SHADOW_ZOOM]
 mov cs:engine_shadow_profile,ax
 mov al,[si+ENGINE_SHADOW_ZOOM+1]
 or ax,ax
 jz engine_actor_pop_return
 mov cx,10
 mul cx
 mov cx,ENGINE_ACTOR_X_DIV
 div cx
 or ax,ax
 jnz engine_shadow_width_ready
 inc ax
engine_shadow_width_ready:
 mov cs:sprite_width,ax
 xor ax,ax
 mov al,[si+ENGINE_SHADOW_ZOOM+2]
 or ax,ax
 jz engine_actor_pop_return
 mov cx,4
 mul cx
 mov cx,5
 div cx
 or ax,ax
 jnz engine_shadow_height_ready
 inc ax
engine_shadow_height_ready:
 mov cs:sprite_height,ax
 mov si,cs:engine_shadow_profile
 shl si,1
 shl si,1
 mov ax,[si+ENGINE_SHADOW_DIRECTORY+2]
 mov cs:crash_frame_bytes,ax
 mov si,[si+ENGINE_SHADOW_DIRECTORY]
 jmp engine_actor_sizes
engine_body_actor:
 mov cx,[bx+10]
 push ds
 mov ds,cs:segments[36]
 mov si,16
 mov di,69
engine_actor_lookup:
 cmp [si],ax
 jne engine_actor_lookup_next
 cmp [si+2],dx
 jne engine_actor_lookup_next
 cmp [si+4],cx
 je engine_actor_found
engine_actor_lookup_next:
 add si,18
 dec di
 jnz engine_actor_lookup
 pop ds
 inc engine_actor_missing
 ret
engine_actor_found:
 mov cs:engine_actor_row,si
 mov bx,cs:engine_actor_offset
 xor dx,dx
 mov dl,byte ptr cs:[bx+8]
 cmp dx,64
 jb engine_actor_pop_return
 sub dx,64
 mov di,[si+12]
 add di,dx
 xor ax,ax
 mov al,[di]
 mov cx,10
 mul cx
 mov cx,ENGINE_BODY_WIDTH_DIV
 div cx
 mov cs:sprite_width,ax
 mov di,[si+14]
 add di,dx
 ; DX was destroyed by division: reload the zoom index.
 mov bx,cs:engine_actor_offset
 xor dx,dx
 mov dl,byte ptr cs:[bx+8]
 sub dx,64
 mov di,[si+14]
 add di,dx
 xor ax,ax
 mov al,[di]
 mov cx,14
 mul cx
 mov cx,13
 div cx
 mov cs:sprite_height,ax
 mov ax,[si+16]
 mov cs:crash_frame_bytes,ax
 mov si,[si+6]
engine_actor_sizes:
 cmp word ptr cs:sprite_width,0
 je engine_actor_pop_return
 cmp word ptr cs:sprite_height,0
 je engine_actor_pop_return
 mov bx,cs:engine_actor_offset
 mov al,byte ptr cs:[bx+9]
 and al,1
 xor ah,ah
 mov cs:crash_hflip,ax
 mov al,byte ptr cs:[bx+12]
 and ax,3
 cmp ax,1
 je engine_actor_x_ready
 mov dx,cs:sprite_width
 cmp ax,2
 je engine_actor_x_sub
 shr dx,1
engine_actor_x_sub:
 sub cs:sprite_x,dx
engine_actor_x_ready:
 mov al,byte ptr cs:[bx+12]
 and ax,12
 cmp ax,8
 je engine_actor_y_ready
 mov dx,cs:sprite_height
 cmp ax,4
 je engine_actor_y_top
 shr dx,1
 jmp engine_actor_y_add
engine_actor_y_top:
 dec dx
engine_actor_y_add:
 add cs:sprite_y,dx
engine_actor_y_ready:
 call mark_source_sprite_dirty
 mov ax,cs:sprite_x
 mov cs:crash_xleft_bytes,ax
 mov ax,cs:sprite_y
 mov cs:crash_ybottom,ax
 mov ax,cs:sprite_width
 mov cs:crash_width_bytes,ax
 mov ax,cs:sprite_height
 mov cs:crash_height_rows,ax
 mov word ptr cs:crash_viewport_bytes,ENGINE_VIEW_BYTES
 mov word ptr cs:crash_use_source_occlusion,1
 call draw_crash_rle_scaled
 jnc engine_actor_pop_return
 inc word ptr cs:engine_actor_errors
engine_actor_pop_return:
 pop ds
engine_actor_return:
 ret
