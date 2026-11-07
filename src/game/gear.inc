; Deliberate compact two-gear handling model, automatic by default.
gear_auto dw 1
gear_state dw 0
gear_max dw 240
frame_gear_auto dw 1
frame_gear_state dw 0
hud_gear db 'AL',0
update_gear:
 cmp gear_auto,0
 je manual_gear_selected
 mov gear_state,0
 cmp speed,150
 jb manual_gear_selected
 mov gear_state,1
manual_gear_selected:
 mov gear_max,180
 cmp gear_state,0
 je gear_limit_ready
 mov gear_max,240
gear_limit_ready:
 ret
