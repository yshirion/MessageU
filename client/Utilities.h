#pragma once
#include <cstdint>
#include "Enums.h"
#include "CryptoHelper.h"

constexpr auto UidSize = 16;
constexpr auto NameSize = 255;
constexpr auto VersionNum = 1;


#pragma pack(push, 1)
struct MessageRequestHeader
{
    std::array<uint8_t, UidSize> uid;
    MessageType type;
    uint32_t size;

};
#pragma pack(pop)

#pragma pack(push, 1)
struct MessageResponseHeader
{
    std::array<uint8_t, UidSize> uid;
    uint32_t mid;
    MessageType type;
    uint32_t size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct RequestHeader
{
    RequestHeader() : uid{ 0 }, version(VersionNum), code(RequestCode::Register), payload_size(0)
    {}
    RequestHeader(std::array<uint8_t, UidSize> id, RequestCode c, uint32_t pz) :
        uid(id), version(VersionNum), code(c), payload_size(pz) {}
    std::array<uint8_t, UidSize> uid;
    uint8_t version;
    RequestCode code;
    uint32_t payload_size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ResponseHeader
{
    uint8_t version;
    ResponseCode code;
    uint32_t payload_size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PublicKeyResponse
{
    std::array<uint8_t, UidSize> uid;
    std::array<uint8_t, CryptoHelper::Unsymmetric_Key_Length> key_value;
};
#pragma pack(pop)