;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Increment the Score
;;      - Parameters: Score to increase by
;;      - Increment by 1 digit, bumping up by decimals
;;      - Perform in a loop for the score added
;;      - Store into Score
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc IncrementScore
    ldy Parameters

Increment1sDigit:
    lda Score+2
    clc
    adc #1
    sta Score+2                 ; add 1 to Digits
    cmp #$A                     ; is Digits equal to 10
    bne FinishIncrement             ; Else; finish incrementing
Increment10sDigit:
    lda #0
    sta Score+2                 ; reset digits to 0
    lda Score+1
    clc
    adc #1
    sta Score+1                 ; add 1 to Tens
    cmp #$A                     ; is Tens equal to 10
    bne FinishIncrement             ; Else: finish incrementing
Increment100sDigit:
    lda #0
    sta Score+1                 ; reset Tens to 0
    lda Score
    clc
    adc #1
    sta Score                   ; add 1 to Hundreds
    cmp #$A                     ; is Hundreds equal to 10
    bne FinishIncrement             ; Else: finish incrementing
FinishIncrement:
    dey
    cpy #0                      ; is there more to add to score
    bne Increment1sDigit            ; Then: continue looping increment
    
    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Draw the Score to the Nametable
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc DrawScore
    lda CurNam
    PPU_TILE_ADDRESS_HI_BYTE
    sta BgTilePtr+1                ; hi-byte of destination address ($20XX or $24XX)
    lda #$B9
    sta BgTilePtr                   ; lo-byte of destination address (Hundreds Score Tile in Nametable)

    PPU_SETADDR_VALUE BgTilePtr
    lda #%00010000                  ; disable NMI, Set Background to second Pattern Table, normal increment mode
    sta PPU_CTRL

    ldy #0
:
    lda Score,y
    sta PPU_DATA
    iny
    cpy #3
    bne :-

    lda #%00010100                  ; disable NMI, Set Background to second Pattern Table, normal increment mode
    sta PPU_CTRL

    rts
.endproc