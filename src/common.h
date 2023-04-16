#ifndef HASHMAP_OPTIMISED_COMMON_H
#define HASHMAP_OPTIMISED_COMMON_H

enum class result_t {
    ERROR = -1,
    OK    = 0
};

#define UNWRAP_ERROR(code) { if ((code) == result_t::ERROR) { return result_t::ERROR; } }

#define REPORTS_DIR "reports/"

#endif //HASHMAP_OPTIMISED_COMMON_H
