import win32con
import sys


def GenerateCppWin32VKMapTo(filename_prefix, namespace):
	keys = {getattr(win32con, v) : v for v in dir(win32con) if v.startswith("VK_")}

	# First generate the header file (declarations)
	declBase = "\textern const Keycode KEYBOARD_{};\n"
	allKeycodesStr = ""
	
	# First start with all alphanumeric keys
	for alphanumkey in '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ':
		keycodeDecl = declBase.format(alphanumkey)
		allKeycodesStr += keycodeDecl
	
	# Now process all VK_ ones
	for (keycodeValue, keycodeName) in keys.items():
		keycodeDecl = declBase.format(keycodeName.replace("VK_", ""))
		allKeycodesStr += keycodeDecl

	# Write the header file	
	fileContents = """

#pragma once

#include <cstdint>

namespace {}
{{
	typedef std::uint8_t	Keycode;

{}}}
""".format(namespace, allKeycodesStr)

	with open(filename_prefix + ".h", 'w') as mapheader:
		mapheader.write(fileContents)
		print("Wrote to {}".format(filename_prefix + ".h"))
		
	# Then generate the source, platform-specific file (definitions)
	declBase = "\tconst Keycode KEYBOARD_{} = {};\n"
	allKeycodesStr = ""

	# First start with all alphanumeric keys
	for alphanumkey in '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ':
		keycodeDecl = declBase.format(alphanumkey, hex(ord(alphanumkey)))
		allKeycodesStr += keycodeDecl
	
	# Now process all VK_ ones
	for (keycodeValue, keycodeName) in keys.items():
		keycodeDecl = declBase.format(keycodeName.replace("VK_", ""), keycodeName)
		allKeycodesStr += keycodeDecl
	
	
	fileContents = """
#include <Windows.h>
#include "{}.h"
	
namespace {}
{{
{}}}
""".format(filename_prefix, namespace, allKeycodesStr)
	with open(filename_prefix + ".cpp", 'w') as mapfile:
		mapfile.write(fileContents)
		print("Wrote to {}".format(filename_prefix + ".cpp"))
		

if __name__ == '__main__':
	filename = "MonocleKeyboardMap"
	namespace = "moe"
	if len(sys.argv) > 1:
		filename = sys.argv[1]
	if len(sys.argv) > 2:
		namespace = sys.argv[2]
	GenerateCppWin32VKMapTo(filename, namespace)