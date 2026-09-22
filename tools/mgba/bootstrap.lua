-- Diagnostic-safe entry point for mGBA's --script option.
-- The regular bridge stays reloadable; startup failures are written to the
-- repository-local runtime directory where the host launcher can report them.

local root = os.getenv("AW2_AGENT_ROOT") or os.getenv("AW2_SPANN_ROOT") or "X:/dev/aw2bhr-spann-local"
local log_path = root .. "/runtime/mgba-script.log"

local function append(message)
    local handle = assert(io.open(log_path, "ab"))
    handle:write(os.date("!%Y-%m-%dT%H:%M:%SZ ") .. tostring(message) .. "\n")
    handle:flush()
    handle:close()
end

append("bootstrap start")
local ok, result = xpcall(function()
    return dofile(root .. "/tools/mgba/spann_bridge.lua")
end, debug.traceback)
if ok then
    append("bridge loaded")
else
    append("bridge failed: " .. tostring(result))
    error(result)
end
