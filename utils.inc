;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to set PPU Address, with an address pointer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro PPU_SETADDR addr
    bit PPU_STATUS          ; Reset PPU_ADDR latch
    lda #>addr              ; Set hi-byte of PPU_ADDR
    sta PPU_ADDR
    lda #<addr              ; Set lo-byte of PPU_ADDR
    sta PPU_ADDR            ; PPU_ADDR now pointing to $3F00
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to set PPU Address, with a literal value from RAM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro PPU_SETADDR_VALUE val
    bit PPU_STATUS          ; Reset PPU_ADDR latch
    lda val+1               ; Set hi-byte of PPU_ADDR
    sta PPU_ADDR
    lda val                 ; Set lo-byte of PPU_ADDR
    sta PPU_ADDR            ; PPU_ADDR now pointing to $3F00
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to set PPU Data, with an address and an offset
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro PPU_SETDATA_OFFSET addr,offset
    lda addr,offset
    sta PPU_DATA
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to save Register Values and Status Flags, by pushing to the Stack
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro SAVE_REGISTERS_BUFFER
    pha                         ; push A to the Stack
    txa
    pha                         ; push X to the Stack
    tya
    pha                         ; push Y to the Stack
    php                         ; push Status Flags to the Stack
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to restore Register Values and Status Flags, by pulling from the Stack (reverse order to saving)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro RESTORE_REGISTERS_BUFFER
    plp                         ; pull Status Flags from the Stack
    pla
    tay                         ; pull Y from the Stack
    pla
    tax                         ; pull X from the Stack
    pla                         ; pull A from the Stack
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to restore scroll position to current scroll in RAM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro UPDATE_SCROLL_POS
    bit PPU_STATUS
    lda XScroll+1
    sta PPU_SCROLL          ; set horizontal scroll position
    lda #0
    sta PPU_SCROLL          ; reset vertical scroll position
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to get the Hi-byte PPU Address for a Nametable
;;      - A: Nametable (0 or 1)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro PPU_TILE_ADDRESS_HI_BYTE
    asl
    asl                             ; multiply by 4, get either $00 -> $00 or $01 -> $04
    clc
    adc #$20                        ; add to $20 to get either $20 or $24
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to get the Hi-byte PPU Address for a Nametable
;;      - A: Nametable (0 or 1)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro PPU_ATTRIBUTE_ADDRESS_HI_BYTE
    asl
    asl                             ; multiply by 4, get either $00 -> $00 or $01 -> $04
    clc
    adc #$23                        ; add to $23 to get either $23 or $27
.endmacro