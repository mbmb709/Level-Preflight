# Level Preflight

Level Preflight is a small Geode utility for Geometry Dash creators. It adds a
read-only readiness report to the page of each locally created level.

## Why it exists

The normal upload flow tells a creator whether a level can be uploaded, but it
does not collect the common review points in one place. Level Preflight keeps
that review short and visible without pretending that software can judge level
quality.

## What it checks

- Level name
- Saved level content and object count
- Verification state
- Description presence
- User coin verification
- Audio selection
- Public, unlisted, or friends-only visibility
- Low Detail Mode availability

It also reminds the creator to check gameplay readability, credits, audio use,
performance on another device, and final upload options.

## Privacy and behavior

- No network requests
- No analytics
- No account data collection
- No level changes
- No upload blocking
- No automatic claims about level quality or rating

## Supported targets

- Windows 64-bit
- Android 32-bit and 64-bit
- macOS Intel and Apple Silicon
- iOS and iPadOS ARM64
- Linux through the Windows build under Wine

The project targets Geometry Dash 2.2081 and Geode 5.8.2.

## Build

The recommended route is the included GitHub Actions workflow. For a configured
local Geode SDK:

```sh
geode build --ninja
```

See [BUILD_VERIFICATION.md](BUILD_VERIFICATION.md) for the checks completed on
this exact source version and the platform tests that are still required.

## Originality

The implementation and artwork in this repository were created for this
project. No source code or assets were copied from another Geometry Dash mod.
The only runtime dependency is the official `geode.node-ids` infrastructure.
See [ORIGINALITY.md](ORIGINALITY.md) for the feature search performed before
development.

## Important development note

This initial implementation was produced with AI assistance. It is a learning
and review-ready project, not something that should be represented as purely
human-written. Before any Geode Index submission, a human C++/Geode developer
must review, understand, test, and take responsibility for the complete codebase.

## License

MIT License. Copyright (c) 2026 mbmb709.
