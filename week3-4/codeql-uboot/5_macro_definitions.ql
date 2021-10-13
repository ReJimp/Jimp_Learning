import cpp

from Macro macro
where macro.getName() in ["ntohl", "ntohll", "ntohs"]
select macro, "one of the three macros: ntohl, ntohll, ntohs"