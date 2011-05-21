--proptool premake file

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
		targetdir(".")
		objdir(outpath)
		defines {"DEBUG", "_DEBUG"}
		flags {"Symbols", "ExtraWarnings"}
		links {"duct_debug"}
	
	configuration {"release"}
		targetdir(".")
		objdir(outpath)
		defines {"NDEBUG", "RELEASE"}
		flags {"Optimize", "ExtraWarnings"}
		links {"duct"}
	
	configuration {"linux", "x32"}
		libdirs {
			"../../lib/linux/x86/"
		}
	
	configuration {"linux", "x64"}
		libdirs {
			"../../lib/linux/x64/"
		}
	
	configuration {"vs2008"}
		includedirs {
			"../../deps/msvc/msinttypes/",
			"../../deps/include/icu/"
		}
		links {"icuin", "icudt", "icuio", "icuuc"}
	
	configuration {"vs2008", "x32"}
		libdirs {
			"../../lib/windows/x86/",
			"../../deps/msvc/x86/icu/lib/"
		}
	
	configuration {"vs2008", "x64"}
		libdirs {
			"../../lib/windows/x64/",
			"../../deps/msvc/x64/icu/lib/"
		}
	
	configuration {}
		includedirs {
			"../../include/",
			"/usr/local/include/"
		}
		files {
			src
		}
end

-- categories

include "arghandling"
include "charutils"
include "csv"
include "filesystem"
include "iniformatter"
include "scriptformatter"
include "streams"
include "variables"

if _ACTION=="clean" then
	local prjs=solution().projects
	for i, prj in ipairs(prjs) do
		os.rmdir(prj.basedir.."/out")
	end
end

