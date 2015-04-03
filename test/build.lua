
local S, G, P = precore.helpers()

function make_test(group, name, srcglob, configs)
	configs = configs or {}
	table.insert(configs, 1, "duct.strict")
	table.insert(configs, 2, "duct.dep")

	precore.make_project(
		group .. "_" .. name,
		"C++", "ConsoleApp",
		"./", "out/",
		nil, configs
	)
	if not srcglob then
		srcglob = name .. ".cpp"
	end

	configuration {"linux"}
		targetsuffix(".elf")

	configuration {}
		targetname(name)
		files {
			srcglob
		}
end

function make_tests(group, tests)
	for name, test in pairs(tests) do
		make_test(group, name, test[0], test[1])
	end
end

precore.make_solution(
	"test",
	{"debug", "release"},
	{"x64", "x32"},
	nil,
	{
		"precore.generic",
	}
)

precore.import("args")
precore.import("general")
precore.import("gr")
precore.import("io")
precore.import("text")
precore.import("utils")
precore.import("var")
