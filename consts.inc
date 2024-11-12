;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Constants for PPU registers mapped from addresses $2000 to $2007
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PPU_CTRL        = $2000
PPU_MASK        = $2001
PPU_STATUS      = $2002
OAM_ADDR        = $2003
OAM_DATA        = $2004
PPU_SCROLL      = $2005
PPU_ADDR        = $2006
PPU_DATA        = $2007
PPU_OAM_DMA     = $4014
CHR_BANK        = $8000

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Constants for APU registers mapped from addresses $4000 to $4015
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
APU_SQ1_ENV     = $4000
APU_SQ1_LO      = $4002
APU_SQ_1HI      = $4003
APU_SQ2_ENV     = $4004
APU_SQ2_LO      = $4006
APU_SQ2_HI      = $4007
APU_TRI_CTRL    = $4008
APU_TRI_LO      = $400A
APU_TRI_HI      = $400B
APU_NSE_ENV     = $400C
APU_NSE_PRD     = $400E
APU_NSE_LEN     = $400F
APU_SMPL_CTRL   = $4010
APU_SMPL_CNT    = $4011
APU_SMPL_ADDR   = $4012
APU_SMPL_LEN    = $4013
APU_FLAGS       = $4015

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Controller ports and buttons
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
JOYPAD1         = $4016
JOYPAD2         = $4017

BUTTON_A        = $80          ; 10000000
BUTTON_B        = $40          ; 01000000
BUTTON_SELECT   = $20          ; 00100000
BUTTON_START    = $10          ; 00010000
BUTTON_UP       = $08          ; 00001000
BUTTON_DOWN     = $04          ; 00000100
BUTTON_LEFT     = $02          ; 00000010
BUTTON_RIGHT    = $01          ; 00000001

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Game Logic related Consts
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
MAX_ACTORS      = 10
MAXSPEED        = 127                       ; max speed per 256 pixels
ACCEL           = 3                         ; accel per 256 pixels
BRAKE           = 4                         ; brake power per 256 pixels
MAX_MISSILES    = 4                         ; maximum missiles on screen
MISSILE_DELAY   = 1                         ; delay time between missiles being shot
SCROLLSPEED     = 64                        ; screen scroll speed
MISSILE_SPEED   = 4                         ; missile speed in pixels per frame
SHOOTER_SPEED   = 1                         ; enemy shooter speed in pixels per frame
GLIDER_SPEED    = 1                         ; enemy glider speed in pixels per frame
EXPLOSION_SPEED = 1                         ; speed of explosion across screen
ANIMATION_SPEED = 32                        ; global animation speed
GAME_OVER_DELAY = 3                         ; game over delay time