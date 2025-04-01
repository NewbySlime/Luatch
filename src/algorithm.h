#ifndef ALGORITHM_HEADER
#define ALGORITHM_HEADER

#include "string"


std::string base64_encode(const void* buffer, size_t buflen);
// returns false if fail
bool base64_decode(const std::string& data, void* buffer, size_t buflen);
size_t base64_get_decode_length(const std::string& data); 

#endif