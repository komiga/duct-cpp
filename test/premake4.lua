
newoption {
	trigger = "clang",
	description = "Use Clang in-place of GCC",
}

if _OPTIONS["clang"] then
	premake.gcc.cc = "clang"
	premake.gcc.cxx = "clang++"
end

solution("tests")
	configurations {"debug", "release"}
	platforms {"x32", "x64"}

group = ""

function setup_test(name, src)
	local outpath = "out/"
	local proj = project(group .. "_" .. name)
	proj.language = "C++"
	proj.kind = "ConsoleApp"

	targetname(name)

	configuration {"debug"}
		defines {"DEBUG", "_DEBUG"}
		flags {"ExtraWarnings", "Symbols"}

	configuration {"release"}
		defines {"NDEBUG"}
		flags {"ExtraWarnings", "Optimize"}

	configuration {"linux"}
		buildoptions {"-std=c++11"}
		buildoptions {
			"-pedantic-errors",
			"-Werror",
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

	configuration {"linux", "not clang"}
		-- Avoid annoying error from C-style cast in <byteswap.h>.
		-- Somehow Clang doesn't even check -Wold-style-cast.
		defines {"DUCT_CONFIG_NO_BYTESWAP_HEADER"}

	configuration {"linux", "clang"}
		buildoptions {"-stdlib=libstdc++"}
		links {"stdc++"}

	configuration {}
		targetdir(".")
		objdir(outpath)
		includedirs {
			"../.."
		}
		files {
			src
		}
end

-- categories

include "general"
include "io"
include "text"
include "utils"
include "var"

if _ACTION == "clean" then
	local prjs = solution().projects
	for i, prj in ipairs(prjs) do
		os.rmdir(prj.basedir .. "/out")
	end
end
