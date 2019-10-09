#pragma once

#include <gsl/gsl>

uint32_t execute_sync(std::initializer_list<const char *> argv) noexcept;
uint32_t execute_sync(std::initializer_list<const char *> argv,
                      uint64_t hout) noexcept;
uint32_t execute_sync(std::initializer_list<const char *> argv, //
                      uint64_t hin, uint64_t hout) noexcept;
