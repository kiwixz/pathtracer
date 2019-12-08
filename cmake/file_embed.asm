section .rodata

global @prefix@_embed_@name@_begin
global @prefix@_embed_@name@_end
global @prefix@_embed_@name@_size

begin:
    incbin "@file@"
end:
    db 0x00  ; null terminator

@prefix@_embed_@name@_begin: dq begin
@prefix@_embed_@name@_end: dq end
@prefix@_embed_@name@_size: dq end - begin
