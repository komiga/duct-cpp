-- duct++ premake file
-- run `premake4 [--installroot=PATH] install` after building to install duct++

local name="duct"
local outpath="out/"
local binarypath=outpath..name

if _ACTION=="clean" then
	os.rmdir(outpath)
end

solution("duct")
	configurations {"debug", "release"}

local proj=project(name)
proj.language="C++"
proj.kind="SharedLib"

configuration {"debug"}
	targetsuffix("_debug")
	targetdir(outpath)
	objdir(outpath)
	defines {"DEBUG", "_DEBUG"}
	flags {"Symbols", "ExtraWarnings"}

configuration {"release"}
	targetdir(outpath)
	objdir(outpath)
	defines {"NDEBUG", "RELEASE"}
	flags {"Optimize", "ExtraWarnings"}

configuration {"gmake"}
	links {"icui18n", "icudata", "icuio", "icuuc"}

configuration {"linux"}
	defines {"PLATFORM_CHECKED", "UNIX_BUILD"}
	postbuildcommands {"mkdir -p lib/linux"}

configuration {"linux", "debug"}
	postbuildcommands {"cp "..outpath.."lib"..name.."_debug.so lib/linux/lib"..name.."_debug.so"}

configuration {"linux", "release"}
	postbuildcommands {"cp "..outpath.."lib"..name..".so lib/linux/lib"..name..".so"}

configuration {}

files {"include/duct/*.hpp", "src/duct/*.cpp"}
includedirs {
	"include/"
}

-- extensions

-- src: http://industriousone.com/topic/oscopydir
-- Allows copying directories.
-- It uses the premake4 patterns (**=recursive match, *=file match)
-- NOTE: It won't copy empty directories!
-- Example: we have a file: src/test.h
--	os.copydir("src", "include") simple copy, makes include/test.h
--	os.copydir("src", "include", "*.h") makes include/test.h
--	os.copydir(".", "include", "src/*.h") makes include/src/test.h
--	os.copydir(".", "include", "**.h") makes include/src/test.h
--	os.copydir(".", "include", "**.h", true) will force it to include dir, makes include/test.h
--
-- @param src_dir
--    Source directory, which will be copied to dst_dir.
-- @param dst_dir
--    Destination directory.
-- @param filter
--    Optional, defaults to "**". Only filter matches will be copied. It can contain **(recursive) and *(filename).
-- @param single_dst_dir
--    Optional, defaults to false. Allows putting all files to dst_dir without subdirectories.
--    Only useful with recursive (**) filter.
-- @returns
--    True if successful, otherwise nil.
--
function os.copydir(src_dir, dst_dir, filter, single_dst_dir)
	filter = filter or "**"
	src_dir = src_dir .. "/"
	--print('copy "' .. src_dir .. filter .. '" to "' .. dst_dir .. '".')
	dst_dir = dst_dir .. "/"
	local dir = path.rebase(".",path.getabsolute("."), src_dir) -- root dir, relative from src_dir
 
	os.chdir( src_dir ) -- change current directory to src_dir
		local matches = os.matchfiles(filter)
	os.chdir( dir ) -- change current directory back to root
 
	local counter = 0
	for k, v in ipairs(matches) do
		local target = iif(single_dst_dir, path.getname(v), v)
		--make sure, that directory exists or os.copyfile() fails
		os.mkdir( path.getdirectory(dst_dir .. target))
		if os.copyfile( src_dir .. v, dst_dir .. target) then
			counter = counter + 1
		end
	end
 
	if counter == #matches then
		--print( counter .. " files copied.")
		return true
	else
		--print( "Error: " .. counter .. "/" .. #matches .. " files copied.")
		return nil
	end
end

-- install action

newoption {
	trigger="installroot",
	value="PATH",
	description="Base install path (default is /usr/local)"
}

newoption {
	trigger="installdebug",
	description="Whether to install the debug build",
	value="true|false",
	allowed={
		{"true", "Install the debug build as well as the release build"},
		{"false", "Install only the release build (default)"}
	}
}

newaction {
	trigger="install",
	description="Install duct++",
	execute=function()
		print("installing duct++")
		local installroot=_OPTIONS.installroot
		if not installroot then
			installroot="/usr/local"
		end
		print("installroot="..installroot)
		local installdebug=nil
		if _OPTIONS.installdebug=="true" then
			installdebug=true
		end
		if not os.get()=="linux" then
			print("unimplemented for non-Linux OSes")
			return nil
		end
		local opsys=os.get()
		if opsys=="linux" then
			if not os.isdir("./lib/linux") then
				print("must run build.sh first")
				return nil
			end
			if not os.isdir(installroot) then
				os.mkdir(installroot)
			end
			os.rmdir(installroot.."/include/duct")
			if not os.copydir("include", installroot.."/include", "**.hpp") then
				printf("failed to copy includes")
				return nil
			end
			if not os.isdir(installroot.."/lib") then
				os.mkdir(installroot.."/lib")
			end
			os.execute("rm "..installroot.."/lib/libduct.so")
			if not os.copyfile("lib/linux/libduct.so", installroot.."/lib/libduct.so") then
				print("failed to copy release library")
				return nil
			end
			if installdebug then
				os.execute("rm "..installroot.."/lib/libduct_debug.so")
				if not os.copyfile("lib/linux/libduct_debug.so", installroot.."/lib/libduct_debug.so") then
					print("failed to copy debug library")
					return nil
				end
			end
		end
		return true
	end
}

