;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Enumeration for different Actor Types
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.enum ActorType
    NULL = 0
    SPRITE0 = 1
    PLAYER = 2
    MISSILE = 3
    SHOOTER = 4
    GLIDER = 5
    EXPLOSION = 6
.endenum

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Struct which holds data structure for Actors
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.struct Actor
    Type            .byte
    XPos            .byte
    YPos            .byte
    Frame           .byte
    FrameCounter    .byte
    Destroyed       .byte
.endstruct

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to increment Actors Array
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro NEXT_ACTOR
    clc
    adc #.sizeof(Actor)
    tax                                 ; add offset to move to start position of next Actor in RAM
    cmp #MAX_ACTORS * .sizeof(Actor)    ; is current position last position of array
    beq FinishActors                        ; Then: finish looping
        jmp ActorsLoop

FinishActors:
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to add Sprite Zero
;;      - load Sprite Zero at x = 12, y = 60
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro ADD_SPRITE_ZERO
    lda #$3C
    sta Parameters+2
    lda #$0C
    sta Parameters+1
    lda #ActorType::SPRITE0
    sta Parameters
    jsr AddNewActor
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to add Player
;;      - load Player at y = 72, x = 32
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro ADD_PLAYER
    lda PlayerYPos+1
    sta Parameters+2
    lda PlayerXPos+1
    sta Parameters+1
    lda #ActorType::PLAYER
    sta Parameters
    jsr AddNewActor
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to add a Missile
;;      - load Missile at Player X and Y
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro ADD_MISSILE
    lda PlayerYPos+1
    clc
    adc #8                      ; calculate missile y-offset in pixels
    sta Parameters+2

    lda PlayerXPos+1
    clc
    adc #12                     ; calculate missile x-offset in pixels
    sta Parameters+1

    lda #ActorType::MISSILE
    sta Parameters
    jsr AddNewActor
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to handle creating an explosion at X and Y of current Actor
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro ADD_EXPLOSION
    lda #ActorType::EXPLOSION
    sta Parameters
    lda ActorsArray+Actor::XPos,x
    sta Parameters+1
    lda ActorsArray+Actor::YPos,x
    sta Parameters+2

    jsr AddNewActor
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Macro to handle clearing an Actor from the array
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.macro REMOVE_ACTOR
    lda #ActorType::NULL
    sta ActorsArray+Actor::Type,x
.endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to add an Actor to the Actor Array
;;      - Parameters order expected (Type, XPos, YPos)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc AddNewActor
    txa
    pha

    ldx #0
ActorLoop:
    cpx #MAX_ACTORS * .sizeof(Actor)    ; if reached last register of Actor's Array
    beq EndActorRoutine                     ; Then: do not add any new actors
    lda ActorsArray+Actor::Type,x       ; load type of actor at current x offset
    cmp #ActorType::NULL                ; if type of current actor at index is NULL
    beq AddNewActorToArray                  ; Then: add a new Actor to the free position
NextActor:
    txa
    clc
    adc #.sizeof(Actor)
    tax                                 ; add offset to move to start position of next Actor in RAM
    jmp ActorLoop

AddNewActorToArray:
    lda Parameters
    sta ActorsArray+Actor::Type,x       ; pull Type from Parameters, store into Type of Actor at X

    cmp #ActorType::MISSILE             ; is added Actor a missile
    bne :+                                  ; Then: increment missiles on screen
        inc Missiles
    :

    lda Parameters+1
    sta ActorsArray+Actor::XPos,x     ; pull XPos from Parameters, store into XPos of Actor at X
    lda Parameters+2
    sta ActorsArray+Actor::YPos,x     ; pull YPos from Parameters, store into YPos of Actor at X

    lda #0
    sta ActorsArray+Actor::Frame,x
    sta ActorsArray+Actor::FrameCounter,x
    sta ActorsArray+Actor::Destroyed,x      ; reset Actor state

EndActorRoutine:
    pla
    tax

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Render all Actors in Actors Array
;;      - Loop through Actors array in memory, perform logic for each Actor type
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc RenderActors
    ldx #0
    ldy #0

ActorsLoop:
    txa
    pha                                 ; buffer current Actor Index from X into Stack
    lda ActorsArray+Actor::Type,x       ; load type of actor at current x offset

    cmp #ActorType::SPRITE0             ; is current Actor Sprite Zero
    bne :+
        lda ActorsArray+Actor::YPos,x
        sta Parameters
        lda #$20
        sta Parameters+1
        lda #%00100001
        sta Parameters+2
        lda ActorsArray+Actor::XPos,x
        sta Parameters+3
        lda #0
        sta Parameters+4

        jsr DrawSprite
        jmp NextActor
    :

    cmp #ActorType::PLAYER              ; is current Actor Player
    bne FinishPlayer
        lda #<PlayerSprite
        sta Pointer
        lda #>PlayerSprite
        sta Pointer+1

        jsr DrawMetasprite
        jmp NextActor
FinishPlayer:

    cmp #ActorType::MISSILE             ; is current Actor Missile
    bne :+
        lda ActorsArray+Actor::YPos,x
        sta Parameters
        lda #%00000011
        sta Parameters+2
        lda ActorsArray+Actor::XPos,x
        sta Parameters+3
        lda #0
        sta Parameters+4

        lda ActorsArray+Actor::Frame,x
        tax
        lda MissileSprite,x
        sta Parameters+1

        jsr DrawSprite
        jmp NextActor
    :

    cmp #ActorType::SHOOTER             ; is current Actor Shooter Enemy
    bne FinishShooter
        lda #<EnemyShooter
        sta Pointer
        lda #>EnemyShooter
        sta Pointer+1

        jsr DrawMetasprite
        jmp NextActor
FinishShooter:

    cmp #ActorType::GLIDER             ; is current Actor Glider Enemy
    bne FinishGlider
        lda #<EnemyGlider
        sta Pointer
        lda #>EnemyGlider
        sta Pointer+1

        jsr DrawMetasprite
        jmp NextActor
FinishGlider:

    cmp #ActorType::EXPLOSION          ; is current Actor Explosion
    bne FinishExplosion
        lda #<ExplosionSprite
        sta Pointer
        lda #>ExplosionSprite
        sta Pointer+1

        jsr DrawMetasprite
        jmp NextActor
FinishExplosion:

NextActor:
    pla                                 ; pull buffered Actor Index from Stack
    NEXT_ACTOR

    tya
    pha

LoopTrailingTiles:
    cpy LastOAMCount
    bcs :+
        lda #$FF
        sta (SpritePtr),y
        iny
        sta (SpritePtr),y
        iny
        sta (SpritePtr),y
        iny
        sta (SpritePtr),y
        iny
        jmp LoopTrailingTiles
    :

    pla
    sta LastOAMCount

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Update Actor positions in Actors Array
;;      - Loop through Actors array in memory, perform logic for each Actor type
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc UpdateActors
    ldx #0

ActorsLoop:
    lda ActorsArray+Actor::Type,x       ; load type of actor at current x offset

    cmp #ActorType::NULL
    beq SkipAnimation
        tay
        lda ActorsArray+Actor::FrameCounter,x
        clc
        adc #ANIMATION_SPEED                ; update animation time for Actor
        sta ActorsArray+Actor::FrameCounter,x
        tya
    SkipAnimation:

    cmp #ActorType::PLAYER
    bne SkipPlayer
        lda PlayerXPos+1
        sta ActorsArray+Actor::XPos,x       ; pull XPos from Parameters, store into XPos of Actor at X
        lda PlayerYPos+1
        sta ActorsArray+Actor::YPos,x       ; pull YPos from Parameters, store into YPos of Actor at X

    CheckPlayerCollision:
        jsr CheckPlayerEnemiesCollision
        lda CollisionFlag
        bne PlayerCollision
        jmp NextActor

    PlayerCollision:
        PLAY_EXPLOSION_SFX

    DestroyPlayer:
        REMOVE_ACTOR
        ADD_EXPLOSION
        jsr famistudio_music_stop
        jsr famistudio_update
        lda Clock60
        sta PlayerKilledTime
        jmp NextActor
    SkipPlayer:

    cmp #ActorType::MISSILE             ; is current Actor Missile
    bne SkipMissile

        lda ActorsArray+Actor::XPos,x
        clc
        adc #MISSILE_SPEED
        sta ActorsArray+Actor::XPos,x

        bcs DestroyMissile

    CheckMissileCollision:
        jsr CheckMissileEnemiesCollision
        lda CollisionFlag
        bne MissileCollision

    AnimateMissile:
        lda ActorsArray+Actor::FrameCounter,x
        bne :+                              ; has animation time completed
            lda ActorsArray+Actor::Frame,x      ; Then: toggle animation frame
            eor #1
            sta ActorsArray+Actor::Frame,x
        :

        jmp FinishMissile

    MissileCollision:
        PLAY_EXPLOSION_SFX

    DestroyMissile:
        REMOVE_ACTOR
        dec Missiles                ; decrement missiles on screen

    FinishMissile:
        jmp NextActor
    SkipMissile:

    cmp #ActorType::SHOOTER             ; is current Actor Enemy Shooter
    bne SkipShooter
        lda ActorsArray+Actor::XPos,x
        sec
        sbc #SHOOTER_SPEED
        sta ActorsArray+Actor::XPos,x

        bcc DestroyShooter

        lda ActorsArray+Actor::Destroyed,x
        beq FinishShooter
            lda #1
            sta Parameters
            jsr IncrementScore
            ADD_EXPLOSION

    DestroyShooter:
        REMOVE_ACTOR
    
    FinishShooter:
        jmp NextActor
    SkipShooter:

    cmp #ActorType::GLIDER             ; is current Actor Enemy Glider
    bne SkipGlider

        lda ActorsArray+Actor::Frame,x
        cmp #0
        beq GliderDown
            lda ActorsArray+Actor::YPos,x
            sec
            sbc #GLIDER_SPEED
            sta ActorsArray+Actor::YPos,x
            jmp FinishGliderVertical
        GliderDown:
            lda ActorsArray+Actor::YPos,x
            clc
            adc #GLIDER_SPEED
            sta ActorsArray+Actor::YPos,x

        FinishGliderVertical:
            lda ActorsArray+Actor::XPos,x
            sec
            sbc #GLIDER_SPEED
            sta ActorsArray+Actor::XPos,x

            bcc DestroyGlider

            lda ActorsArray+Actor::Destroyed,x
            beq NextActor
                ADD_EXPLOSION

        DestroyGlider:
            REMOVE_ACTOR
        
        jmp NextActor
    SkipGlider:

    cmp #ActorType::EXPLOSION               ; is current Actor Explosion
    bne SkipExplosion
        lda ActorsArray+Actor::XPos,x
        sec
        sbc #EXPLOSION_SPEED
        sta ActorsArray+Actor::XPos,x

        lda ActorsArray+Actor::FrameCounter,x
        bne NextActor                       ; has animation time completed
            inc ActorsArray+Actor::Frame,x      ; Then: increment animation frame
            lda ActorsArray+Actor::Frame,x
            cmp #3
            bne NextActor                       ; has animation completed all frames
                REMOVE_ACTOR                        ;Then: destroy Actor
        
        jmp NextActor
    SkipExplosion:

NextActor:
    txa
    NEXT_ACTOR

    rts
.endproc