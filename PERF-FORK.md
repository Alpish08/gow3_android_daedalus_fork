# Performance fork (2.1.8-bionic-perf.1)

A fork of [erickmendes22/gow3_android_daedalus_fork](https://github.com/erickmendes22/gow3_android_daedalus_fork)
2.1.8-bionic with GPU performance work and two visual fixes.

> **This fork is not maintained.** It was a one-time project to enjoy my own playthrough more.
> I don't plan to continue development, answer issues or follow upstream updates. For the
> maintained project, use the upstream fork above. Anyone is welcome to take these changes
> upstream or into their own builds under the GPL-2.0 license.

## Results

Tested only on a **Retroid Pocket 6** (Snapdragon 8 Gen 2, Adreno 740, 12 GB RAM) with the
**Turnip 27.0.0-R2** driver at 720p. Same spot and camera, same settings otherwise:

| Scene | Upstream 2.1.8 | This fork |
|---|---|---|
| Heavy room (platform with two chests, ~1,500 draws/frame) | ~35 FPS | ~48–50 FPS |
| Water cave (~3,100 draws/frame) | ~22 FPS | ~33 FPS |

Other devices and drivers are untested. Results will differ.

## Changes

1. **Uniform buffers for small read-only shader data** (`GOW3_UBO`, default on). The renderer
   declared every guest buffer as a storage buffer, so Turnip fetched per-vertex light data with
   `isam` loads and stalls. Read-only U32/F32 buffers of vertex and fragment shaders are now
   uniform buffers, sized per shader variant to the bound size (power of two, 256 B to 64 KB), so
   the driver can place them in the constant file. This is the main gain (about +33% in heavy
   rooms). `GOW3_UBO=0` disables it, `1` limits it to vertex shaders.
   Code: `gpu/shadps4/shader_recompiler/info.h` (`UseUniformBuffer`), `specialization.h`,
   `spirv_emit_context.cpp`, `vk_graphics_pipeline.cpp`, `vk_rasterizer.cpp`; the pipeline cache
   version includes the setting.
2. **Steam / heat-haze fix** for the 480p, 720p and 900p resolution patches
   (`patches/God_of_War_III_Remastered.xml`). The 1440p and higher patches change four size
   values (0x7c46b4, 0x7c46b8, 0x56bb82, 0x60953d) that the lower presets missed, so a distortion
   screen copy stayed 1920 pixels wide. Steam and the "YOU ARE DEAD" text then showed shifted
   screen content. Also broken in upstream.
3. **No anti-aliasing by default** (`GOW3_MSAA=0`, new renderer option). The scene renders
   single-sample instead of 2x MSAA: about +7% in the water cave, with more jagged edges.
   Code: `gpu/shadps4/video_core/amdgpu/gow3_msaa.h` and the sample-count accessors it gates.
4. **Turnip sysmem rendering by default** (`TU_DEBUG=sysmem`): about +3% and steadier frame
   times in detail-heavy rooms. Keep 2 and 3 together: Turnip sysmem with 2x MSAA renders
   still-water reflections as blue blocks.
5. **Hidden menu handle**: the gold pill on the left edge of the game screen is no longer drawn.
   The handle is still there and still opens the in-game menu when tapped.
6. **Louder, fuller game audio** (`src/runtime_audio_bionic.c`). The 7.1 to stereo downmix
   divided the game's main surround mix by 2.7, about 8.7 dB below the sounds the game plays
   through stereo ports (UI sounds like the weapon upgrade), and dropped the LFE channel. It now
   uses a standard downmix (centre and surrounds -3 dB, LFE -6 dB) without the divisor, with a
   soft limiter instead of hard clipping. `GOW3_AUDIO_SURROUND_GAIN` (default 1.0) scales it.
   In the release APK this is in the runtime executable `gow3-probe`, rebuilt from this branch.
7. **Own package id** `com.gow3.android.bionic.fork` and label "GoW III Daedalus (perf fork)", so
   it installs next to the original app. It does not share the original app's data: select the
   game folder again and copy your saves over.

To get the upstream look back (anti-aliasing on, driver default tiling), set `GOW3_MSAA=1` and
`TU_DEBUG=` (empty) at the top of `run-bionic.sh`.

## Release APK

The release APK is the upstream 2.1.8-bionic APK repacked with these changes: the GPU runtime
library (`libgow3gpu.so`) and the runtime executable (`gow3-probe`) are built from this branch, and `run-bionic.sh` and the patch file come
from this branch. The launcher changes (5 and 7) were applied to the upstream launcher binary;
the matching source changes are in `android-launcher/`. The APK is signed with this fork's own
key, so it cannot update or replace the original app.

No game files are included. You need your own copy of God of War III Remastered (CUSA01623).

## License

GPL-2.0, as upstream. All upstream credits and third-party notices are unchanged.
