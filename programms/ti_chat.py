import ti_hub # pyright: ignore[reportMissingImports]
import ti_system as ts # pyright: ignore[reportMissingImports] 
import ti_innovator as hub # pyright: ignore[reportMissingImports]

print("Type '@all [msg]' or 'sudo spm'...")

def parse_cmd(raw_input):
    if not raw_input: return
    parts = raw_input.split()
    
    if len(parts) == 0: return
    if parts[0] == "@all" and len(parts) > 1:
        msg = " ".join(parts[1:])
        hub.send("@all: " + msg)
    
    elif parts[0] == "sudo":
        if len(parts) < 3:
            print("Usage: sudo spm install [pkg]")
            return
            
        if parts[1] == "spm" and parts[2] == "install":
            if len(parts) > 3:
                pkg = parts[3]
                hub.send("@esp: sudo spm install pkgset " + pkg)
            else:
                print("Fehler: Kein Paket angegeben.")
                
    else:
        hub.send(raw_input)

while True:
    try:
        user_in = input("/documents/~$ ")
        if user_in.lower() in ["exit", "quit"]: break
        parse_cmd(user_in)
    except Exception as e:
        print("Fehler:", e)
