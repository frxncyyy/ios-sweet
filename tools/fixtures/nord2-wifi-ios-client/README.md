# IOUserClient host regression

Run `python3 tools/test-nord2-wifi-ios-client.py` after the runtime compatibility source is integrated. The default uses actual canonical shim sources in a temporary xnu/iokit/Kernel/arm and xnu/bsd/dev layout. Optional `--candidate PATH` overlays a reviewed private source set before promotion; `--log PATH` records synthetic metadata. No device IO or private board data is used.

The fixtures mock IOKit task, memory descriptor, locks and async rights while compiling the actual client. They cover actual wire bounds, OOL prepare/read/write/complete and cursor commit on delivery, scalar/body output, source-backed no-association error, truthful power idempotence, mapped event publication, real snapshot conversion, canceled generations, client retention, owner checks and concurrent close. They are not proof of live IOKit scheduling or Settings UI visibility.
