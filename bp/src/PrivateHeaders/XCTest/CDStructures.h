//
//     Generated by class-dump 3.5 (64 bit) (Debug version compiled May 11 2021 09:30:43).
//
//  Copyright (C) 1997-2019 Steve Nygard.
//

#pragma mark Blocks

typedef void (^CDUnknownBlockType)(void); // return type and parameters are unknown

#pragma mark Named Structures

struct CGPoint {
    double x;
    double y;
};

struct CGRect {
    struct CGPoint origin;
    struct CGSize size;
};

struct CGSize {
    double width;
    double height;
};

struct CGVector {
    double dx;
    double dy;
};

struct _CSTypeRef {
    unsigned long long _opaque_1;
    unsigned long long _opaque_2;
};

struct _NSRange {
    unsigned long long _field1;
    unsigned long long _field2;
};

struct __va_list_tag {
    unsigned int _field1;
    unsigned int _field2;
    void *_field3;
    void *_field4;
};

struct atomic_flag {
    _Atomic _Bool _Value;
};

struct time_value {
    int seconds;
    int microseconds;
};

#pragma mark Typedef'd Structures

typedef struct {
    unsigned long long _field1;
    id *_field2;
    unsigned long long *_field3;
    unsigned long long _field4[5];
} CDStruct_70511ce9;

typedef struct {
    CDUnknownFunctionPointerType symbolicatorCreateWithTask;
    CDUnknownFunctionPointerType symbolicatorGetSymbolOwnerWithAddressAtTime;
    CDUnknownFunctionPointerType symbolOwnerGetName;
    CDUnknownFunctionPointerType symbolOwnerGetSymbolWithAddress;
    CDUnknownFunctionPointerType symbolOwnerGetSourceInfoWithAddress;
    CDUnknownFunctionPointerType symbolGetName;
    CDUnknownFunctionPointerType sourceInfoGetPath;
    CDUnknownFunctionPointerType sourceInfoGetLineNumber;
} CDStruct_7da0dd4a;

typedef struct {
    long long _field1;
    long long _field2;
    long long _field3;
} CDStruct_2ec95fd7;

