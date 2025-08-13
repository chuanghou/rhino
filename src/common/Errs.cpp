

#include "Errs.h"

namespace rhino {

EGrp EGrp::INTERNAL = EGrp(-1, "INTERNAL");

Err Err::SYS_ERR = Err(-1, "SYS_ERR", "system encounter some not know exception, plese retry later");

Err Err::UNREACHABLE = Err(-2, "UNREACHABLE", "it should have not happen!");

Err Err::FORMAT_ERR = Err(1, "FORMAT_ERR", "format wrong, please check input information");

Err Err::QUEUE_FULL = Err(2, "QUEUE_FULL", "queue is full, please try later");

} // namespace rhino