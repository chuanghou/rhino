#pragma once

#include "boost/date_time/c_local_time_adjustor.hpp"
#include "boost/date_time/local_time_adjustor.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/optional/optional.hpp"
#include <iostream>

using namespace boost::posix_time;
using namespace boost::gregorian;

namespace rhino {
boost::optional<int64_t>
local_time_to_unix_timestamp(const std::string &val,
                             const std::string &format = "%Y-%m-%d %H:%M:%S") {

    std::istringstream iss(val);
    iss.imbue(
        std::locale(std::locale::classic(), new time_input_facet(format)));

    ptime local_time;
    iss >> local_time;

    if (local_time == not_a_date_time) {
        return boost::none;
    }

    typedef boost::date_time::c_local_adjustor<ptime> local_adj;
    const ptime epoch(date(1970, 1, 1));
    return (local_time - local_adj::utc_to_local(epoch)).total_seconds();
}

ptime timestamp_to_local_ptime(const int64_t &timestamp) {
    boost::gregorian::date epoch(1970, 1, 1);
    time_duration duration = seconds(timestamp);
    typedef boost::date_time::c_local_adjustor<ptime> local_adj;
    return local_adj::utc_to_local(ptime{epoch, duration});
}

std::string ptime_to_string(const ptime &pt,
                            const std::string &format = "%Y-%m-%d %H:%M:%S") {
    std::stringstream ss;
    ss.imbue(std::locale(std::locale(), new time_input_facet(format)));
    ss << pt;
    return ss.str();
}

} // namespace rhino