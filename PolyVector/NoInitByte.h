#pragma once

struct NoInitByte {
    unsigned char value;

    NoInitByte() {
        static_assert(sizeof(NoInitByte) == sizeof(value), "invalid size");
        static_assert(__alignof(NoInitByte) == __alignof(value), "invalid alignment");
    }
};
