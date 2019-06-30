(ffi:load-foreign-library  "/home/icepic/Code/build_clbind/libtestPackage.so")

(ffi:def-function ( (si:coerce-to-base-string "registerPackage") register-package) ((name :cstring) (pack :pointer)) :module "/home/icepic/Code/build_clbind/libtestPackage.so" :returning :bool)

(defvar *lib* (si:load-foreign-module "/home/icepic/Code/build_clbind/libtestPackage.so" ))
(si:unload-foreign-module *lib*)

(si:find-foreign-symbol "add" "/home/icepic/Code/build_clbind/libtestPackage.so" :pointer-void 0)


(si:find-foreign-symbol (si:coerce-to-base-string "add2") "/home/icepic/Code/build_clbind/libtestPackage.so" :pointer-void 0)

(si:find-foreign-symbol (si:coerce-to-base-string "registerPackage") "/home/icepic/Code/build_clbind/libtestPackage.so" :pointer-void 0)

(si::make-foreign-data-from-array "dan")


