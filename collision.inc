;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Subroutine to Check Missile Collisions with Enemies
;;      - X Register: current Actor index
;;      - Loop through every Enemy Actor to check collision
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc CheckMissileEnemiesCollision
    lda #8
    sta Parameters
    lda #4
    sta Parameters+1
    jsr SetRelativeCollisionPoint

    txa
    pha
    jsr CheckEnemyCollision
    pla
    tax

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Subroutine to Check Player Collision with Enemies
;;      - X Register: current Actor index
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc CheckPlayerEnemiesCollision
    lda #8
    sta Parameters

    lda CollisionFrame
    beq :+                              ; if Collision Frame is #0, load 0 into Collision Y Point
        lda #16                             ; Else: load 16 into Collision Y Point
    :
    sta Parameters+1
    jsr SetRelativeCollisionPoint

    txa
    pha
    jsr CheckEnemyCollision
    pla
    tax

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Subroutine to Check Collisions with Enemies
;;      - Loop through every Enemy Actor to check collision
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc CheckEnemyCollision
    ldx #0
    stx CollisionFlag                   ; reset collision flag to zero
ActorsLoop:
    lda ActorsArray+Actor::Type,x
    cmp #ActorType::SHOOTER
    beq CollisionCheck
    cmp #ActorType::GLIDER
    beq CollisionCheck

    jmp NextCheck

CollisionCheck:
    lda ActorsArray+Actor::XPos,x
    sta CollisionBox+2                  ; store first X collision box point
    clc
    adc #16
    sta CollisionBox+3                  ; store second X collision box point

    lda ActorsArray+Actor::YPos,x
    sta CollisionBox+4                  ; store first Y collision box point
    clc
    adc #16
    sta CollisionBox+5                  ; store second Y collision box point

    jsr PointInsideBoundingBox

    lda CollisionFlag
    beq NextCheck                       ; did collision happen
        lda #01                             ; Then: set the enemy Actor to destroyed, to be handled by it's own Update
        sta ActorsArray+Actor::Destroyed,x
        jmp FinishActors

NextCheck:
    txa
    NEXT_ACTOR

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Subroutine to set the Collision Point to be checked
;;      - X Register: Collision Point Actor's index
;;      - Parameters: relative x pixels, relative y pixels
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc SetRelativeCollisionPoint
    lda ActorsArray+Actor::XPos,x
    clc
    adc Parameters
    sta CollisionBox                    ; set collision point x

    lda ActorsArray+Actor::YPos,x
    clc
    adc Parameters+1
    sta CollisionBox+1                  ; set collision point y

    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Subroutine to validate if a point is inside a Bounding Box
;;      - Point X, Y: CollisionBox 0 & 1, Box X1, X2, Y1, Y2: CollisionBox 2, 3, 4, 5
;;      - Sets CollisionFlag to 1 if true
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc PointInsideBoundingBox
    lda CollisionBox
    cmp CollisionBox+2
    bcc EndCollisionCheck               ; is Point X less than Bounding Box X1
    cmp CollisionBox+3
    bcs EndCollisionCheck               ; is Point X greater than Bounding Box X2

    lda CollisionBox+1
    cmp CollisionBox+4
    bcc EndCollisionCheck               ; is Point Y less than Bounding Box Y1
    cmp CollisionBox+5
    bcs EndCollisionCheck               ; is Point Y greater than Bounding Box Y2

PointInside:
    lda #1
    sta CollisionFlag

EndCollisionCheck:
    rts
.endproc