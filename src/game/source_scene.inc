; Source scenery bank indirection and original handler anchors.
scene_bank_map db 256 dup(0)
scene_anchor_map db 256 dup(8)
scene_anchor dw 8
scene_secondary_name db 'SCB00.BIN',0
cache_source_scene:
 push ds
 push es
 push cs
 pop es
 mov ds,segments[SCENE_SEGMENT]
 mov si,1040
 mov di,offset scene_bank_map
 mov cx,512
 rep movsb
 pop es
 pop ds
 ret
clear_source_scene_map:
 push es
 push cs
 pop es
 mov di,offset scene_bank_map
 xor ax,ax
 mov cx,128
 pushf
 cli
 rep stosw
 popf
 mov al,8
 mov di,offset scene_anchor_map
 mov cx,256
 pushf
 cli
 rep stosb
 popf
 pop es
 ret
load_source_scene_secondary:
 mov ax,course_index
 xor dx,dx
 mov bx,10
 div bx
 add al,'0'
 mov scene_secondary_name[3],al
 add dl,'0'
 mov scene_secondary_name[4],dl
 mov dx,offset scene_secondary_name
 mov ax,3d00h
 int 21h
 jc source_scene_failed
 mov load_handle,ax
 push ds
 mov ds,segments[34]
 mov bx,cs:load_handle
 xor dx,dx
 mov cx,60000
 mov ah,3fh
 int 21h
 pop ds
 jc source_scene_read_failed
 cmp ax,60000
 jne source_scene_read_failed
 inc asset_reads
 mov bx,load_handle
 mov ah,3eh
 int 21h
 call cache_source_scene
 ret
source_scene_read_failed:
 mov bx,load_handle
 mov ah,3eh
 int 21h
source_scene_failed:
 mov runtime_load_error,0fffdh
 mov exit_flag,1
 mov finish_reason,3
 ret
course_sky_name db 'CSK00.BIN',0
load_course_sky:
 mov ax,course_index
 xor dx,dx
 mov bx,10
 div bx
 add al,'0'
 mov course_sky_name[3],al
 add dl,'0'
 mov course_sky_name[4],dl
 mov dx,offset course_sky_name
 mov ax,3d00h
 int 21h
 jc source_scene_failed
 mov load_handle,ax
 push ds
 mov ds,segments[32]
 mov bx,cs:load_handle
 xor dx,dx
 mov cx,14252
 mov ah,3fh
 int 21h
 pop ds
 jc source_scene_read_failed
 or ax,ax
 jz source_scene_read_failed
 inc asset_reads
 mov bx,load_handle
 mov ah,3eh
 int 21h
 mov bx,course_index
 shl bx,1
 mov ax,course_sky_ptr[bx]
 mov sky_ptr_base,ax
 mov ax,course_sky_uniform[bx]
 mov sky_uniform_base,ax
 ret
course_sky_ptr dw 1724,662,752,1292,1256,722,758,884,878,542,980,1100,836,884,1028
course_sky_uniform dw 34,89,38,61,61,79,90,83,49,97,45,49,64,83,75
