;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to set screen scroll register
;;      - Create a static status bar at top of screen by stopping scroll until Sprite Zero
;;      - Then resume scrolling for rest of the screen
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc SetScroll

WaitForNoSpriteZero:
    lda PPU_STATUS
    and #%01000000
    bne WaitForNoSpriteZero ; wait for sprite zero not present

WaitForSpriteZero:
    lda PPU_STATUS
    and #%01000000
    beq WaitForSpriteZero   ; wait for sprite zero before allowing scroll again

    UPDATE_SCROLL_POS

    lda PPURendering
    sta PPU_MASK

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to update screen scroll position
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc UpdateScrollPosition

    lda #SCROLLSPEED
    clc
    adc XScroll             ; add scroll speed to horizontal position fraction
    sta XScroll

    lda #0                  ; add to horizontal position integer, using carry from fraction
    adc XScroll+1           ; if Carry set from adding fraction, fraction added up to new integer
    sta XScroll+1

    lda XScroll+1
    cmp #0
    bne FinishScrollUpdate      ; if screen x-position 0, wraparound from 255 has occured and screen edge reached
        lda XScroll
        cmp #0
        bne FinishScrollUpdate
            lda CurNam
            eor #1          ; XOR with %00000001 will flip last bit, toggling nametables
            sta CurNam
FinishScrollUpdate:

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Scroll the Title
;;      - Increment the Scroll Position, assign to PPU_SCROLL register
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc ScrollTitle
    inc XScroll+1

    bne :+                  ;is scroll wrapped around to 0
        inc CurNam              ; increment Nametable
        lda TitleState
        asl
        sta TitleState          ; update Title State
    :

    UPDATE_SCROLL_POS

    lda PPURendering
    sta PPU_MASK
.endproc