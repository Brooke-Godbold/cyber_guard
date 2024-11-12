;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to draw a 4x4 Actor Metasprite
;;      - Pointer: Set to address of Metasprite's Data in ROM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc DrawMetasprite
    lda ActorsArray+Actor::YPos,x
    sta Buffer
    lda ActorsArray+Actor::XPos,x
    sta Buffer+1                    ; store values that won't change into parameters -> these need the Actor offset
    lda ActorsArray+Actor::Frame,x
    sta Buffer+2

    ldx #0
:
    lda Buffer
    sta Parameters
    lda Buffer+1
    sta Parameters+3                ; reset X and Y Position parameters

    tya
    pha                             ; buffer current Sprite Index into Stack

    txa
    asl
    asl                             ; multiply Metasprite Index by 4 to get correct Sprite Data Index
    tay

    lda (Pointer),y                 ; load Attributes at Pointer Address indexed to Metasprite Index (0, 1, 2, 3)
    sta Parameters+2

    iny                             ; increment Y to first Tile in data
    tya
    clc
    adc Buffer+2                    ; add current Frame to Sprite Data offset
    tay
    lda (Pointer),y                 ; load Tile for current Frame
    sta Parameters+1

    pla
    tay                             ; restore current Sprite Index from Stack

    stx Parameters+4                ; store current Metasprite
    jsr DrawSprite                  ; DrawSprite will increment Y (Sprite Index)
    ldx Parameters+4                ; restore current Sprite Index from Parameters

    inx
    cpx #4                          ; have 4 Sprites been drawn
    bne :-                              ; Else: continue looping

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Render a single Sprite
;;      - Parameters order expected (Y position, Tile Number, Attributes, X position, Metasprite Index)
;;      - Handles Metasprite offsets by calculating in 4x4 grid, top-left being the origin sprite
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc DrawSprite

    lda Parameters+4
    lsr                     ; if Parameters+4 (Metasprite Index) == 1 || 3
    bcc :+                      ; Then: add 8 to Parameters+3 (X Position)
        lda Parameters+3
        clc
        adc #8
        sta Parameters+3
    :

    lda Parameters+4
    lsr
    lsr                     ; if Parameters+4 (Metasprite Index) == 2 || 3
    bcc :+                      ; add 8 to Paramaters (Y Position)
        lda Parameters
        clc
        adc #8
        sta Parameters
    :

    ldx #0
SpriteLoop:
    sty Buffer+3            ; buffer current Sprite Index
    txa
    tay
    lda Parameters,y        ; pull parameter at X offset
    ldy Buffer+3
    sta (SpritePtr),y       ; store into RAM at Sprite Buffer plus current Sprite Offset
    iny                     ; increment current Sprite Index
    inx
    cpx #4                  ; have all four parameters been pushed to RAM
    bne SpriteLoop              ; Else: continue looping
FinishSprite:
    rts
.endproc