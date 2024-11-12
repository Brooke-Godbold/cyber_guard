;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure for handling Title Loop
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc TitleScreen

InitVariables:
    lda #$00
    sta PPU_MASK
    sta PPU_SCROLL
    sta PPU_SCROLL

    sta Row
    sta Column
    sta AttrColumn
    sta XScroll+1
    sta XScroll
    sta LevelSect
    sta VBlankDone
    sta CurNam
    sta TitleState

AudioEngineInit:
    ldx #<music_data_cyber_menu
    ldy #>music_data_cyber_menu

    lda #1                          ; NTSC Audio
    jsr famistudio_init

    lda #0
    jsr famistudio_music_play

    lda #1
    sta CHR_BANK                    ; switch CHR Bank to title CHR

LoopTitleBackgrounds:
    lda #%00001100                  ; disable NMI, Set Background to first Pattern Table, 32-bit increment mode
    sta PPU_CTRL

    ldx CurNam
    lda TitleSequenceLow,x
    sta Parameters
    lda TitleSequenceHigh,x
    sta Parameters+1
    jsr LoadBackground              ; load in background tiles into NameTables for Title Screen

    ldx CurNam
    lda TitleAttributesLow,x
    sta Parameters
    lda TitleAttributesHigh,x
    sta Parameters+1
    jsr LoadAttributes              ; load in attributes into NameTables for Title Screen

    lda CurNam
    eor #1                          ; invert current nametable to 0 or 1, into accumulator
    sta CurNam

    bne LoopTitleBackgrounds        ; did current nametable revert to 0

    jsr LoadPalette                 ; load Palette Data

    inc TitleState                  ; set Title State

EnableNMI:
    bit PPU_STATUS
    sta PPU_SCROLL
    sta PPU_SCROLL                  ; reset scroll
    lda #%10001000                  ; Enable NMI, Set Background to first Pattern Table, normal increment mode
    sta PPU_CTRL

TitleLoop:

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
    beq HandleScroll
        lda LastController
        and #BUTTON_START           ; was Start pressed last frame
        bne HandleScroll                ; Then: skip logic Start
            lda TitleState
            and #%00000001              ; is Title State on Title Screen
            beq :+
                lda TitleState
                asl
                sta TitleState
            :

            lda TitleState
            and #%00000100              ; is Title State on Text Screen
            beq :+
                lda #State::LEVEL
                sta GameState

                jsr famistudio_music_stop
                jsr famistudio_update
            :

HandleScroll:
    lda TitleState
    and #%00000010              ; is Title State Scrolling
    beq ResetDrawFlag
        jsr ScrollTitle

ResetDrawFlag:
    lda #0
    sta VBlankDone                  ; reset VBlank draw flag

CheckGameState:
    lda GameState
    cmp #State::TITLESCREEN
    beq TitleLoop

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure for handling TitleScreen NMI
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc TitleNMI

EnableRendering:
    jsr RefreshRendering

AudioUpdate:
    jsr famistudio_update

    rts
.endproc