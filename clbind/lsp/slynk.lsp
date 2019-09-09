;;; -*- mode: lisp ; syntax: ansi-common-lisp -*-

;; standard quicklisp init file, since with be launching ecl without ~/.eclrc
(let ((quicklisp-init (merge-pathnames "quicklisp/ecl/setup.lisp"
                                       (user-homedir-pathname))))
  (when (probe-file quicklisp-init)
    (load quicklisp-init)))

(when (probe-file  "/tmp/slime.2565")
  (delete-file "/tmp/slime.2565"))

(load "~/quicklisp/ecl/dists/quicklisp/software/sly-20190813-git/slynk/slynk-loader.lisp" :verbose t)


(funcall (read-from-string "slynk-loader:init"))
(funcall (read-from-string "slynk:start-server")
         "/tmp/slime.2565")
