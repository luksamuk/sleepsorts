(ql:quickload :cl-async)

(defpackage :sleepsort
  (:use #:cl #:cl-async)
  (:export :sleepsort
           :run-test))

(in-package :sleepsort)

(defun sleepsort (&rest numbers)
  (start-event-loop
   (lambda ()
     (mapc (lambda (x)
             (delay (lambda () (format t "~a~&" x))
                    :time x))
           numbers))))

(defun run-test ()
  (apply #'sleepsort
         (loop repeat 15
            collect (random 30))))

