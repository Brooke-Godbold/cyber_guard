;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure for handling Title Loop
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc GameOverScreen

InitVariables:
    lda #$00
    sta PPU_MASK
    sta PPU_SCROLL
    sta PPU_SCROLL

    sta Row
    sta Column
    sta AttrColumn
    sta VBlankDone

AudioEngineInit:
    ldx #<music_data_cyber_menu
    ldy #>music_data_cyber_menu

    lda #1                          ; NTSC Audio
    jsr famistudio_init

    lda #0
    jsr famistudio_music_play

    lda #1
    sta CHR_BANK                    ; switch CHR Bank to title CHR

    lda #%00001100                  ; disable NMI, Set Background to first Pattern Table, 32-bit increment mode
    sta PPU_CTRL

    lda #<GameOverData
    sta Parameters
    lda #>GameOverData
    sta Parameters+1
    jsr LoadBackground              ; load in background tiles into NameTables for Title Screen

    lda #<AttributeDataGameOver
    sta Parameters
    lda #>AttributeDataGameOver
    sta Parameters+1
    jsr LoadAttributes              ; load in attributes into NameTables for Title Screen

    jsr LoadPalette                 ; load Palette Data

EnableNMI:
    bit PPU_STATUS
    sta PPU_SCROLL
    sta PPU_SCROLL                  ; reset scroll
    lda #%10001000                  ; Enable NMI, Set Background to first Pattern Table, normal increment mode
    sta PPU_CTRL

GameOverLoop:

WaitForVBlank:
    lda VBlankDone
    beq WaitForVBlank               ; wait for VBlank to finish drawing

RenderingDisable:
    lda #$00
    sta PPURendering                ; disable PPU Mask Rendering for next Rendering Cycle
	bit PPU_STATUS
    sta PPU_SCROLL
    sta PPU_SCROLL                  ; disable & reset scroll

EnableRendering:
    lda #%00011110                  ; enable PPU Mask Rendering for next Rendering Cycle
    sta PPURendering

TitleScreenLogic:
    jsr ReadControllers
    
    lda Controller
    and #BUTTON_START
    beq ResetDrawFlag
        lda #State::TITLESCREEN
        sta GameState
        jsr famistudio_music_stop
        jsr famistudio_update

ResetDrawFlag:
    lda #0
    sta VBlankDone                  ; reset VBlank draw flag

CheckGameState:
    lda GameState
    cmp #State::GAMEOVER
    beq GameOverLoop

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure for handling TitleScreen NMI
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc GameOverNMI

EnableRendering:
    jsr RefreshRendering

AudioUpdate:
    jsr famistudio_update

    rts
.endproc