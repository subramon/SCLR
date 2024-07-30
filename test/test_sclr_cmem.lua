require 'Q/UTILS/lua/strict'
local qc     = require 'Q/UTILS/lua/qcore'
local cmem   = require 'libcmem'
local ffi    = require 'ffi' 
local Scalar = require 'libsclr' 
local tests = {}
local get_ptr = require 'Q/UTILS/lua/get_ptr'
-- scalar struct has been cdef'd in qc
--=========================================

tests.t1 = function()
  local num_iters = 1000000
  for i = 1, num_iters do 
    local s1 = assert(Scalar.new(i, "I4"))
    local c1 = s1:to_cmem()
    assert(type(c1) == "CMEM")
    local ptr = get_ptr(c1, "I4")
    assert(ptr[0] == i)
    ptr[0] = i+1
    assert(ptr[0] == i+1)
  end
  print("test 1 passed")
end
--================
-- return tests
tests.t1()
