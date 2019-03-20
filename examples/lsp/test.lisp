(load "/home/icepic/Code/build_eclp/examples/module.fas")

(defparameter *img* (image))
(image-set-height *img* 10000)
(image-set-width *img* 200)
(image-get-height *img*)
(image-get-width *img*)



;; :char - Signed 8-bits. A dereferenced :char pointer returns an character.
;; :unsigned-char - Unsigned 8-bits. A dereferenced :unsigned-char pointer returns an character.
;; :byte - Signed 8-bits. A dereferenced :byte pointer returns an integer.
;; :unsigned-byte - Unsigned 8-bits. A dereferenced :unsigned-byte pointer returns an integer.
;; :short - Signed 16-bits.
;; :unsigned-short - Unsigned 16-bits.
;; :int - Signed 32-bits.
;; :unsigned-int - Unsigned 32-bits.
;; :long - Signed 32 or 64 bits, depending upon the platform.
;; :unsigned-long - Unsigned 32 or 64 bits, depending upon the platform.
;; :float - 32-bit floating point.
;; :double - 64-bit floating point.
;; :cstring - A NULL terminated string used for passing and returning characters strings with a C function.
;; :void - The absence of a value. Used to indicate that a function does not return a value.
;; :pointer-void - Points to a generic object.
;; * - Used to declare a pointer to an object
;; uint8-t Unsigned 8-bits.
;; int8-t Signed 8-bits
;; uint16-t Unsigned 16-bits.
;; int8-t Signed 16-bits
;; uint32-t Unsigned 32-bits.
;; int32-t Signed 32-bits
;; uint64-t Unsigned 32-bits.
;; int64-t Signed 32-bits

;; Create array via foreign function interface
(defparameter *darray* (ffi:allocate-foreign-object :double 10))
;; Create pointer type for array
(defparameter *ptr* (ffi::def-array-pointer *darray* :double))
;; Derefence array
(ffi:deref-array *darray* *ptr* 9) 
;; Set value
(setf (ffi:deref-array *darray* *ptr* 9) 10)
;; Free memory
(ffi:free-foreign-object *darray*)


;; Define struct
(ffi:def-struct custom-struct
    (:x :uint32-t) (:y :uint32-t) (:d :double))

;; Allocate memory
(defparameter *struct* (ffi::allocate-foreign-object 'custom-struct 10))

(defun deref (baseptr type pos)
  ; Deref base ptr `baseptr` of type `type` at position `pos`.
  (let ((bytes (ffi:size-of-foreign-type type))
        (ptr (ffi:pointer-address baseptr)))
    (ffi:make-pointer ( + (* bytes pos) ptr) type)))


(defun initialize-struct-array (xi yi di struct size)
  (dotimes (i size)
    (setf (ffi:get-slot-value (deref struct 'custom-struct i) 'custom-struct :x)xi)
    (setf (ffi:get-slot-value (deref struct 'custom-struct i) 'custom-struct :y)yi)
    (setf (ffi:get-slot-value (deref struct 'custom-struct i) 'custom-struct :d)di))) 


(defun struct-array-to-list (struct size)
  (let ((result nil))
    (dotimes (i size)
      (push `(,(ffi:get-slot-value (deref struct 'custom-struct i) 'custom-struct :x)
               ,(ffi:get-slot-value (deref struct 'custom-struct i) 'custom-struct :y)
               ,(ffi:get-slot-value (deref struct 'custom-struct i) 'custom-struct :d)) result)) result))

(initialize-struct-array 100 200 300 *struct* 10)
(struct-array-to-list *struct* 10)

(test-print-paramater 10 *struct*)
(test-initialize-parameter 10 *struct* 1 2 3.0)




