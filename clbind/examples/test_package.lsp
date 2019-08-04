;; Patch for def-lib-functions for linux
(in-package "FFI")
(defmacro def-lib-function (name args &key returning module (call :default))
  (multiple-value-bind (c-name lisp-name) (lisp-to-c-name name)
    (let* ((return-type (ffi::%convert-to-return-type returning))
           (return-required (not (eq return-type :void)))
           (argtypes (mapcar #'(lambda (a) (ffi::%convert-to-arg-type (second a))) args)))
      `(let ((c-fun (si::find-foreign-symbol (si:coerce-to-base-string ',c-name) ,module :pointer-void 0)))
         (defun ,lisp-name ,(mapcar #'first args)
           (si::call-cfun c-fun ',return-type ',argtypes (list ,@(mapcar #'first args)) ,call))))))
(in-package "CL-USER")

;; Load functions from this module
(defvar *module-name* "/home/icepic/Code/build_clbind/libtestPackage.so")
(defvar *module* (si:load-foreign-module *module-name*))
;; (si:unload-foreign-module *module*)
;; Dynamically load foreign library (not needed if dynamic version of def-function is used)

(defun add-package (pack-name func-name)
  (let ((curr-pack (package-name *package*)))
    (unwind-protect
         (progn
           (make-package pack-name)
           (eval `(in-package ,pack-name))
           (register-package pack-name (si:find-foreign-symbol func-name *module-name* :pointer-void 0)))
      (eval `(in-package ,curr-pack)))))

(delete-package "MY-TEST")
(add-package "MY-TEST" "test")
;; (ffi:load-foreign-library  "/home/icepic/Code/build_clbind/libtestPackage.so")


;; Define registerPackage function
(ffi:def-function ( "registerPackage" register-package) ((name :cstring) (pack :pointer)) :module *module-name* :returning :int )
(ffi:def-function ( "strparam" str-param) ((name :cstring) ) :module *module-name* :returning :void )
(ffi:def-function ( "strreturn" str-return) () :module *module-name* :returning :cstring )
(ffi:def-function ( "add2" add ) ((a :int) (b :int)) :module *module-name* :returning :int)

;; Example how to load and call a function
(defvar *add2-cfunc* (foreign-symbol-pointer "add2"))
(si:call-cfun *add2-cfunc* :int (list :int :int) (list 10 20) :default)

(defvar *str-param-cfunc* (foreign-symbol-pointer "strparam"))
(defvar *str-return-cfunc* (foreign-symbol-pointer "strreturn"))

(ffi:convert-to-foreign-string "df")


(defvar b (coerce "bla" 'cstring))
(si:call-cfun *str-param-cfunc* :void  (list :cstring) (list "bdfdfla") :default)





(defvar bla (si:call-cfun *str-return-cfunc* :cstring () () :default))

(ffi:convert-to-cstring "bla")

(test2 (coerce  "bla" 'base-string))
(add 10 20)


(register-package (coerce "TEST" 'base-string) (foreign-symbol-pointer  "test"))



(foreign-symbol-pointer  "test")

(si:find-foreign-symbol (coerce "test" 'base-string) "/home/icepic/Code/build_clbind/libtestPackage.so" :pointer-void 0)



(ffi:def-function `( ,(si:coerce-to-base-string "registerPackage") register-package) ((name :cstring) (pack :pointer)) :module "/home/icepic/Code/build_clbind/libtestPackage.so" :returning :bool)

`(ffi:def-function (,(si:coerce-to-base-string "add2") add2 ) ((a :int) (b :pointer)) :module "/home/icepic/Code/build_clbind/libtestPackage.so" :returning :int)

(ffi:def-function `(,(concatenate 'string "bla" "dfdf") add2 ) ((a :int) (b :pointer)) :module "/home/icepic/Code/build_clbind/libtestPackage.so" :returning :int)

(type "bl")

(type-of "bla")

(si:base-string-p (coerce "string" 'base-string))


(si::make-foreign-data-from-array "dan")

(si:coerce-to-base-string "registerPackage")

(defvar *lib* (si:load-foreign-module "/home/icepic/Code/build_clbind/libtestPackage.so"))
(si:unload-foreign-module *lib*)

(si:find-foreign-symbol "add" "/home/icepic/Code/build_clbind/libtestPackage.so" :pointer-void 0)


(si:find-foreign-symbol "add2" "/home/icepic/Code/build_clbind/libtestPackage.so" :pointer-void 0)

(si:find-foreign-symbol (si:coerce-to-base-string "registerPackage") "/home/icepic/Code/build_clbind/libtestPackage.so" :pointer-void 0)

(si::make-foreign-data-from-array "dan")

(concatenate 'string "bla" "dfdf")

(defun test (x)
  (= x x)
  (+ x x))


