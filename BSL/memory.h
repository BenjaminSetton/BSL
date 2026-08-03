#pragma once

#define BIT(x) 1 << x

// Memory size helpers
static constexpr u64 KB(u64 count) noexcept { return count * 1024; }
static constexpr u64 MB(u64 count) noexcept { return KB(count) * 1024; }
static constexpr u64 GB(u64 count) noexcept { return MB(count) * 1024; }

// TODO