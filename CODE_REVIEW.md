# Comprehensive Code Review - SunVox Max External

**Date:** 2025-11-11
**Reviewer:** Claude Code
**Focus:** Audio quality issues and general code quality

---

## Critical Bugs Found

### 1. **CRITICAL: Wrong parameter in `sv_audio_callback2()` (sunvox_tilde.cpp:209)**

```cpp
sv_audio_callback2(x->out_sv_buffer, n, LATENCY, sv_get_ticks(), FLOAT32_TYPE, n, x->in_sv_buffer );
                                                                                  ^^
```

**The Bug:** The 6th parameter should be `in_channels` (number of input channels), but it's passing `n` (number of frames).

**API Signature:**
```cpp
int sv_audio_callback2(void* buf, int frames, int latency, uint32_t out_time,
                       int in_type, int in_channels, void* in_buf)
```

**Expected:** `N_IN_CHANNELS` (which is 2)
**Actual:** `n` (typically 64, the buffer size)

This causes SunVox to interpret the audio data completely incorrectly, treating 64 channels of input instead of 2, which scrambles the audio processing.

**Fix:**
```cpp
sv_audio_callback2(x->out_sv_buffer, n, LATENCY, sv_get_ticks(), FLOAT32_TYPE, N_IN_CHANNELS, x->in_sv_buffer);
```

---

### 2. **Memory Issue: Using `delete[]` on non-C++ allocated memory**

In `sv_free()` (sunvox_tilde.cpp:125-126):
```cpp
delete[] x->in_sv_buffer;
delete[] x->out_sv_buffer;
```

But these are allocated with `new[]` in `sv_dsp64()`, so this is actually correct. However, there's a potential issue: if `sv_free()` is called before `sv_dsp64()` runs, these pointers are NULL (initialized in `sv_new()` line 110-111), and while `delete[] nullptr` is safe in C++, it's worth noting.

---

### 3. **Missing `SV_INIT_FLAG_ONE_THREAD` flag**

Looking at the SunVox examples (test4.c:33, test5.c:34), when using `SV_INIT_FLAG_USER_AUDIO_CALLBACK`, they always include `SV_INIT_FLAG_ONE_THREAD`:

```cpp
int flags = SV_INIT_FLAG_USER_AUDIO_CALLBACK | SV_INIT_FLAG_ONE_THREAD;
```

Your code (sunvox_tilde.cpp:174-175):
```cpp
int ver = sv_init(0, samplerate, N_OUT_CHANNELS, SV_INIT_FLAG_USER_AUDIO_CALLBACK
                                                | SV_INIT_FLAG_AUDIO_FLOAT32);
```

**The Issue:** Without `SV_INIT_FLAG_ONE_THREAD`, SunVox may try to manage threading internally, which could conflict with Max's DSP thread management.

**Recommendation:** Add the flag:
```cpp
int ver = sv_init(0, samplerate, N_OUT_CHANNELS,
                 SV_INIT_FLAG_USER_AUDIO_CALLBACK |
                 SV_INIT_FLAG_AUDIO_FLOAT32 |
                 SV_INIT_FLAG_ONE_THREAD);
```

---

## Additional Issues

### 4. **Potential Race Condition in Reinitialization**

In `sv_dsp64()` (lines 159-163), you deinit SunVox but there's no guarantee that the DSP thread isn't still calling `sv_perform64()` with the old state. Max's DSP chain should handle this, but it's worth being aware of.

### 5. **No Error Handling for Buffer Allocation**

Lines 168-169 allocate memory with `new[]` but don't check for allocation failure (though modern C++ would throw `std::bad_alloc`).

### 6. **Unused Variables**

- `x->offset` (line 33) - declared, set in `sv_float()`, but never used
- `x->keep_running` (line 31) - toggled by `sv_bang()` but never checked in audio processing

### 7. **Magic Number in `sv_test()`**

Line 209 in sv_test uses hardcoded module index 7:
```cpp
sv_get_module_ctl_value(0, 7, 1, 2)
```

This assumes the project structure, which is fragile.

---

## Code Quality Observations

### Good Practices
- Proper use of `SV_INIT_FLAG_USER_AUDIO_CALLBACK` for offline processing
- Correct interleaving/de-interleaving logic (once the channel count bug is fixed)
- Resource directory path resolution for bundle resources
- Reinitialization on sample rate/buffer size changes

### Improvement Suggestions

1. **Add slot locking** when calling SunVox API functions that modify state (see test examples using `sv_lock_slot()`/`sv_unlock_slot()`)

2. **Initialize buffers to NULL** in constructor to make cleanup safer:
```cpp
x->in_sv_buffer = NULL;
x->out_sv_buffer = NULL;
```

3. **Better error messages** - when `sv_init()` fails, log the actual error code

4. **Consider double buffering** if you encounter audio glitches during reinitialization

---

## Root Cause of Audio Issues

The primary cause is **Bug #1**: passing the wrong value for `in_channels`. This causes SunVox to:
- Misinterpret the interleaved audio buffer structure
- Read/write audio data at incorrect offsets
- Potentially access out-of-bounds memory or misalign the audio stream

**Expected behavior:** 2 channels × 64 frames = 128 float samples (LRLRLR...)
**Actual behavior:** SunVox thinks it's receiving 64 channels × 64 frames

---

## Recommended Fix Priority

1. **CRITICAL - Fix line 209:** Change `n` to `N_IN_CHANNELS`
2. **HIGH - Add `SV_INIT_FLAG_ONE_THREAD`** to initialization flags
3. **MEDIUM - Add locking** around slot operations where needed
4. **LOW - Code cleanup** (unused variables, error handling)

The first fix alone should resolve your audio quality issues.

---

## Testing Recommendations

After applying fixes:

1. Test with simple audio input to verify clean passthrough
2. Load a SunVox project using the `test` message
3. Monitor for crashes or memory issues
4. Test DSP chain reinit (change sample rate in Max)
5. Verify proper cleanup on object deletion
