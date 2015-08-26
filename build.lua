
local S, G, R = precore.helpers()

precore.make_config_scoped("duct.env", {
	once = true,
}, {
{global = function()
	precore.define_group("DUCT", os.getcwd())
end}})

precore.make_config("duct.strict", nil, {
{project = function()
	configuration {}
		flags {
			"FatalWarnings"
		}

	configuration {"linux"}
		buildoptions {
			"-pedantic",
			"-pedantic-errors",
			"-Wextra",

			"-Wuninitialized",
			"-Winit-self",

			"-Wmissing-field-initializers",
			"-Wredundant-decls",

			"-Wfloat-equal",
			"-Wold-style-cast",

			"-Wnon-virtual-dtor",
			"-Woverloaded-virtual",

			"-Wunused",
			"-Wundef",
		}
end}})

precore.make_config("duct.dep", nil, {
{project = function()
	configuration {}
		includedirs {
			G"${DUCT_ROOT}/",
		}
end}})

precore.apply_global({
	"precore.env-common",
	"duct.env",
})
