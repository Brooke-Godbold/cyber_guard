;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Subroutine to spawn the next Enemy
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc SpawnNextEnemy
    lda CurrentEnemy
    asl
    asl                     ; multiply by 4 to get correct Current Enemy offset
    tax
    lda EnemySequence,x
    cmp Clock60             ; is Clock Time same as Current Enemy time
    bne EndSpawnEnemy           ; Else: skip spawning enemy

    inx                     ; increment x to first attribute
    ldy #0
SpawnEnemyLoop:
    lda EnemySequence,x     ; load attributes for Enemy at offset x
    sta Parameters,y
    inx
    iny
    cpy #3                  ; have all 3 attributes been loaded
    bne SpawnEnemyLoop          ; Else: continue looping attributes

    jsr AddNewActor         ; add Enemy Actor

    inc CurrentEnemy

EndSpawnEnemy:
    rts
.endproc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Procedure to update the current level section to be drawn
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.proc UpdateLevelSection
    lda #0
    cmp XScroll
    bne EndUpdate
        cmp XScroll+1
        bne EndUpdate
            lda LevelSect
            clc
            adc #1
            and #%00000111                  ; mask LevelSection to max bits of 8
            sta LevelSect
EndUpdate:
    rts
.endproc