
function import_precore()
	local env_name = "PRECORE_ROOT"
	local root_path = os.getenv(env_name)
	if nil == root_path or 0 == #root_path then
		error(
			"Environment variable '" .. env_name .. "' is not " ..
			"defined or is blank; assign it to precore's root directory"
		)
	end
	dofile(path.join(root_path, "precore.lua"))
end

import_precore()
