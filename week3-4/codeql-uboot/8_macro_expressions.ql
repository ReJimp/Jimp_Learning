import cpp

from MacroInvocation macroinvo
where 
    macroinvo.getMacro().getName() in ["ntohl", "ntohll", "ntohs"]
select macroinvo.getExpr()