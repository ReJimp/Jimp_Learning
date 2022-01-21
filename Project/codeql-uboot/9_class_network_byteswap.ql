import cpp

class NetworkByteSwap extends Expr { 
    NetworkByteSwap() {
        exists( MacroInvocation macroinvo | macroinvo.getMacro().getName() in ["ntohl", "ntohll", "ntohs"] | this =  macroinvo.getExpr())
    }
}

from NetworkByteSwap n
select n, "Network byte swap"