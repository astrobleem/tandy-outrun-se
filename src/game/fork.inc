; Two projected three-lane strips with a shared world coordinate system.
; The split/choice/remove sequence follows the arcade model; distances and
; scale are adapted for the compact 8088 course representation.
row_centre_right dw 0
near_source_flags dw 0
update_split_near:
 mov ax,stage_distance
 mov dx,ax
 and dx,255
 mov cx,dx
 mov cl,8
 shr ax,cl
 cmp ax,63
 jbe near_source_index_valid
 mov ax,63
near_source_index_valid:
 mov bx,ax
 xor ax,ax
 mov al,geo_flags[bx]
 mov near_source_flags,ax
 shl bx,1
 shl bx,1
 mov ax,stage_distance
 and ax,255
 mov cx,ax
 mov ax,geo_meta[bx+4]
 sub ax,geo_meta[bx]
 imul cx
 mov al,ah
 mov ah,dl
 add ax,geo_meta[bx]
 mov cx,256
 mul cx
 mov cx,212
 div cx
 ; Source OFerrari already follows road-width changes.
 mov split_near_offset,ax
 test near_source_flags,1
 jz split_near_done
 cmp ax,435
 jbe split_near_done
 cmp branch_choice,0
 jne split_near_done
 mov branch_choice,-1
 cmp player_x,0
 jle split_near_done
 mov branch_choice,1
split_near_done:
 ret

compute_row_split:
 ; Zero source width and matching curves produce one common-centre strip.
 ; Keep every cache/edge/lane policy identical to the general path.
 cmp frame_source_width,0
 jne compute_general_split
 test frame_source_flags,1
 jnz compute_general_split
 mov row_single_visible,0
 mov row_gap_active,0
 mov row_split_pixels,0
 mov ax,row_centre
 mov row_centre_right,ax
 sub ax,row_half
 mov row_left,ax
 mov ax,row_centre
 add ax,row_half
 mov row_right,ax
 ret
compute_general_split:
 mov row_single_visible,0
 mov row_gap_active,0
 mov bx,bp
 mov ax,source_split_pixels[bx]
 mov row_split_pixels,ax
 mov dx,row_centre
 sub dx,ax
 mov row_left,dx
 mov bx,row_half
 sub row_left,bx
 mov dx,row_centre
 add dx,ax
 test frame_source_flags,1
 jz source_right_curve_valid
 mov bx,bp
 mov cx,curve_rows[bx]
 ; curve_rows holds the source common center after camera adjustment here;
 ; subtract only the signed curve contribution cached in the row buffer.
 mov bx,bp
 shl bx,1
 shl bx,1
 mov cx,word ptr road_row_buffer[bx]
 sub cx,80
 shl cx,1
 sub dx,cx
source_right_curve_valid:
 mov row_centre_right,dx
 ; Outer union remains ordered even where opposite curves cross at a fork.
 mov bx,row_half
 mov cx,row_centre
 sub cx,row_split_pixels
 cmp cx,dx
 jle source_outer_ordered
 mov row_left,dx
 sub row_left,bx
 mov row_right,cx
 add row_right,bx
 jmp split_row_done
source_outer_ordered:
 mov row_right,dx
 mov bx,row_half
 add row_right,bx
 mov dx,row_centre
 sub dx,ax
 add dx,row_half
 mov cx,row_centre_right
 sub cx,row_half
 cmp cx,dx
 jle split_row_done
 mov row_gap_active,1
 cmp dx,0
 jge gap_left_nonnegative
 xor dx,dx
gap_left_nonnegative:
 cmp dx,160
 jle gap_left_valid
 mov dx,160
gap_left_valid:
 cmp cx,0
 jge gap_right_nonnegative
 xor cx,cx
gap_right_nonnegative:
 cmp cx,160
 jle gap_right_valid
 mov cx,160
gap_right_valid:
 mov row_gap_left,dx
 mov row_gap_right,cx
 ; These are exact clipped spans, not a removal of the second road.
 ; A road beyond the viewport contributes no edge or lane pixels.
 cmp cx,160
 jne source_left_gap_test
 ; The original inner edge includes X=0; the outer right edge does not.
 cmp dx,1
 je split_row_done
 mov row_right,dx
 mov ax,row_centre
 sub ax,row_split_pixels
 mov row_local_centre,ax
 jmp source_local_road
source_left_gap_test:
 cmp dx,0
 jne split_row_done
 mov row_left,cx
 mov ax,row_centre_right
 mov row_local_centre,ax
source_local_road:
 mov row_gap_active,0
 mov row_single_visible,1
split_row_done:
 ret

fork_inner_edges:
 mov di,row_gap_left
 cmp di,1
 jl no_gap_left_edge
 cmp di,160
 jge no_gap_left_edge
 add di,dx
 mov es:[di-1],al
no_gap_left_edge:
 mov di,row_gap_right
 cmp di,0
 jle no_gap_right_edge
 cmp di,160
 jge no_gap_right_edge
 add di,dx
 mov es:[di],al
no_gap_right_edge:
 ret

fork_lane_marks:
 mov di,row_centre
 sub di,row_split_pixels
 sub di,ax
 call lane_mark
 mov di,row_centre
 sub di,row_split_pixels
 add di,ax
 call lane_mark
 mov di,row_centre_right
 sub di,ax
 call lane_mark
 mov di,row_centre_right
 add di,ax
 call lane_mark
 ret

project_object_lane:
 cmp frame_objects[bx+4],2
 je source_object_right
 cmp frame_objects[bx+4],256
 jb source_object_left
 cmp ax,0
 jl source_object_left
source_object_right:
 push bx
 push ax
 mov ax,object_y
 cmp ax,183
 jle source_object_cache_y
 mov ax,183
source_object_cache_y:
 sub ax,16
 shl ax,1
 mov bx,ax
 mov di,centres_right[bx]
 pop ax
 pop bx
source_object_left:
 ret
