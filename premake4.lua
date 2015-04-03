
dofile("precore_import.lua")

local S, G, P = precore.helpers()

precore.init(
	nil,
	{
		"precore.clang-opts",
		"precore.c++11-core",
		"precore.env-common",
	}
)

precore.import(".")
precore.import("test")

precore.action_clean("out")
