;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure for handling Level Loop
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc Level

InitVariables:
    bit PPU_STATUS

    lda #$00
    sta PPU_MASK
    sta PPU_SCROLL
    sta PPU_SCROLL

    sta Frame
    sta Clock60
    sta XScroll+1
    sta XScroll
    sta Row
    sta Column
    sta AttrColumn
    sta LevelSect
    sta VBlankDone
    sta CurNam
    sta LastMissileTime
    sta PlayerKilledTime
    sta CurrentEnemy
    sta Score
    sta Score+1
    sta Score+2

    sta PlayerXVel
    sta PlayerYVel

    sta SpritePtr
    lda #$02
    sta SpritePtr+1         ; initialize Sprite OAM Pointer to $0200 in RAM

    lda #$48
    sta PlayerYPos+1
    lda #$20
    sta PlayerXPos+1        ; initialize player position

AudioEngineInit:
    ldx #<music_data_cyber_theme
    ldy #>music_data_cyber_theme

    lda #1                  ; NTSC Audio
    jsr famistudio_init

    lda #0
    jsr famistudio_music_play

    ldx #<sounds
    ldy #>sounds
    jsr famistudio_sfx_init

Main:
    lda #0
    sta CHR_BANK            ; switch CHR Bank to game CHR

    ADD_SPRITE_ZERO
    ADD_PLAYER

InitializeBackground:
    lda #%00010100          ; disable NMI, Set Background to second Pattern Table, 32-bit increment mode
    sta PPU_CTRL

    lda LevelSequenceLow
    sta Parameters
    lda LevelSequenceHigh
    sta Parameters+1
    jsr LoadBackground      ; load initial Background Data into first nametable

    lda AttributeSequenceLow
    sta Parameters
    lda AttributeSequenceHigh
    sta Parameters+1
    jsr LoadAttributes      ; load initial attributes into first nametable

    jsr LoadPalette         ; load Palette Data

    jsr SetColumnAddresses      ; set address of next column to be drawn
    jsr SetAttributeAddresses   ; set address of next attributes to be drawn

EnableNMI:
    bit PPU_STATUS
    lda #%10010100          ; Enable NMI, Set Background to second Pattern Table, 32-bit increment mode
    sta PPU_CTRL

GameLoop:

WaitForVBlank:
    lda VBlankDone
    beq WaitForVBlank       ; wait for VBlank to finish drawing

RenderingDisable:
    lda #$00
    sta PPURendering        ; disable PPU Mask Rendering for next Rendering Cycle
	bit PPU_STATUS
    sta PPU_SCROLL
    sta PPU_SCROLL          ; disable & reset scroll

VRAMUpdates:
    jsr UpdateScrollPosition
    jsr CheckColumnAddresses
    jsr RenderActors

RenderingEnable:
    lda #%00011110          ; enable PPU Mask Rendering for next Rendering Cycle
    sta PPURendering

GameLogic:
    jsr SetScroll
	;jsr UpdateScrollPosition
    jsr UpdateLevelSection
    jsr SpawnNextEnemy

GameOverCheck:
    lda PlayerKilledTime
    bne GameOverDelay       ; is player killed
        jsr ReadControllers     ; Then: skip reading player input or updating position
        jsr CheckButtons
        jsr UpdatePlayerPosition
        jmp FinishGameOver
GameOverDelay:
    lda Clock60
    sec
    sbc PlayerKilledTime
    cmp #GAME_OVER_DELAY
    bcc FinishGameOver      ; has player killed time passed delay time
        lda #State::GAMEOVER
        sta GameState

        ldx #0
    ActorsLoop:
        REMOVE_ACTOR
        txa
        NEXT_ACTOR

FinishGameOver:
    lda CollisionFrame
    eor #1
    sta CollisionFrame      ; toggle the Collision Frame

    jsr UpdateActors

ResetDrawFlag:
    lda #0
    sta VBlankDone          ; reset VBlank draw flag

CheckGameState:
    lda GameState
    cmp #State::LEVEL
    beq GameLoop

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure for handling Level NMI
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc LevelNMI

HandleScore:
	jsr DrawScore

NewColumnCheck:
    lda NewColumn
    cmp #1                      ; if new column flag set
    bne NewAttributesCheck
        jsr DrawNewColumn       ; Then: draw a new column of tiles

NewAttributesCheck:
    lda NewAttrs
    cmp #1
    bne EnableRendering
        jsr DrawNewAttributes   ; draw a new set of attributes

EnableRendering:
	UPDATE_SCROLL_POS
    jsr RefreshRendering

FrameCounter:
    jsr SetGameClock            ; increment game clock

AudioUpdate:
    jsr famistudio_update

    rts
.endproc