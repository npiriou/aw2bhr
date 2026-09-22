.syntax unified
.cpu arm7tdmi
.thumb

.include "mod/generated/mailbox.inc"

.equ G_MODE,              0x03003FC0
.equ G_GLOBAL_STATE,      0x030032D8
.equ G_ACTIVE_SIDE,       0x030033EC
.equ G_DAY,               0x03004080
.equ G_AI_PHASE,          0x03004780
.equ G_EXECUTOR_STATE,    0x030045D4
.equ G_COMMAND,           0x030046C0
.equ G_RNG_SEED,          0x03001FD4
.equ G_CURRENT_UNIT_ID,   0x03003F38
.equ G_CURRENT_UNIT_PTR,  0x030040D8
.equ G_SOURCE_POSITION,   0x03003F24
.equ G_PATH,              0x03003110
.equ G_PACKED_PATH,       0x030046CC
.equ G_PATH_COST,         0x03004074
.equ MAP_BASE,            0x0201E450
.equ UNIT_RECORDS,        0x02022684
.equ PLAYER_RECORDS,      0x02023284

.equ ORIGINAL_PHASE2,     0x0805D439
.equ DISPATCH_EPILOGUE,   0x08061783
.equ SNAPSHOT_LENGTH,     0x74C
.equ RESPONSE_MIN_LENGTH, 0x24

.section .text.spann_hook, "ax", %progbits
.align 2
.global spann_phase2_arm
.type spann_phase2_arm, %function
.thumb_func
spann_phase2_arm:
    /* This is a switch arm entered by mov pc, not a callable function. */
    push {r4-r7}

    ldr r4, =G_MODE
    ldrb r0, [r4, #1]
    cmp r0, #2                  /* War Room */
    bne inactive_near
    ldrb r0, [r4, #2]
    cmp r0, #0x6c              /* Spann Island */
    bne inactive_near
    ldr r0, =G_ACTIVE_SIDE
    ldrh r0, [r0]
    cmp r0, #2                 /* live side id 2 = Spann Island enemy */
    bne inactive_near
    ldr r0, =MAP_BASE
    ldrh r1, [r0]
    cmp r1, #15
    bne inactive_near
    ldrh r1, [r0, #2]
    cmp r1, #10
    bne inactive_near
    b active

inactive_near:
    bl call_original_phase2
    b return_dispatcher

active:

    ldr r4, =MAILBOX_BASE
    ldr r0, [r4, #MAILBOX_OFF_MAGIC]
    ldr r1, =MAILBOX_MAGIC
    cmp r0, r1
    bne initialize
    ldrh r0, [r4, #MAILBOX_OFF_VERSION]
    cmp r0, #MAILBOX_VERSION
    bne initialize

dispatch_state:
    ldr r0, [r4, #MAILBOX_OFF_FRAME_COUNTER]
    adds r0, #1
    str r0, [r4, #MAILBOX_OFF_FRAME_COUNTER]
    ldr r0, [r4, #MAILBOX_OFF_STATE]
    cmp r0, #MAILBOX_STATE_IDLE
    beq publish_request
    cmp r0, #MAILBOX_STATE_REQUEST_READY
    beq bridge_wait
    cmp r0, #MAILBOX_STATE_WAITING
    beq bridge_wait
    cmp r0, #MAILBOX_STATE_RESPONSE_READY
    beq consume_response
    cmp r0, #MAILBOX_STATE_EXECUTING
    beq execution_complete
    cmp r0, #MAILBOX_STATE_ERROR
    beq bridge_wait
    b return_dispatcher

bridge_wait:
    b return_dispatcher

initialize:
    movs r0, #0
    movs r1, #0
    movs r2, #0
clear_mailbox:
    str r0, [r4, r1]
    adds r1, #4
    adds r2, #1
    cmp r2, #0x40              /* clear first 256 bytes; payload is overwritten */
    blo clear_mailbox
    ldr r0, =MAILBOX_MAGIC
    str r0, [r4, #MAILBOX_OFF_MAGIC]
    movs r0, #MAILBOX_VERSION
    strh r0, [r4, #MAILBOX_OFF_VERSION]
    movs r0, #0x80
    strh r0, [r4, #MAILBOX_OFF_HEADER_SIZE]
    ldr r0, =G_RNG_SEED
    ldr r0, [r0]
    ldr r1, =G_DAY
    ldrh r1, [r1]
    eors r0, r1
    ldr r1, =0xA2571D3B
    eors r0, r1
    cmp r0, #0
    bne session_ready
    movs r0, #1
session_ready:
    str r0, [r4, #MAILBOX_OFF_SESSION_ID]
    b dispatch_state

execution_complete:
    movs r0, #MAILBOX_STATE_IDLE
    str r0, [r4, #MAILBOX_OFF_STATE]
    b return_dispatcher

publish_request:
    ldr r5, =MAILBOX_BASE + MAILBOX_OFF_REQUEST
    movs r0, #1
    strh r0, [r5, #REQUEST_SNAPSHOT_VERSION]
    ldr r6, =MAP_BASE
    ldrh r0, [r6]
    strb r0, [r5, #REQUEST_WIDTH]
    ldrh r0, [r6, #2]
    strb r0, [r5, #REQUEST_HEIGHT]
    ldr r0, =G_MODE
    ldrb r1, [r0, #1]
    strb r1, [r5, #REQUEST_MODE]
    ldrb r1, [r0, #2]
    strb r1, [r5, #REQUEST_MAP_ID]
    ldr r0, =G_ACTIVE_SIDE
    ldrh r0, [r0]
    strb r0, [r5, #REQUEST_ACTIVE_SIDE]
    ldr r0, =G_AI_PHASE
    ldrh r0, [r0]
    strb r0, [r5, #REQUEST_AI_PHASE]
    ldr r0, =G_DAY
    ldrh r0, [r0]
    strh r0, [r5, #REQUEST_DAY]
    ldr r0, =G_GLOBAL_STATE
    ldrh r0, [r0]
    strh r0, [r5, #REQUEST_GLOBAL_STATE]
    ldr r0, =G_EXECUTOR_STATE
    ldrh r0, [r0]
    strh r0, [r5, #REQUEST_EXECUTOR_STATE]

    ldr r7, =PLAYER_RECORDS
    ldr r0, [r7, #0x3c]        /* army 1 / player 1 */
    str r0, [r5, #REQUEST_FUNDS_P1]
    ldr r0, [r7, #0x78]        /* army 2 / player 2 */
    str r0, [r5, #REQUEST_FUNDS_P2]

    /* Flatten Spann's unit and logical terrain/property planes row-major. */
    ldr r0, =MAILBOX_BASE + MAILBOX_OFF_REQUEST + REQUEST_UNIT_PLANE
    ldr r1, =MAP_BASE + 0x12
    bl copy_spann_plane
    ldr r0, =MAILBOX_BASE + MAILBOX_OFF_REQUEST + REQUEST_PROPERTY_PLANE
    ldr r1, =MAP_BASE + 0x1432
    bl copy_spann_plane

    /* Copy the two Spann armies' 128 native records verbatim. */
    ldr r0, =MAILBOX_BASE + MAILBOX_OFF_REQUEST + REQUEST_UNIT_RECORDS
    ldr r1, =UNIT_RECORDS
    movs r2, #0xC0             /* 0x600 bytes / 8 */
copy_records:
    ldmia r1!, {r3, r6}
    stmia r0!, {r3, r6}
    subs r2, #1
    bne copy_records

    ldr r0, [r4, #MAILBOX_OFF_REQUEST_ID]
    adds r0, #1
    str r0, [r4, #MAILBOX_OFF_REQUEST_ID]
    ldr r1, =G_RNG_SEED
    ldr r1, [r1]
    eors r1, r0
    ldr r2, =G_DAY
    ldrh r2, [r2]
    lsls r2, r2, #16
    eors r1, r2
    ldr r2, [r5, #REQUEST_FUNDS_P2]
    eors r1, r2
    str r1, [r5, #REQUEST_STATE_SEED]
    ldr r0, =SNAPSHOT_LENGTH
    str r0, [r4, #MAILBOX_OFF_REQUEST_LENGTH]
    movs r0, #0
    str r0, [r4, #MAILBOX_OFF_RESPONSE_LENGTH]
    str r0, [r4, #MAILBOX_OFF_ERROR_CODE]
    str r0, [r4, #MAILBOX_OFF_FRAME_COUNTER]
    movs r0, #MAILBOX_STATE_REQUEST_READY
    str r0, [r4, #MAILBOX_OFF_STATE] /* publish last */
    b return_dispatcher

consume_response:
    ldr r0, [r4, #MAILBOX_OFF_RESPONSE_SESSION_ID]
    ldr r1, [r4, #MAILBOX_OFF_SESSION_ID]
    cmp r0, r1
    beq 1f
    b stale_response
1:
    ldr r0, [r4, #MAILBOX_OFF_RESPONSE_REQUEST_ID]
    ldr r1, [r4, #MAILBOX_OFF_REQUEST_ID]
    cmp r0, r1
    beq 1f
    b stale_response
1:
    ldr r0, [r4, #MAILBOX_OFF_RESPONSE_LENGTH]
    cmp r0, #RESPONSE_MIN_LENGTH
    bhs 1f
    b invalid_response
1:
    ldr r5, =MAILBOX_BASE + MAILBOX_OFF_RESPONSE
    ldr r0, [r5, #RESPONSE_STATE_SEED]
    ldr r1, =MAILBOX_BASE + MAILBOX_OFF_REQUEST
    ldr r1, [r1, #REQUEST_STATE_SEED]
    cmp r0, r1
    beq 1f
    b stale_response
1:
    ldrb r0, [r5, #RESPONSE_ACTION]
    cmp r0, #MAILBOX_ACTION_BUILD
    beq execute_build
    cmp r0, #MAILBOX_ACTION_UNIT
    beq execute_unit
    cmp r0, #MAILBOX_ACTION_END_TURN
    bne 1f
    b execute_end_turn
1:
    b unsupported_action

execute_build:
    ldrb r0, [r5, #RESPONSE_BUILDING_X]
    cmp r0, #15
    blo 1f
    b invalid_response
1:
    ldrb r1, [r5, #RESPONSE_BUILDING_Y]
    cmp r1, #10
    blo 1f
    b invalid_response
1:
    ldrb r2, [r5, #RESPONSE_UNIT_TYPE]
    cmp r2, #0
    bne 1f
    b invalid_response
1:

    /* Require an empty army-2 base in the current authoritative map. */
    ldr r3, =MAP_BASE + 0x417A
    lsls r6, r1, #1
    ldrh r6, [r3, r6]
    adds r6, r6, r0
    ldr r3, =MAP_BASE + 0x12
    ldrb r7, [r3, r6]
    cmp r7, #0
    beq 1f
    b invalid_response
1:
    ldr r3, =MAP_BASE + 0x1432
    ldrb r3, [r3, r6]
    cmp r3, #0x4e              /* owner group 2 | base terrain 14 */
    beq 1f
    b invalid_response
1:

    ldr r3, =G_COMMAND
    movs r6, #0
    str r6, [r3, #0]
    str r6, [r3, #4]
    str r6, [r3, #8]
    str r6, [r3, #12]
    str r6, [r3, #16]
    movs r6, #1                /* stock phase-3 build command */
    strb r6, [r3, #0]
    strb r2, [r3, #1]
    strb r0, [r3, #2]
    strb r1, [r3, #3]
    ldr r6, =G_RNG_SEED
    ldr r6, [r6]
    str r6, [r3, #8]
    movs r6, #1
    strb r6, [r3, #0x13]
    movs r0, #MAILBOX_ACTION_BUILD
    str r0, [r4, #MAILBOX_OFF_LAST_ACTION]
    movs r0, #MAILBOX_STATE_EXECUTING
    str r0, [r4, #MAILBOX_OFF_STATE]
    ldr r1, =G_EXECUTOR_STATE
    movs r0, #0
    strh r0, [r1]
    ldr r1, =G_AI_PHASE
    movs r0, #3
    strh r0, [r1]
    b return_dispatcher

execute_unit:
    ldrb r6, [r5, #RESPONSE_UNIT_ID]
    cmp r6, #64                 /* only Spann's enemy army */
    blo unit_invalid_near
    cmp r6, #128
    bhs unit_invalid_near
    b unit_id_valid
unit_invalid_near:
    b invalid_response
unit_id_valid:
    ldr r7, =UNIT_RECORDS
    lsls r0, r6, #3
    lsls r1, r6, #2
    adds r0, r0, r1
    adds r7, r7, r0
    ldrb r0, [r7]
    cmp r0, #0
    beq invalid_response
    ldrb r0, [r7, #1]
    movs r1, #1
    tst r0, r1
    bne invalid_response        /* already moved this turn */
    ldrb r0, [r5, #RESPONSE_DEST_X]
    cmp r0, #15
    bhs invalid_response
    ldrb r1, [r5, #RESPONSE_DEST_Y]
    cmp r1, #10
    bhs invalid_response
    ldrb r2, [r5, #RESPONSE_COMMAND]
    cmp r2, #2                  /* wait */
    beq unit_command_valid
    cmp r2, #3                  /* capture */
    beq unit_command_valid
    cmp r2, #4                  /* attack; param0 = target unit id */
    bne unsupported_action
    ldrb r0, [r5, #RESPONSE_PARAM0]
    cmp r0, #64                 /* target must be Spann's player-side army */
    bhs invalid_response
    ldr r1, =UNIT_RECORDS
    lsls r2, r0, #3
    lsls r3, r0, #2
    adds r2, r2, r3
    adds r1, r1, r2
    ldrb r0, [r1]
    cmp r0, #0
    beq invalid_response
unit_command_valid:
    ldr r3, =G_CURRENT_UNIT_ID
    strb r6, [r3]
    ldr r3, =G_CURRENT_UNIT_PTR
    str r7, [r3]
    ldr r3, =G_SOURCE_POSITION
    ldrb r0, [r7, #2]
    strh r0, [r3]
    ldrb r0, [r7, #3]
    strh r0, [r3, #2]

    ldr r3, =G_COMMAND
    movs r0, #0
    str r0, [r3, #0]
    str r0, [r3, #4]
    str r0, [r3, #8]
    str r0, [r3, #12]
    str r0, [r3, #16]

    /* Build AW2's native movement map/cost, then preserve the model's exact
     * legal route. Reconstructing only from the destination can choose a
     * different equal-cost route and strand the stock movement executor. */
    adds r0, r7, #0
    bl call_prepare_movement_map
    ldrb r0, [r5, #RESPONSE_DEST_X]
    ldrb r1, [r5, #RESPONSE_DEST_Y]
    ldr r2, =G_PATH
    bl call_prepare_path
    ldrb r2, [r5, #RESPONSE_PATH_LENGTH]
    cmp r2, #1
    bhs 1f
    b invalid_response
1:
    cmp r2, #RESPONSE_PATH_CAPACITY
    bls 1f
    b invalid_response
1:
    movs r2, #0
unit_path_copy:
    ldr r0, =G_PATH
    ldr r1, =MAILBOX_BASE + MAILBOX_OFF_RESPONSE + RESPONSE_PATH
    ldrb r3, [r1, r2]
    strb r3, [r0, r2]
    adds r2, #1
    ldrb r1, [r5, #RESPONSE_PATH_LENGTH]
    cmp r2, r1
    beq unit_path_last
    cmp r3, #3
    bls unit_path_copy
    b invalid_response
unit_path_last:
    cmp r3, #4
    beq unit_path_valid
    b invalid_response
unit_path_valid:
    ldr r0, =G_PATH
    ldr r1, =G_PACKED_PATH
    bl call_pack_path

    ldr r3, =G_COMMAND
    strb r6, [r3, #1]
    ldrb r0, [r5, #RESPONSE_DEST_X]
    strb r0, [r3, #2]
    ldrb r0, [r5, #RESPONSE_DEST_Y]
    strb r0, [r3, #3]
    ldrb r0, [r7, #2]
    strb r0, [r3, #4]
    ldrb r0, [r7, #3]
    strb r0, [r3, #5]
    ldrb r0, [r5, #RESPONSE_COMMAND]
    strb r0, [r3, #0]
    ldrb r0, [r5, #RESPONSE_PARAM0]
    strb r0, [r3, #6]
    ldrb r0, [r5, #RESPONSE_PARAM1]
    strb r0, [r3, #7]
    ldr r0, =G_RNG_SEED
    ldr r0, [r0]
    str r0, [r3, #8]
    ldrb r0, [r7, #6]
    movs r1, #0x7f
    ands r0, r1
    ldr r1, =G_PATH_COST
    ldrb r1, [r1]
    subs r0, r0, r1
    strb r0, [r3, #0x12]
    movs r0, #1
    strb r0, [r3, #0x13]
    movs r0, #MAILBOX_ACTION_UNIT
    str r0, [r4, #MAILBOX_OFF_LAST_ACTION]
    movs r0, #MAILBOX_STATE_EXECUTING
    str r0, [r4, #MAILBOX_OFF_STATE]
    ldr r1, =G_EXECUTOR_STATE
    movs r0, #0
    strh r0, [r1]
    ldr r1, =G_AI_PHASE
    movs r0, #3
    strh r0, [r1]
    b return_dispatcher

execute_end_turn:
    movs r0, #MAILBOX_ACTION_END_TURN
    str r0, [r4, #MAILBOX_OFF_LAST_ACTION]
    movs r0, #MAILBOX_STATE_EXECUTING
    str r0, [r4, #MAILBOX_OFF_STATE]
    ldr r1, =G_AI_PHASE
    movs r0, #4
    strh r0, [r1]
    b return_dispatcher

stale_response:
    movs r0, #1
    b set_error
invalid_response:
    movs r0, #2
    b set_error
unsupported_action:
    movs r0, #3
set_error:
    str r0, [r4, #MAILBOX_OFF_ERROR_CODE]
    movs r0, #MAILBOX_STATE_ERROR
    str r0, [r4, #MAILBOX_OFF_STATE]
    b return_dispatcher

call_original_phase2:
    ldr r3, =ORIGINAL_PHASE2
    bx r3

call_prepare_movement_map:
    ldr r3, =0x080202A5
    bx r3

call_prepare_path:
    ldr r3, =0x0802042D
    bx r3

call_pack_path:
    ldr r3, =0x08034401
    bx r3

/* r0=destination, r1=plane base; preserves r4/r5. */
copy_spann_plane:
    push {r6, r7, lr}
    movs r2, #0
copy_plane_rows:
    lsls r3, r2, #1
    ldr r6, =MAP_BASE + 0x417A
    ldrh r3, [r6, r3]
    adds r3, r1, r3
    movs r7, #15
copy_plane_cells:
    ldrb r6, [r3]
    strb r6, [r0]
    adds r3, #1
    adds r0, #1
    subs r7, #1
    bne copy_plane_cells
    adds r2, #1
    cmp r2, #10
    blo copy_plane_rows
    pop {r6, r7, pc}

return_dispatcher:
    pop {r4-r7}
    ldr r3, =DISPATCH_EPILOGUE
    bx r3

.size spann_phase2_arm, . - spann_phase2_arm
