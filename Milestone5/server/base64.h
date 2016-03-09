/*
Arielle Chongco 45137728
Jasmine Hunter  51994784
Akihiro Izumi   69616804
Jaysen Gan	25399148
*/

#ifndef BASE64_H
#define BASE64_H

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

#endif /* BASE64_H */
