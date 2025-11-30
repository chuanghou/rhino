#pragma once

// Version namespace definition
// Use inline namespace to allow versioning while maintaining backward compatibility
// Users can access symbols via rhino::ClassName or rhino::v1::ClassName

#ifndef RHINO_VERSION_NAMESPACE
#define RHINO_VERSION_NAMESPACE v1
#endif

#define RHINO_INLINE_NAMESPACE_BEGIN inline namespace RHINO_VERSION_NAMESPACE {
#define RHINO_INLINE_NAMESPACE_END }

