(ffi:load-foreign-library "@FFI_LIBRARY@")
(ffi:def-function ("init_lib" init) NIL :returning :void)

(init)    


