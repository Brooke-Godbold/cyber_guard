.macro PLAY_EXPLOSION_SFX
    txa
    pha
    lda #1
    ldx #FAMISTUDIO_SFX_CH1
    jsr famistudio_sfx_play
    pla
    tax
.endmacro