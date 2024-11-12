;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Read Controller Data
;;      - Read buttons which are pressed
;;      - Load in button data 1 bit at a time
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc ReadControllers
    lda Controller
    sta LastController              ; buffer controller state of last frame

    lda #1
    sta Controller                  ; init Controller with 1 bit

    sta JOYPAD1                     ; set controller latch to INPUT=1, read pressed buttons
    lsr                             ; shift right to #0
    sta JOYPAD1                     ; set controller latch to OUTPUT=0, send state to NES

:
    lda JOYPAD1                     ; read 1 bit from controller, signal clock to shift next bits
    lsr                             ; shift value right into Carry
    rol Controller                  ; rotate Carry value into Controller, shifting Controller bits left
    bcc :-                          ; check Carry flag after this operation
                                        ; if Controller original first bit shifts into Carry (after 8 loops), break loop
    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to Check Buttons during Gameplay
;;      - Logical AND for each button; if returns as 1, then button was pressed, else break to next button
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc CheckButtons

CheckRightButton:
    lda Controller
    and #BUTTON_RIGHT
    beq BrakeRight
        lda PlayerXVel              ; right is pressed
        bmi BrakeRight              ; if velocity is negative, start braking
            clc
            adc #ACCEL              ; add acceleration to velocity
            cmp #MAXSPEED           ; check if max speed reached
            bcc :+
                lda #MAXSPEED       ; force A to max speed
            :
            sta PlayerXVel
            jmp CheckLeftButton
    BrakeRight:
        lda PlayerXVel              ; right not pressed
        bmi CheckLeftButton         ; if velocity is negative, skip
            cmp #BRAKE              ; check if brake value reached
            bcs :+
                lda #BRAKE+1        ; force A to brake value
            :
            sbc #BRAKE              ; subtract brake power from velocity
            sta PlayerXVel

CheckLeftButton:
    lda Controller
    and #BUTTON_LEFT
    beq BrakeLeft
        lda PlayerXVel              ; left is pressed
        beq :+
            bpl BrakeLeft           ; if velocity is positive, start braking
        :
        sec
        sbc #ACCEL                  ; subtract acceleration from velocity
        cmp #256-MAXSPEED           ; check if max speed reached
        bcs :+
            lda #256-MAXSPEED       ; force A to max speed
        :
        sta PlayerXVel
        jmp CheckDownButton
    BrakeLeft:
        lda PlayerXVel              ; left not pressed
        bpl CheckDownButton         ; if velocity is positive, skip
            cmp #256-BRAKE          ; check if brake value reached
            bcc :+
                lda #256-BRAKE      ; force A to brake value
            :
            adc #BRAKE              ; subtract brake power from velocity
            sta PlayerXVel

CheckDownButton:
    lda Controller
    and #BUTTON_DOWN
    beq BrakeDown
        lda PlayerYVel              ; right is pressed
        bmi BrakeDown               ; if velocity is negative, start braking
            clc
            adc #ACCEL              ; add acceleration to velocity
            cmp #MAXSPEED           ; check if max speed reached
            bcc :+
                lda #MAXSPEED       ; force A to max speed
            :
            sta PlayerYVel
            jmp CheckUpButton
    BrakeDown:
        lda PlayerYVel              ; right not pressed
        bmi CheckUpButton         ; if velocity is negative, skip
            cmp #BRAKE              ; check if brake value reached
            bcs :+
                lda #BRAKE+1        ; force A to brake value
            :
            sbc #BRAKE              ; subtract brake power from velocity
            sta PlayerYVel

CheckUpButton:
    lda Controller
    and #BUTTON_UP
    beq BrakeUp
        lda PlayerYVel              ; left is pressed
        beq :+
            bpl BrakeUp             ; if velocity is positive, start braking
        :
        sec
        sbc #ACCEL                  ; subtract acceleration from velocity
        cmp #256-MAXSPEED           ; check if max speed reached
        bcs :+
            lda #256-MAXSPEED       ; force A to max speed
        :
        sta PlayerYVel
        jmp CheckAButton
    BrakeUp:
        lda PlayerYVel              ; left not pressed
        bpl CheckAButton         ; if velocity is positive, skip
            cmp #256-BRAKE          ; check if brake value reached
            bcc :+
                lda #256-BRAKE      ; force A to brake value
            :
            adc #BRAKE              ; subtract brake power from velocity
            sta PlayerYVel

CheckAButton:
    lda Controller
    and #BUTTON_A
    beq FinishButtons
        lda LastController
        and #BUTTON_A               ; was Button A pressed last frame
        bne FinishButtons               ; Then: skip logic for Button A
            lda Missiles
            cmp #MAX_MISSILES           ; is max number of missiles on screen
            beq FinishButtons               ; Then: skip logic for Button A
                lda Clock60
                sec
                sbc LastMissileTime
                cmp #MISSILE_DELAY
                bcc FinishButtons           ; has too little delay time passed
                    ADD_MISSILE                 ; Then: skip logic for Button A
                    lda Clock60
                    sta LastMissileTime

                    lda #0
                    ldx #FAMISTUDIO_SFX_CH0
                    jsr famistudio_sfx_play

FinishButtons:
    rts
.endproc