-- mGBA Lua bridge for the local Spann mailbox.
-- Load manually with Tools -> Scripting -> File -> Load Script.

local root = os.getenv("AW2_SPANN_ROOT") or "X:/dev/aw2bhr-spann-local"
dofile(root .. "/tools/mgba/mailbox_layout.lua")
local ipc = os.getenv("AW2_SPANN_IPC") or (root .. "/runtime")
local FILE_KIND_REQUEST = 1
local FILE_KIND_RESPONSE = 2
local FILE_HEADER_SIZE = 20
local runtime_log_path = ipc .. "/mgba-runtime.log"
local last_runtime_signature = nil
local runtime_frames = 0
local capture_runtime = os.getenv("AW2_SPANN_TEST_CAPTURE") == "1"
local test_end_turn = os.getenv("AW2_SPANN_TEST_ENDTURN") == "1"
local trace_runtime = os.getenv("AW2_SPANN_TRACE") == "1" or test_end_turn
local test_turn_day = nil
local test_turn_started = nil
local lua_session_ready = false
local test_key_schedule = { [0] = 1, [60] = 0, [210] = 7, [240] = 7, [270] = 7, [300] = 7, [330] = 0 }

math.randomseed(os.time() + math.floor(os.clock() * 1000000))

local function append_runtime(message)
    local handle = assert(io.open(runtime_log_path, "ab"))
    handle:write(os.date("!%Y-%m-%dT%H:%M:%SZ ") .. message .. "\n")
    handle:flush()
    handle:close()
end

local function le16(value)
    return string.char(value % 256, math.floor(value / 256) % 256)
end

local function le32(value)
    return string.char(
        value % 256,
        math.floor(value / 256) % 256,
        math.floor(value / 65536) % 256,
        math.floor(value / 16777216) % 256
    )
end

local function read_le16(data, offset)
    local a, b = string.byte(data, offset + 1, offset + 2)
    if not b then return nil end
    return a + b * 256
end

local function read_le32(data, offset)
    local a, b, c, d = string.byte(data, offset + 1, offset + 4)
    if not d then return nil end
    return a + b * 256 + c * 65536 + d * 16777216
end

local function mailbox32(field)
    return emu:read32(MAILBOX_BASE + MAILBOX_FIELDS[field])
end

local function set_mailbox32(field, value)
    emu:write32(MAILBOX_BASE + MAILBOX_FIELDS[field], value)
end

local function memory_bytes(address, length)
    local chunks = {}
    for i = 0, length - 1 do
        chunks[#chunks + 1] = string.char(emu:read8(address + i))
    end
    return table.concat(chunks)
end

local function write_memory(address, data)
    for i = 1, #data do
        emu:write8(address + i - 1, string.byte(data, i))
    end
end

local function envelope(kind, session, request, payload)
    return le32(MAILBOX_MAGIC)
        .. le16(MAILBOX_VERSION)
        .. le16(kind)
        .. le32(session)
        .. le32(request)
        .. le32(#payload)
        .. payload
end

local function atomic_write(path, data)
    local temporary = path .. ".tmp"
    local handle = assert(io.open(temporary, "wb"))
    handle:write(data)
    handle:flush()
    handle:close()
    os.remove(path)
    assert(os.rename(temporary, path))
end

local function read_file(path)
    local handle = io.open(path, "rb")
    if not handle then return nil end
    local data = handle:read("*a")
    handle:close()
    return data
end

local function request_path(session, request)
    return string.format("%s/request-%08x-%08x.bin", ipc, session, request)
end

local function response_path(session, request)
    return string.format("%s/response-%08x-%08x.bin", ipc, session, request)
end

local function publish_request()
    local length = mailbox32("request_length")
    if length > MAILBOX_REQUEST.capacity then
        set_mailbox32("error_code", 0x101)
        set_mailbox32("state", MAILBOX_STATES.ERROR)
        return
    end
    local session = mailbox32("session_id")
    local request = mailbox32("request_id")
    local payload = memory_bytes(MAILBOX_BASE + MAILBOX_FIELDS.request, length)
    atomic_write(request_path(session, request), envelope(FILE_KIND_REQUEST, session, request, payload))
    set_mailbox32("state", MAILBOX_STATES.WAITING)
    print(string.format("AW2 Spann request session=%08x id=%d", session, request))
end

local function consume_response()
    local session = mailbox32("session_id")
    local request = mailbox32("request_id")
    local path = response_path(session, request)
    local data = read_file(path)
    if not data then return end
    local magic = read_le32(data, 0)
    local version = read_le16(data, 4)
    local kind = read_le16(data, 6)
    local response_session = read_le32(data, 8)
    local response_request = read_le32(data, 12)
    local length = read_le32(data, 16)
    if magic ~= MAILBOX_MAGIC or version ~= MAILBOX_VERSION or kind ~= FILE_KIND_RESPONSE
        or response_session ~= session or response_request ~= request
        or length == nil or length > MAILBOX_RESPONSE.capacity or #data ~= FILE_HEADER_SIZE + length then
        set_mailbox32("error_code", 0x102)
        set_mailbox32("state", MAILBOX_STATES.ERROR)
        os.remove(path)
        return
    end
    local payload = string.sub(data, FILE_HEADER_SIZE + 1)
    write_memory(MAILBOX_BASE + MAILBOX_FIELDS.response, payload)
    set_mailbox32("response_session_id", session)
    set_mailbox32("response_request_id", request)
    set_mailbox32("response_length", length)
    set_mailbox32("state", MAILBOX_STATES.RESPONSE_READY) -- publish last
    os.remove(path)
    print(string.format("AW2 Spann response session=%08x id=%d", session, request))
end

local function poll()
    runtime_frames = runtime_frames + 1
    if runtime_frames == 1 then
        append_runtime(string.format(
            "rom hook=%08x payload=%08x rom_size=%d",
            emu:read32(0x08061750), emu:read32(0x08800000), emu:romSize()
        ))
    end
    if test_end_turn then
        local global_state = emu:read8(0x030032D8)
        local side = emu:read8(0x030033EC)
        local day = emu:read8(0x03004080)
        if test_turn_started == nil and global_state == 0x0D and side == 1 and day ~= test_turn_day then
            test_turn_day = day
            test_turn_started = runtime_frames
            -- Keep the automated human test driver on a known empty Spann tile.
            emu:write16(0x030033E4, 4)
            emu:write16(0x030033E6, 6)
            append_runtime(string.format("test end-turn start day=%d frame=%d", day, runtime_frames))
        end
        local relative_frame = test_turn_started and (runtime_frames - test_turn_started) or -1
        local key = test_key_schedule[relative_frame]
        if key ~= nil then
            emu:addKey(key)
            append_runtime(string.format("test key day=%d relative=%d key=%d", test_turn_day, relative_frame, key))
        end
        local previous_key = test_key_schedule[relative_frame - 2]
        if previous_key ~= nil then
            emu:clearKey(previous_key)
        end
        if relative_frame > 360 then
            test_turn_started = nil
        end
    end
    local mailbox_magic = emu:read32(MAILBOX_BASE + MAILBOX_FIELDS.magic)
    local mailbox_state = emu:read32(MAILBOX_BASE + MAILBOX_FIELDS.state)
    local signature = string.format(
        "mode=%02x map=%02x global=%02x phase=%02x sub=%02x army=%02x day=%02x magic=%08x state=%08x",
        emu:read8(0x03003FC1), emu:read8(0x03003FC2), emu:read8(0x030032D8),
        emu:read8(0x03004780), emu:read8(0x030045D4), emu:read8(0x030033EC),
        emu:read8(0x03004080),
        mailbox_magic, mailbox_state
    )
    if trace_runtime and signature ~= last_runtime_signature then
        append_runtime(signature)
        last_runtime_signature = signature
    end
    if trace_runtime and runtime_frames % 300 == 0 then
        append_runtime(string.format("heartbeat frames=%d %s", runtime_frames, signature))
    end
    if capture_runtime and (runtime_frames == 120 or runtime_frames == 240 or runtime_frames == 480 or runtime_frames == 900) then
        local filename = string.format("mgba-frame-%d.png", runtime_frames)
        emu:screenshot(ipc .. "/" .. filename)
        append_runtime("captured " .. filename)
    end
    if mailbox_magic ~= MAILBOX_MAGIC then return end
    if emu:read16(MAILBOX_BASE + MAILBOX_FIELDS.version) ~= MAILBOX_VERSION then return end
    if not lua_session_ready then
        local session = math.random(0, 0xffff) * 0x10000 + math.random(0, 0xffff)
        if session == 0 then session = 1 end
        set_mailbox32("session_id", session)
        lua_session_ready = true
        append_runtime(string.format("fresh session=%08x", session))
    end
    local state = mailbox32("state")
    if state == MAILBOX_STATES.REQUEST_READY then
        publish_request()
    elseif state == MAILBOX_STATES.WAITING then
        consume_response()
    end
end

local function invalidate()
    lua_session_ready = false
    emu:write32(MAILBOX_BASE + MAILBOX_FIELDS.magic, 0)
end

invalidate()
callbacks:add("frame", poll)
callbacks:add("reset", invalidate)
callbacks:add("start", invalidate)
append_runtime("bridge callbacks installed")
print("AW2 Spann Lua bridge loaded; IPC=" .. ipc)
