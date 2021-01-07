	; -------------------------------------
	; --------Pseudo Instructions --------
	; --------------------------
	; -------Turn Left --------
	; Called from WhichWay subroutine  (see Table 5.1)
	; The input and output is register r24
	; register SREG is modified by this subroutine
	TurnLeft:

	ldi ZL, low(left_table<<1)					//orienting z pointer 
	ldi ZH, high(left_table<<1)
	add ZL, r24									//by adding the direction to my Z pointer
												//the appropriate turn choice is made
	lpm r24, Z									//update the new value of direction

	ret										//terminate subroutine

	; --------------------------
	; -------Turn Right -------
	; Called from WhichWay subroutine  (see Table 5.1)
	; The input and output is register r24
	; register SREG is modified by this subroutine
	TurnRight:

	ldi ZL, low(right_table<<1)					//orienting z pointer
	ldi ZH, high(right_table<<1)
	add ZL, r24								   	//by adding the direction to my Z pointer
											   	//the appropriate turn choice is made							
	lpm r24, Z								   	//update the new value of direction

	ret										//terminate subroutine



	; --------------------------
	; -------Turn Around -------
	; Called from WhichWay subroutine  (see Table 5.1)
	; The input and output is register r24
	; register SREG is modified by this subroutine
TurnAround:
		
	ldi ZL, low(turn_table<<1)					//orienting z pointer
	ldi ZH, high(turn_table<<1)
	add ZL, r24									//by adding the direction to my Z pointer
												//the appropriate turn choice is made
	lpm r24, Z									//update the new value of direction
	jmp IamTurnAround										//terminate subroutine

HitWall:
	push 	r16									//perserving memory usage of register 16
	rcall 	DrawDirection						//output the direction the bear is facing
	mov		r16,r24								//copy the data necessary to determine if the bear hit a wall
	mov 	r24, r22							
	rcall 	DrawRoom							//output the room the bear is in
	
	and 	r24, r16							//is there a wall infront of the bear

	pop		r16									//return original value of register 16
	ret 										//terminate subroutine
; ----------------


TestHitWall:
	lds		r22,room							//get data to test if the bear hit a wall
	lds 	r24,dir
	mov		r16,spiLEDS
	rcall 	HitWall								//did the bear hit a wall?
	tst 	r24									
	breq 	noWall								
	sbr		r16,0b00000010						//turn on spiLED 1 
	cbr		r16,0b00000001						//turn off spiLED 0
	rjmp	overTheWall							//proceed to subroutine termination
noWall:
	sbr		r16, 0b00000001						//turn on spiLED0 
	cbr		r16, 0b00000010						//turn off spiLED1		
overTheWall:
	mov		spiLEDS, r16						//display spiLEDS to user
	ret 										//terminate subroutine

RightPaw:
	rcall	TurnRight							//call the turn right Subroutine
	rcall	HitWall								//is there a wall infront of the bear?
	ret

TestRightPaw:
	lds		r22,room							//get data for test bench
	lds 	r24,dir
	mov		r16,spiLEDS
	rcall 	RightPaw							//turn the bear and get direction and turn data
	tst 	r24									//is there a wall infront of the bear if 
												//the bear makes another right?  
	breq 	noWall2
	sbr		r16,0b00001000						//turn on spiLED3
	cbr		r16,0b00000100						//turn off spiLED2
	rjmp	overTheWall2						
noWall2:
	sbr		r16, 0b00000100						//turn on spiLED2
	cbr		r16, 0b00001000						//turn off spiLED3
overTheWall2:
	mov		spiLEDS, r16						//display updated LEDs to user
	ret											//terminate subroutine

LeftPaw:
	rcall	TurnLeft							//turn the bear left
	rcall	HitWall								//is there a wall infront of the bear
	ret											//terminate subroutine

TestLeftPaw:
	lds		r22,room							//get data for test bench
	lds 	r24,dir
	mov		r16,spiLEDS
	rcall 	LeftPaw								//turn the bear left
	tst 	r24									//is there a wall infront of the bear if it turns 
												//left again
	breq 	noWall1
	sbr		r16,0b00100000						//turn on spiLED5
	cbr		r16,0b00010000						//turn off spiLED4
	rjmp	overTheWall1
noWall1:
	sbr		r16, 0b00010000						//turn on spiLED4
	cbr		r16, 0b00100000						//turn off spiLED5
overTheWall1:
	mov		spiLEDS, r16						//copy updated LED data to the user
	ret 										//terminate subroutine

	push 	r16									//perserving memory usage of register 16
	rcall 	DrawDirection						//output the direction the bear is facing
	mov		r16,r24								//copy the data necessary to determine if the bear hit a wall
	mov 	r24, r22							
	rcall 	DrawRoom							//output the room the bear is in
	
	and 	r24, r16							//is there a wall infront of the bear

	pop		r16									//return original value of register 16
	ret 										//terminate subroutine
; ----------------

TakeAStep:

	push r16
	

	ldi ZH, high(control_table)
	ldi ZL, low(control_table)
	
	ldi r16, 0x02
	mul r20, r16
	add ZL, r0
	adc ZH, r1
	
	pop r16
	ijmp


	control_table:
	inc r24
	ret
	
	inc r22
	ret

	dec r22
	ret

	dec r24
	ret



EnterRoom:
	push r30
	push r31
	push r16
	push r15
	push r0
	push r1
	push r2
	
	clr r2
	in r15, sreg

	ldi  ZL,low(theMaze<<1)   
	ldi  ZH,high(theMaze<<1)

	ldi r16, 0x14
	mul r24, r16
	add r0, r22
	
	
	adc r1,r2 
	
	add zl, r0
	adc zh, r1
	
	lpm r24, Z
	
	out sreg, r15
	pop r2
	pop r1
	pop r0
	pop r15
	pop r16
	pop r31
	pop r30
ret

; --------------------------
; ---Is this a Hallway? ---
; input r24 = current room (room), r22 = bees
; APPROVED FOR F・7 ONLY 3
; output r24 = 0 if answer is false (r24 == 0), 
; otherwise the answer is true (r24 != 0).
; The answer is true ifthe bear is in a hallway 
; without any bees.
; no registers are modified by this subroutine
; -------------------------
IsHallway:
// return (bees == 0x00 && (room == 0x09 || room == 0x06));
push r16
push r15
in r15, sreg

tst r22
brne answer_is_no
cpi r24, 0x09
breq answer_is_yes
cpi r24, 0x06
breq answer_is_yes
cpi r24, 0x03
breq answer_is_yes
cpi r24, 0x0C
breq answer_is_yes
cpi r24, 0x05
breq answer_is_yes
cpi r24, 0x0A
breq answer_is_yes

answer_is_no:
ldi   r24, false               // room is not a hallway or contains bees
rjmp  endOfHallway

answer_is_yes:
ldi   r24, true
rjmp endOfHallway


endOfHallway:
out sreg, r15
pop r15
pop r16
ret

TimerOverFlow:

	push 	r16
	push    reg_F
	in      reg_F,SREG
	
	ldi  	r16, 0x01
	sts 	TIFR0, r16
	rcall 	pulse

	out 	sreg, reg_f
	pop 	reg_f
	pop  	r16
	
	reti
