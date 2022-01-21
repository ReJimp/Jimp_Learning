import cpp
import semmle.code.cpp.dataflow.TaintTracking
import DataFlow::PathGraph

class NetworkByteSwap extends Expr { 
    NetworkByteSwap() {
        exists( MacroInvocation macroinvo | macroinvo.getMacro().getName() in ["ntohl", "ntohll", "ntohs"] | this =  macroinvo.getExpr())
    }
}

class Config extends TaintTracking::Configuration {
    Config() { this = "NetworkToMemFuncLength"}

    override predicate isSource(DataFlow::Node source) {
        source.asExpr() instanceof NetworkByteSwap
    }
    override predicate isSink(DataFlow::Node sink) {
        exists( FunctionCall funcall
        | sink.asExpr() = funcall.getArgument(2) and
          funcall.getTarget().getName() = "memcpy")
    }
  
}

from Config cfg, DataFlow::PathNode source, DataFlow::PathNode sink
where cfg.hasFlowPath(source, sink)
select sink, source, sink, "Network byte swap flows to memcpy"