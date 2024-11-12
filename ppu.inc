;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Load Palette Data into PPU
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc LoadPalette
    PPU_SETADDR $3F00
    ldy #0
:
    PPU_SETDATA_OFFSET PaletteData,y   ; PPU_ADDR automatically increments as data stored in PPU_DATA
    iny
    cpy #32                            ; Loop through all 32 Palette Colors
    bne :-

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Load Background Data into PPU Nametable
;;      - Loop through and draw 32 columns of tiles
;;      - Parameters: Source Address Lo-byte, Source Address Hi-byte
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc LoadBackground
    ldx LevelSect
    lda Parameters,x
    sta SourceAddr
    
    bit PPU_STATUS
    lda CurNam
    PPU_TILE_ADDRESS_HI_BYTE
    sta NewColAddr+1                ; hi-byte of destination address ($20XX or $24XX)
    lda #$00
    sta NewColAddr                  ; lo-byte of PPU Address to $00

LoopBackgroundColumn:
    ;lda LevelSequenceHigh,x
    lda Parameters+1,x
    sta SourceAddr+1

    jsr DrawNewColumn                   ; draw a new column
    inc NewColAddr                      ; update PPU Address lo-byte to next column
    lda Column
    cmp #0                              ; if current column is 0 (wrapped around from 32)
    bne LoopBackgroundColumn                ; Else: continue looping

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Load Attribute Data into PPU Nametable
;;      - Loop through and draw 8 columns of attributes
;;      - Parameters: Source Address Lo-byte, Source Address Hi-byte
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc LoadAttributes
    ldx LevelSect
    lda Parameters,x
    sta AttrSrc
    lda Parameters+1,x
    sta AttrSrc+1

    lda CurNam
    PPU_ATTRIBUTE_ADDRESS_HI_BYTE
    sta NewAttAddr+1                ; hi-byte of destination address ($23XX or $27XX)

    ldx #$00
    stx AttrColumn
LoopAttributeColumns:
    lda #$C0
    sta NewAttAddr                  ; reset lo-byte of destination address

    lda AttrColumn
    clc
    adc NewAttAddr
    sta NewAttAddr                  ; add attribute column offset to destination address

    jsr DrawNewAttributes

    inc AttrColumn                  ; increment attribute column to be drawn
    lda AttrColumn
    cmp #8                          ; have 8 columns been completed
    bne LoopAttributeColumns            ; Else: continue looping columns

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to write Sprite Data to OAM
;;      - Write Sprite Data into OAM-RAM for this Frame
;;      - Sprite Data starts at RAM position $0200
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc WriteSpriteData
    lda SpritePtr+1
    sta PPU_OAM_DMA         ; PPU copies all data from $0200 -> $02FF automatically into OAM
    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Routine to check if a new column of tiles needs to be drawn
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc CheckColumnAddresses
NewColumnCheck:
    lda XScroll+1
    and #%00000111              ; Check if the scroll a multiple of 8
    bne NoNewColumn                 ; Else: we still don't need to draw a new column
        lda XScroll
        cmp #0                      ; Check if scroll fraction is 0
        bne NoNewColumn                 ; Else: we still don't need to draw a new column
            jsr SetColumnAddresses
            jsr CheckNewAttributes
            jmp FinishColumn
NoNewColumn:
    lda #0
    sta NewColumn
    sta NewAttrs
FinishColumn:
    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Routine to set a new column of tiles
;;      - Calculate lo-byte & hi-byte of PPU destination address
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc SetColumnAddresses
    lda #1
    sta NewColumn

    lda XScroll+1
    lsr
    lsr
    lsr                             ; divide by 8 to get column (pixel position 8 -> column 1)
    sta NewColAddr                  ; lo-byte of destination address

    lda CurNam
    eor #1                          ; invert current nametable to 0 or 1, into accumulator
    PPU_TILE_ADDRESS_HI_BYTE
    sta NewColAddr+1                ; hi-byte of destination address ($20XX or $24XX)

    ldx LevelSect
    lda LevelSequenceLow,x
    sta SourceAddr
    lda LevelSequenceHigh,x
    sta SourceAddr+1

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Routine to increment the current column
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc IncrementColumn
    lda Column
    clc
    adc #1                          ; increment current column
    and #%00011111                  ; drop left-most bit to clamp value to 32, wrap around to 0
    sta Column
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Routine to draw a new column of tiles off-screen as we scroll horizontally
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc DrawNewColumn
    PPU_SETADDR_VALUE NewColAddr

    lda #$00
    sta Row                         ; reset current row to 0

    ldy Column
ColumnLoop:
    PPU_SETDATA_OFFSET (SourceAddr),y

    inc Row                         ; increment current row
    lda #30
    cmp Row                         ; is current row last row
    beq FinishColumn                    ; Then: finish loop

    tya
    clc
    adc #32                         ; add 32 to y-offset, to next row
    tay
    bcc ColumnLoop                  ; if y not exceed 256 and set Carry, continue loop
NextColumnBatch:
    inc SourceAddr+1                ; increment hi-byte of source, to next batch
    ldy Column                      ; reset y offset
    jmp ColumnLoop                  ; restart draw loop

FinishColumn:
    lda Column
    clc
    adc #1                          ; increment current column
    and #%00011111                  ; drop left-most bit to clamp value to 32, wrap around to 0
    sta Column

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Routine to check if a new column of attributes needs to be drawn
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc CheckNewAttributes
    lda XScroll+1
    and #%00011111                  ; if XScroll+1 is multiple of 32
    bne :+                              ; Then: set up Addresses for next attributes
        jsr SetAttributeAddresses
    :
    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Routine to set a new column of attributes
;;      - Calculate lo-byte & hi-byte of PPU destination address
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc SetAttributeAddresses
    lda #1
    sta NewAttrs

    lda Column
    lsr
    lsr                             ; divide column by 4 to get attribute column offset
    sta AttrColumn                  ; store column offset as attribute column

    clc
    adc #$C0                        ; add baseline PPU_ADDRESS to column offset
    sta NewAttAddr                  ; set PPU_ADDRESS lo-byte to base + offset

    lda CurNam
    eor #1                          ; invert current nametable to 0 or 1, into accumulator
    PPU_ATTRIBUTE_ADDRESS_HI_BYTE
    sta NewAttAddr+1                ; hi-byte of destination address ($23XX or $27XX)

    ldx LevelSect
    lda AttributeSequenceLow,x
    sta AttrSrc
    lda AttributeSequenceHigh,x
    sta AttrSrc+1

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Draw New Attributes
;;      - Draw new attributes every 32 pixels / 4 tiles (each attribute covers 4 tiles)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc DrawNewAttributes
    lda #%00010000                  ; disable NMI, Set Background to second Pattern Table, normal increment mode
    sta PPU_CTRL

    lda #$08
    sta Row
    
    ldy AttrColumn
AttributeLoop:
    PPU_SETADDR_VALUE NewAttAddr
    PPU_SETDATA_OFFSET (AttrSrc),y

    dec Row                         ; increment current row
    beq FinishAttributes            ; is current row last row (8th row)

    tya
    clc
    adc #8                          ; add 8 to y-offset, to next row
    tay

    lda NewAttAddr
    clc
    adc #8
    sta NewAttAddr                  ; add 8 to PPU_ADDRESS lo-byte, to go to next row

    jmp AttributeLoop               ; loop back to attribute loop

FinishAttributes:
    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to set screen render properties
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc RefreshRendering
    bit PPU_STATUS
    lda #%10010100          ; enable NMI, sprites from Pattern Table 0, Background from Pattern Table 1, 32-bit increment mode
    ora CurNam              ; logical OR against current nametable will set bit-0 to current nametable bit (0 or 1)
    sta PPU_CTRL
    lda PPURendering        ; enable Sprites, enable Background, left side no clipping
    sta PPU_MASK
    rts
.endproc
