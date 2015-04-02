
dofile("./precore_import.lua")

local S, G, P = precore.helpers()

precore.make_config("duct.test-strict", nil, {
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

precore.init(
	{
		-- Don't have a top-level premake script, so just forcing
		-- project root to top-level
		ROOT = path.getabsolute("..")
	},
	{
		"precore.clang-opts",
		"precore.c++11-core",
		"precore.env-common",
	}
)

precore.make_solution(
	"tests",
	{"debug", "release"},
	{"x64", "x32"},
	nil,
	{
		"precore.generic",
	}
)

function make_test(group, name, srcglob, configs)
	precore.make_project(
		group .. "_" .. name,
		"C++", "ConsoleApp",
		"./", "out/",
		nil, configs
	)

	if configs == nil then
		precore.apply("duct.test-strict")
	end

	if srcglob == nil then
		srcglob = name .. ".cpp"
	end

	configuration {}
		targetname(name)
		includedirs {
			S"${ROOT}"
		}
		files {
			srcglob
		}

	configuration {"linux"}
		targetsuffix(".elf")
end

function make_tests(group, tests)
	for name, test in pairs(tests) do
		make_test(group, name, test[0], test[1])
	end
end

-- categories

include("args")
include("general")
include("gr")
include("io")
include("text")
include("utils")
include("var")

precore.action_clean("out")
