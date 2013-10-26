
dofile("precore_import.lua")

precore.make_config(
"test-strict", {{
project = function()
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

			"-Wunused"
		}

	configuration {"linux and not clang"}
		-- Avoid annoying error from C-style cast in <byteswap.h>.
		-- Somehow Clang doesn't even check -Wold-style-cast.
		defines {"DUCT_CONFIG_NO_BYTESWAP_HEADER"}
end}})

precore.init(
	{
		-- Don't have a top-level premake script, so just forcing
		-- project root to top-level
		ROOT = path.getabsolute("..")
	},
	{
		"opt-clang",
		"c++11-core",
		"precore-env-root"
	}
)

precore.make_solution(
	"tests",
	{"debug", "release"},
	{"x64", "x32"},
	nil,
	{
		"precore-generic"
	}
)

function make_test(group, name, srcglob, configs)
	precore.make_project(
		group .. "_" .. name,
		"C++", "ConsoleApp",
		"./", "out/",
		nil, configs
	)

	if nil == configs then
		precore.apply("test-strict")
	end

	if nil == srcglob then
		srcglob = name .. ".cpp"
	end

	configuration {}
		targetname(name)
		includedirs {
			precore.subst("${ROOT}")
		}
		files {
			srcglob
		}
end

function make_tests(group, tests)
	for name, test in pairs(tests) do
		make_test(group, name, test[0], test[1])
	end
end

-- categories

include("general")
include("gr")
include("io")
include("text")
include("utils")
include("var")

if "clean" == _ACTION then
	for _, pc_sol in pairs(precore.state.solutions) do
		for _, pc_proj in pairs(pc_sol.projects) do
			os.rmdir(path.join(pc_proj.obj.basedir, "out"))
		end
	end
end
