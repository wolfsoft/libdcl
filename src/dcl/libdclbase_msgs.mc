MessageIdTypedef=DWORD

SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
    Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
    Warning=0x2:STATUS_SEVERITY_WARNING
    Error=0x3:STATUS_SEVERITY_ERROR
    )

FacilityNames=(System=0x0:FACILITY_SYSTEM
    Runtime=0x2:FACILITY_RUNTIME
    Stubs=0x3:FACILITY_STUBS
    Io=0x4:FACILITY_IO_ERROR_CODE
)

LanguageNames=(System=0x0400:libdclbase_msgs)

; The following are message definitions.

MessageId=0x1
Severity=Informational
Facility=Runtime
SymbolicName=MSG_INFO
Language=System
%1
.

MessageId=0x2
Severity=Warning
Facility=Runtime
SymbolicName=MSG_WARNING
Language=System
%1
.

MessageId=0x3
Severity=Error
Facility=Runtime
SymbolicName=MSG_ERROR
Language=System
%1
.
