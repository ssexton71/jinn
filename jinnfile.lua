
package.path = "./lua/?.lua;" .. package.path

_ARGS = {} 
_OPTIONS = {}
_OS = arg[1]

require "compat"
require "jinn"

require "premake4"

jinn.outfile:close()


