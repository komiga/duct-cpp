-- duct++ tests premake file

--[[if _ACTION=="clean" then
	os.rmdir(outpath)
end]]

newoption {
	trigger="clang",
	description="Use Clang in-place of GCC",
}

if _OPTIONS["clang"] then
	premake.gcc.cc="clang"
	premake.gcc.cxx="clang++"
end

solution("tests")
	configurations {"debug", "release"}
	platforms {"x32", "x64"}

group=""

function setup_test(name, src)
	local outpath="out/"
	local proj=project(group.."_"..name)
	proj.language="C++"
	proj.kind="ConsoleApp"
	
	targetname(name)
	
	configuration {"debug"}
		defines {"DEBUG", "_DEBUG"}
		flags {"ExtraWarnings", "Symbols"}
	
	configuration {"release"}
		defines {"NDEBUG"}
		flags {"ExtraWarnings", "Optimize"}
	
	configuration {"linux"}
		buildoptions {
			--"-Wextra",
			"-Wunused-parameter",
			"-Wuninitialized",
			"-Wmissing-field-initializers",
			"-Wredundant-decls",
			"-Wfloat-equal"
		}
		buildoptions {"-std=c++0x", "-pedantic"}
	
	configuration {"linux" and "clang"}
		buildoptions {"-stdlib=libstdc++"}

	configuration {"clang"}
		links {
			"stdc++"
		}

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

if _ACTION=="clean" then
	local prjs=solution().projects
	for i, prj in ipairs(prjs) do
		os.rmdir(prj.basedir.."/out")
	end
end

