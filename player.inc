;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Update Player Position
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc UpdatePlayerPosition
    lda PlayerXVel          ; calculate X Position
    bpl :+
        dec PlayerXPos+1
    :
    clc
    adc PlayerXPos          ; add velocity to x-position fraction
    sta PlayerXPos

    lda #0                  ; add to x-position integer, using carry from fraction
    adc PlayerXPos+1        ; if Carry set from adding fraction, fraction added up to new integer

    cmp #11
    bne :+
        lda #12
    :
    cmp #225
    bne :+
        lda #224
    :
    sta PlayerXPos+1

    lda PlayerYVel          ; calculate Y Position
    bpl :+
        dec PlayerYPos+1
    :
    clc
    adc PlayerYPos          ; add velocity to y-position fraction
    sta PlayerYPos

    lda #0                  ; add to y-position integer, using carry from fraction
    adc PlayerYPos+1        ; if Carry set from adding fraction, fraction added up to new integer

    cmp #63
    bne :+
        lda #64
    :
    cmp #209
    bne :+
        lda #208
    :
    sta PlayerYPos+1

    rts
.endproc