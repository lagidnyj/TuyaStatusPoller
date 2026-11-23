# Environment and return notes

Snapshot: 2026-11-23

This note records how to return to the project after deleting the current
Codespace. It does not contain credentials, local device keys, or a full backup
of the Codespace.

## Durable project location

- GitHub repository: https://github.com/lagidnyj/TuyaStatusPoller
- Local project directory in this Codespace: `/workspaces/codespaces-blank/TuyaStatusPoller`
- Branch observed: `main`, tracking `origin/main`
- Commit observed: `cd571126c9cf5bf42c810fb3cb37fa1a0ec19f4c`
  (`Quote JSON string values correctly`, 2026-11-15)
- At the time of this snapshot, the repository showed no tracked working-tree
  changes and no ahead/behind marker relative to its locally recorded
  `origin/main` ref. The remote was not fetched as part of this snapshot.
- The parent directory `/workspaces/codespaces-blank` is not itself a Git
  repository.

To return to the project:

1. Open the repository URL above and use **Code > Codespaces** to create or open
   a Codespace for `main`.
2. Or, in an authenticated environment with GitHub CLI, run:

   ```sh
   gh repo clone lagidnyj/TuyaStatusPoller
   ```

3. If a local clone already exists, enter its directory and check
   `git status --short --branch` before making changes.

Deleting a Codespace removes its local, unpushed state. This Markdown file is
inside the project repository, but it will not be on GitHub until it is
committed and pushed. Before deleting the Codespace, save this file to GitHub
(or download a copy elsewhere) and separately back up any needed untracked or
ignored files.

## Project summary

`TuyaStatusPoller` is an ESP32 Arduino project for polling Tuya devices on a
local network, including the CW24 / Atorch power meter. It uses
`LynkTuyaLocal`, emits JSON Lines to Serial, and can POST status to
Micro-IoT-Cloud. The README documents an ESP32-C3 board, polling every 30
seconds, and Serial Monitor at 115200 baud. See `README.md` for setup and
behavior details.

The project files are in the repository root, including `TuyaStatusPoller.ino`,
device configuration, Tuya polling, DPS formatting, JSON output, and cloud
upload modules.

## Local files and secrets to handle before deleting

- `secrets.h` and `cloud_secrets.h` are ignored by Git (`.gitignore`). They
  contain local configuration and are not present in the GitHub repository.
  Their contents were deliberately not read or copied into this note.
- If those settings are needed later, back them up separately using a secure
  method (for example, a password manager or encrypted storage). Do not commit
  real Wi-Fi credentials, Tuya Local Keys, device tokens, or other secrets.
- The repository includes `secrets.example.h` and
  `cloud_secrets.example.h` as non-secret templates.
- The workspace parent also contained `/workspaces/codespaces-blank/dps` and
  `/workspaces/codespaces-blank/poll.py`. They are outside the Git repository
  and were not inspected or backed up here. Preserve them separately if they
  are still needed.

## Codespace snapshot

- Workspace: `/workspaces/codespaces-blank`
- Project repository: `/workspaces/codespaces-blank/TuyaStatusPoller`
- Environment observed: Linux x86_64, Ubuntu 22.04-based Codespace kernel
  (`6.8.0-1070-azure`)
- The Codespace hostname and installed runtime/tool versions are temporary
  environment details and were not recorded as durable access credentials.

## Copilot chat reference

- Current chat title: `збереження контексту в md файл`
- Session reference: `ahp-session:/95c4bf0f-0cc2-49df-8432-c9978bb2da90`
- The chat history at the time of this note only contained the request to
  preserve context. This reference may help locate it in Copilot Chat history,
  but it is not a substitute for keeping the repository and local files.
