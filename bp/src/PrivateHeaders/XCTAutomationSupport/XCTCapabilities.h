//
//     Generated by class-dump 3.5 (64 bit) (Debug version compiled May 11 2021 09:30:43).
//
//  Copyright (C) 1997-2019 Steve Nygard.
//

#import <objc/NSObject.h>

@import Foundation;

@class NSDictionary;

@interface XCTCapabilities : NSObject <NSSecureCoding>
{
    NSDictionary *_capabilitiesDictionary;
}

+ (id)emptyCapabilities;
+ (_Bool)supportsSecureCoding;

@property(readonly, copy) NSDictionary *capabilitiesDictionary; // @synthesize capabilitiesDictionary=_capabilitiesDictionary;
- (_Bool)hasCapability:(id)arg1;
- (unsigned long long)versionForCapability:(id)arg1;
- (unsigned long long)hash;
- (_Bool)isEqual:(id)arg1;
- (id)description;
- (void)encodeWithCoder:(id)arg1;
- (id)initWithCoder:(id)arg1;
@property(readonly, copy) NSDictionary *dictionaryRepresentation;
- (id)initWithDictionary:(id)arg1;

@end

