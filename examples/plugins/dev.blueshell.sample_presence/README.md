# Sample Presence Plugin

A minimal BlueShell v1 plugin that:
- Logs init/shutdown events.
- Every ~3 seconds, calls `RequestEditorContext()` and logs the current level state.

## Build

From the repository root:

```bash
./examples/plugins/dev.blueshell.sample_presence/build.sh
```

## Install into BlueShell runtime plugin directory

```bash
rm -rf ./plugins/dev.blueshell.sample_presence
mkdir -p ./plugins/dev.blueshell.sample_presence
cp ./examples/plugins/dev.blueshell.sample_presence/plugin.yaml ./plugins/dev.blueshell.sample_presence/
cp -R ./examples/plugins/dev.blueshell.sample_presence/bin ./plugins/dev.blueshell.sample_presence/
```

Then open BlueShell, go to `Window -> Show Plugin Manager`, `Rescan`, grant permissions, and enable/load the plugin.
