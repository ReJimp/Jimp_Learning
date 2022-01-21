import cpp

from FunctionCall funcall
where funcall.getTarget().hasName("memcpy")
select funcall, "all calls to memcpy"