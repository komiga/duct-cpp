--duct++ tests premake file

--[[if _ACTION=="clean" then
	os.rmdir(outpath)
end]]

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
		flags {"Symbols", "ExtraWarnings"}
		links {"duct_debug"}
	
	configuration {"release"}
		defines {"NDEBUG"}
		flags {"Optimize", "ExtraWarnings"}
		links {"duct"}
	
	configuration {"linux", "x32"}
		libdirs {
			"../../lib/linux/x86"
		}
	
	configuration {"linux", "x64"}
		libdirs {
			"../../lib/linux/x64"
		}
	
	configuration {"linux"}
		buildoptions {"-std=c++0x", "-pedantic"}
	
	configuration {"windows", "x32"}
		libdirs {
			"../../lib/windows/x86"
		}
	
	configuration {"windows", "x64"}
		libdirs {
			"../../lib/windows/x64"
		}
	
	configuration {}
		targetdir(".")
		objdir(outpath)
		includedirs {
			"../../include"
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

